#include "CurlHttpClient.h"

#include <queue>
#include <errno.h>

#include "include/curl/curl.h"

namespace Dobot
{
    typedef size_t (*write_callback)(void *ptr, size_t size, size_t nmemb, void *stream);

    // Callback function used by libcurl for collect response data
    static size_t writeData(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        std::vector<char> *recvBuffer = (std::vector<char>*)stream;
        size_t sizes = size * nmemb;
        
        // add data to the end of recvBuffer
        // write data maybe called more than once in a single request
        recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr+sizes);
        
        return sizes;
    }

    // Callback function used by libcurl for collect header data
    static size_t writeHeaderData(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        std::vector<char> *recvBuffer = (std::vector<char>*)stream;
        size_t sizes = size * nmemb;
        
        // add data to the end of recvBuffer
        // write data maybe called more than once in a single request
        recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr+sizes);
        
        return sizes;
    }

    //Configure curl's timeout property
    static bool configureCURL(CurlHttpClient* pHttpClient, std::shared_ptr<CurlHttpRequest> request, 
                              CURL* pCurlHandle, char* pszErrMsgBuffer)
    {
        if (!pHttpClient || !pCurlHandle || !pszErrMsgBuffer) 
        {
            return false;
        }
        
        CURLcode code = curl_easy_setopt(pCurlHandle, CURLOPT_ERRORBUFFER, pszErrMsgBuffer);
        if (code != CURLE_OK) 
        {
            return false;
        }
        code = curl_easy_setopt(pCurlHandle, CURLOPT_TIMEOUT, request->GetReadTimeout());
        if (code != CURLE_OK) 
        {
            return false;
        }
        code = curl_easy_setopt(pCurlHandle, CURLOPT_CONNECTTIMEOUT, request->GetConnectTimeout());
        if (code != CURLE_OK) 
        {
            return false;
        }

        std::string strSslCaFile = pHttpClient->GetSSLVerification();
        if (strSslCaFile.empty()) 
        {
            curl_easy_setopt(pCurlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(pCurlHandle, CURLOPT_SSL_VERIFYHOST, 0L);
        } 
        else 
        {
            curl_easy_setopt(pCurlHandle, CURLOPT_SSL_VERIFYPEER, 1L);
            curl_easy_setopt(pCurlHandle, CURLOPT_SSL_VERIFYHOST, 2L);
            curl_easy_setopt(pCurlHandle, CURLOPT_CAINFO, strSslCaFile.c_str());
        }
        std::string proxyIpAndPort = pHttpClient->getProxy();

        curl_easy_setopt(pCurlHandle, CURLOPT_PROXY, proxyIpAndPort.c_str());
        
        // FIXED #3224: The subthread of CCHttpClient interrupts main thread if timeout comes.
        // Document is here: http://curl.haxx.se/libcurl/c/curl_easy_setopt.html#CURLOPTNOSIGNAL 
        curl_easy_setopt(pCurlHandle, CURLOPT_NOSIGNAL, 1L);

        curl_easy_setopt(pCurlHandle, CURLOPT_ACCEPT_ENCODING, "");

        return true;
    }

    class CURLRaii
    {
        /// Instance of CURL
        CURL* m_pCurl;
        /// Keeps custom header data
        curl_slist* m_pHeaders;
    public:
        CURLRaii()
            : m_pCurl(curl_easy_init())
            , m_pHeaders(nullptr)
        {
        }

        ~CURLRaii()
        {
            if (m_pCurl)
                curl_easy_cleanup(m_pCurl);
            /* free the linked list for header data */
            if (m_pHeaders)
                curl_slist_free_all(m_pHeaders);
        }

        template <class T>
        bool setOption(CURLoption option, T data)
        {
            return CURLE_OK == curl_easy_setopt(m_pCurl, option, data);
        }

        /**
         * @brief Inits CURL instance for common usage
         * @param request Null not allowed
         * @param callback Response write callback
         * @param stream Response write stream
         */
        bool init(CurlHttpClient* pHttpClient, std::shared_ptr<CurlHttpRequest> request, write_callback callback, 
                void* stream, write_callback headerCallback, void* headerStream, char* errorBuffer)
        {
            if (!m_pCurl)
                return false;
            if (!configureCURL(pHttpClient, request, m_pCurl, errorBuffer))
                return false;

            /* get custom header data (if set) */
            std::vector<std::string> headers = request->GetHeaders();
            if(!headers.empty())
            {
                /* append custom headers one by one */
                for (const auto& header : headers)
                    m_pHeaders = curl_slist_append(m_pHeaders, header.c_str());
                /* set custom headers for curl */
                if (!setOption(CURLOPT_HTTPHEADER, m_pHeaders))
                    return false;
            }

            std::string strCookieFile = pHttpClient->GetCookiesFileFullPath();
            if (!strCookieFile.empty()) 
            {
                if (!setOption(CURLOPT_COOKIEFILE, strCookieFile.c_str())) 
                {
                    return false;
                }
                if (!setOption(CURLOPT_COOKIEJAR, strCookieFile.c_str()))
                {
                    return false;
                }
            }

            return setOption(CURLOPT_URL, request->GetUrl())
                    && setOption(CURLOPT_WRITEFUNCTION, callback)
                    && setOption(CURLOPT_WRITEDATA, stream)
                    && setOption(CURLOPT_HEADERFUNCTION, headerCallback)
                    && setOption(CURLOPT_HEADERDATA, headerStream);
            
        }

        /// @param responseCode Null not allowed
        bool perform(long *responseCode)
        {
            if (CURLE_OK != curl_easy_perform(m_pCurl))
                return false;
            CURLcode code = curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, responseCode);
            if (code != CURLE_OK || !(*responseCode >= 200 && *responseCode < 300)) 
            {
                printf("Curl curl_easy_getinfo failed: %s", curl_easy_strerror(code));
                return false;
            }
            // Get some mor data.
            
            return true;
        }
    };

    //Process Get Request
    static int processGetTask(CurlHttpClient* pHttpClient, std::shared_ptr<CurlHttpRequest> request, 
                            write_callback callback, 
                            void* stream, long* responseCode, write_callback headerCallback, 
                            void* headerStream, char* errorBuffer)
    {
        CURLRaii curl;
        bool ok = curl.init(pHttpClient, request, callback, stream, headerCallback, headerStream, errorBuffer)
                && curl.setOption(CURLOPT_FOLLOWLOCATION, true)
                && curl.perform(responseCode);
        return ok ? 0 : 1;
    }

    //Process POST Request
    static int processPostTask(CurlHttpClient* pHttpClient, std::shared_ptr<CurlHttpRequest> request, 
                              write_callback callback, 
                              void* stream, long* responseCode, write_callback headerCallback, 
                              void* headerStream, char* errorBuffer)
    {
        CURLRaii curl;
        bool ok = curl.init(pHttpClient, request, callback, stream, headerCallback, headerStream, errorBuffer)
                && curl.setOption(CURLOPT_POST, 1)
                && curl.setOption(CURLOPT_POSTFIELDS, request->GetRequestData())
                && curl.setOption(CURLOPT_POSTFIELDSIZE, request->GetRequestDataSize())
                && curl.perform(responseCode);
        return ok ? 0 : 1;
    }

    //Process PUT Request
    static int processPutTask(CurlHttpClient* pHttpClient, std::shared_ptr<CurlHttpRequest> request, 
                             write_callback callback, 
                             void* stream, long* responseCode, write_callback headerCallback, 
                             void* headerStream, char* errorBuffer)
    {
        CURLRaii curl;
        bool ok = curl.init(pHttpClient, request, callback, stream, headerCallback, headerStream, errorBuffer)
                && curl.setOption(CURLOPT_CUSTOMREQUEST, "PUT")
                && curl.setOption(CURLOPT_POSTFIELDS, request->GetRequestData())
                && curl.setOption(CURLOPT_POSTFIELDSIZE, request->GetRequestDataSize())
                && curl.perform(responseCode);
        return ok ? 0 : 1;
    }

    //Process DELETE Request
    static int processDeleteTask(CurlHttpClient* pHttpClient, std::shared_ptr<CurlHttpRequest> request, 
                               write_callback callback, 
                               void* stream, long* responseCode, write_callback headerCallback, 
                               void* headerStream, char* errorBuffer)
    {
        CURLRaii curl;
        bool ok = curl.init(pHttpClient, request, callback, stream, headerCallback, headerStream, errorBuffer)
                && curl.setOption(CURLOPT_CUSTOMREQUEST, "DELETE")
                && curl.setOption(CURLOPT_FOLLOWLOCATION, true)
                && curl.perform(responseCode);
        return ok ? 0 : 1;
    }

    class CurlGlobalEnv
    {
    public:
        CurlGlobalEnv()
        {
            curl_global_init(CURL_GLOBAL_DEFAULT);
        }

        ~CurlGlobalEnv()
        {
            curl_global_cleanup();
        }
    };
    static CurlGlobalEnv g_CurlGlobalEnv; //do not used,but init curl env
    
    /*******************************************************************************************************************************************/
    /*******************************************************************************************************************************************/
    /*******************************************************************************************************************************************/
    static CurlHttpClient* g_pCurlHttpClient = nullptr; // pointer to singleton
    static std::mutex g_mtxCurlHttpClient;

    CurlHttpClient::CurlHttpClient()
    {
        printf("In the constructor of HttpClient!");

        m_isInit.store(false);
        m_bStopThd.store(false);
        m_threadCount.store(0);

        memset(m_szResponseMessage, 0, RESPONSE_BUFFER_SIZE * sizeof(char));

        IncreaseThreadCount();
    }

    CurlHttpClient::~CurlHttpClient()
    {
    }

    CurlHttpClient* CurlHttpClient::GetInstance()
    {
        if (g_pCurlHttpClient == nullptr) {
            std::lock_guard<std::mutex> guard(g_mtxCurlHttpClient);
            if (g_pCurlHttpClient == nullptr)
            {
                g_pCurlHttpClient = new CurlHttpClient();
            }
        }

        return g_pCurlHttpClient;
    }

    void CurlHttpClient::DestroyInstance()
    {
        if (nullptr == g_pCurlHttpClient)
        {
            printf("CurlHttpClient singleton is nullptr");
            return ;
        }
        printf("CurlHttpClient::destroyInstance begin");
        CurlHttpClient* thiz = nullptr;
        {
            std::lock_guard<std::mutex> guard(g_mtxCurlHttpClient);
            thiz = g_pCurlHttpClient;
            g_pCurlHttpClient = nullptr;
        }

        thiz->m_bStopThd.store(true);

        thiz->m_mtxRequestQueue.lock();
        thiz->m_vcRequestQueue.push_front(std::shared_ptr<CurlHttpRequest>());
        thiz->m_condRequestQueue.notify_all();
        thiz->m_mtxRequestQueue.unlock();

        thiz->m_mtxResponseQueue.lock();
        thiz->m_vcResponseQueue.push_back(std::shared_ptr<CurlHttpResponse>());
        thiz->m_condResponseQueue.notify_all();
        thiz->m_mtxResponseQueue.unlock();

        thiz->DecreaseThreadCountAndMayDeleteThis();

        printf("CurlHttpClient::destroyInstance() finished!");
    }


    void CurlHttpClient::SetCookiesFileFullPath(const char* pszCookieFile)
    {
        std::lock_guard<std::mutex> lock(m_mtxCookieFile); 
        if (pszCookieFile)
        {
            m_strCookieFile = std::string(pszCookieFile);
        }
        else
        {
            m_strCookieFile.clear();
        }
    }

    std::string CurlHttpClient::GetCookiesFileFullPath()
    {
        std::lock_guard<std::mutex> lock(m_mtxCookieFile);
        return m_strCookieFile;
    }

    void CurlHttpClient::SetSSLVerification(const std::string& strCaFile)
    {
        std::lock_guard<std::mutex> lock(m_mtxSslCaFileMutex);
        m_strSslCaFilename = strCaFile;
    }

    void CurlHttpClient::SetProxy(const std::string &ip, int port)
    {
        m_proxytIpAndPort = ip+":"+std::to_string(port);

    }

    const std::string &CurlHttpClient::getProxy()
    {
        return m_proxytIpAndPort;
    }

    const std::string& CurlHttpClient::GetSSLVerification()
    {
        std::lock_guard<std::mutex> lock(m_mtxSslCaFileMutex);
        return m_strSslCaFilename;
    }

    bool CurlHttpClient::InitThreadSemphore()
    {
        if (m_isInit.load())
        {
            return true;
        }
        else
        {
            m_isInit.store(true);

            auto thdReq = std::thread(&CurlHttpClient::NetworkThread, this);
            thdReq.detach();

            auto thdRes = std::thread(&CurlHttpClient::DispatchResponseCallbacks, this);
            thdRes.detach();
        }
        return true;
    }


    void CurlHttpClient::Send(std::shared_ptr<CurlHttpRequest> request)
    {
        if (false == InitThreadSemphore())
        {
            return;
        }
        if (!request)
        {
            return;
        }

        std::lock_guard<std::mutex> guard(m_mtxRequestQueue);
        m_vcRequestQueue.push_back(request);
        m_condRequestQueue.notify_one();
    }

    void CurlHttpClient::SendImmediate(std::shared_ptr<CurlHttpRequest> request)
    {
        if(!request)
        {
            return;
        }

        // Create a HttpResponse object, the default setting is http access failed
        std::shared_ptr<CurlHttpResponse> response(new (std::nothrow) CurlHttpResponse(request));
        auto t = std::thread(&CurlHttpClient::NetworkThreadAlone, this, request, response);
        t.detach();
    }

    void CurlHttpClient::ClearResponseAndRequestQueue()
    {
        {
            std::lock_guard<std::mutex> guard(m_mtxRequestQueue);
            m_vcRequestQueue.clear();
        }

        {
            std::lock_guard<std::mutex> guard(m_mtxResponseQueue);
            m_vcResponseQueue.clear();
        }
    }

    void CurlHttpClient::NetworkThread()
    {
        IncreaseThreadCount();

        while (!m_bStopThd.load())
        {
            std::shared_ptr<CurlHttpRequest> request;

            // step 1: send http request if the requestQueue isn't empty
            {
                std::unique_lock<std::mutex> lock(m_mtxRequestQueue);
                while (m_vcRequestQueue.empty() && !m_bStopThd.load())
                {
                    m_condRequestQueue.wait(lock);
                }
                if (m_vcRequestQueue.size()>0)
                {
                    request = m_vcRequestQueue.front();
                    m_vcRequestQueue.pop_front();
                }
            }

            // step 2: libcurl sync access
            if (request && !m_bStopThd.load())
            {
                // Create a HttpResponse object, the default setting is http access failed
                std::shared_ptr<CurlHttpResponse> response(new (std::nothrow) CurlHttpResponse(request));

                ProcessResponse(response, m_szResponseMessage);

                // add response packet into queue
                AddTask(response);
            }
        }
        
        // cleanup: if worker thread received quit signal, clean up un-completed request queue
        m_mtxRequestQueue.lock();
        m_vcRequestQueue.clear();
        m_mtxRequestQueue.unlock();

        m_condRequestQueue.notify_one();

        DecreaseThreadCountAndMayDeleteThis();
    }

    void CurlHttpClient::NetworkThreadAlone(std::shared_ptr<CurlHttpRequest> request, std::shared_ptr<CurlHttpResponse> response)
    {
        IncreaseThreadCount();

        char responseMessage[RESPONSE_BUFFER_SIZE] = { 0 };
        ProcessResponse(response, responseMessage);

        const auto& callback = request->GetResponseCallback();
        if (callback != nullptr)
        {
            callback(this, response.get());
        }

        DecreaseThreadCountAndMayDeleteThis();
    }

    void CurlHttpClient::AddTask(std::shared_ptr<CurlHttpResponse> response)
    {
        std::lock_guard<std::mutex> guard(m_mtxResponseQueue);
        m_vcResponseQueue.push_back(response);
        m_condResponseQueue.notify_one();
    }

    void CurlHttpClient::DispatchResponseCallbacks()
    {
        IncreaseThreadCount();

        while (!m_bStopThd.load())
        {
            std::shared_ptr<CurlHttpResponse> response;

            {
                std::unique_lock<std::mutex> guard(m_mtxResponseQueue);
                while (m_vcResponseQueue.empty() && !m_bStopThd.load())
                {
                    m_condResponseQueue.wait(guard);
                }
                if (m_vcResponseQueue.size()>0)
                {
                    response = m_vcResponseQueue.front();
                    m_vcResponseQueue.pop_front();
                }
            }
        
            if (response && !m_bStopThd.load())
            {
                std::shared_ptr<CurlHttpRequest> request = response->GetHttpRequest();
                const auto& callback = request->GetResponseCallback();

                if (callback != nullptr)
                {
                    callback(this, response.get());
                }
            }
        }

        m_mtxResponseQueue.lock();
        m_vcResponseQueue.clear();
        m_mtxResponseQueue.unlock();

        m_condResponseQueue.notify_one();

        DecreaseThreadCountAndMayDeleteThis();
    }

    void CurlHttpClient::ProcessResponse(std::shared_ptr<CurlHttpResponse> response, char* responseMessage)
    {
        auto request = response->GetHttpRequest();
        long responseCode = -1;
        int retValue = 0;

        // Process the request -> get response packet
        switch (request->GetRequestType())
        {
        case CurlHttpRequest::Type::GET: // HTTP GET
            retValue = processGetTask(this, request,
                writeData,
                response->GetResponseData(),
                &responseCode,
                writeHeaderData,
                response->GetResponseHeader(),
                responseMessage);
            break;

        case CurlHttpRequest::Type::POST: // HTTP POST
            retValue = processPostTask(this, request,
                writeData,
                response->GetResponseData(),
                &responseCode,
                writeHeaderData,
                response->GetResponseHeader(),
                responseMessage);
            break;

        case CurlHttpRequest::Type::PUT:
            retValue = processPutTask(this, request,
                writeData,
                response->GetResponseData(),
                &responseCode,
                writeHeaderData,
                response->GetResponseHeader(),
                responseMessage);
            break;

//        case CurlHttpRequest::Type::DELETE:
//            retValue = processDeleteTask(this, request,
//                writeData,
//                response->GetResponseData(),
//                &responseCode,
//                writeHeaderData,
//                response->GetResponseHeader(),
//                responseMessage);
//            break;

        default:
            printf("CCHttpClient: unknown request type, only GET, POST, PUT or DELETE is supported");
            break;
        }

        // write data to HttpResponse
        response->SetResponseCode(responseCode);
        if (retValue != 0)
        {
            response->SetSucceed(false);
            response->SetErrorMessage(responseMessage);
        }
        else
        {
            response->SetSucceed(true);
        }
    }

    void CurlHttpClient::IncreaseThreadCount()
    {
        ++m_threadCount;
    }

    void CurlHttpClient::DecreaseThreadCountAndMayDeleteThis()
    {
        bool needDeleteThis = false;

        int iCount = --m_threadCount;
        if (0 == iCount)
        {
            needDeleteThis = true;
        }
        if (needDeleteThis)
        {
            delete this;
        }
    }
}
