#ifndef CURL_HTTP_CLIENT_H
#define CURL_HTTP_CLIENT_H

#include <thread>
#include <condition_variable>
#include <atomic>
#include <list>

#include "CurlHttpRequest.h"
#include "CurlHttpResponse.h"

namespace Dobot
{
    class CurlHttpClient
    {
    public:

        static const int RESPONSE_BUFFER_SIZE = 512;

        ~CurlHttpClient();
        CurlHttpClient(const CurlHttpClient&) = delete;
        CurlHttpClient(CurlHttpClient&&) = delete;
        CurlHttpClient& operator=(const CurlHttpClient&) = delete;
        CurlHttpClient& operator=(CurlHttpClient&&) = delete;

        static CurlHttpClient* GetInstance();
        static void DestroyInstance();

        /*
        * enable cookies support,if pszCookieFile is nullptr,do not record cookies
        */
        void SetCookiesFileFullPath(const char* pszCookieFile);
        std::string GetCookiesFileFullPath();

        /**
         * Set root certificate path for SSL verification.
         * strCaFile a full path of root certificate. if it is empty, SSL verification is disabled.
         */
        void SetSSLVerification(const std::string& strCaFile);

        void SetProxy(const std::string& ip,int port);
        const std::string& getProxy();
        /**
         * Get the ssl CA filename
         */
        const std::string& GetSSLVerification();

        /*Add a request to task queue*/
        void Send(std::shared_ptr<CurlHttpRequest> request);

        /**
         * Immediate send a request
         *request a HttpRequest object, which includes url, response callback etc.
         * please make sure request->m_vcRequestData is clear before calling "sendImmediate" here.
        */
        void SendImmediate(std::shared_ptr<CurlHttpRequest> request);

        /**
         * Clears the pending http responses and http requests
         * If defined, the method uses the ClearRequestPredicate and ClearResponsePredicate
         * to check for each request/response which to delete
         */
        void ClearResponseAndRequestQueue(); 

    private:
        CurlHttpClient();

        bool InitThreadSemphore();
        void NetworkThread();
        void NetworkThreadAlone(std::shared_ptr<CurlHttpRequest> request, std::shared_ptr<CurlHttpResponse> response);

        /*add a response to a task queue*/
        void AddTask(std::shared_ptr<CurlHttpResponse> response);

        /** Poll function called from main thread to dispatch callbacks when http requests finished **/
        void DispatchResponseCallbacks();

        void ProcessResponse(std::shared_ptr<CurlHttpResponse> response, char* responseMessage);
        void IncreaseThreadCount();
        void DecreaseThreadCountAndMayDeleteThis();

    private:
        std::atomic_bool m_isInit;

        std::atomic_int  m_threadCount;
        std::atomic_bool m_bStopThd;

        std::list<std::shared_ptr<CurlHttpRequest>>  m_vcRequestQueue;
        std::mutex m_mtxRequestQueue;
        std::condition_variable m_condRequestQueue;

        std::list<std::shared_ptr<CurlHttpResponse>> m_vcResponseQueue;
        std::mutex m_mtxResponseQueue;
        std::condition_variable m_condResponseQueue;


        std::string m_strSslCaFilename;
        std::mutex m_mtxSslCaFileMutex;

        std::string m_proxytIpAndPort;

        std::string m_strCookieFile;
        std::mutex m_mtxCookieFile;

        char m_szResponseMessage[RESPONSE_BUFFER_SIZE];
    };
}

#endif // CURL_HTTP_CLIENT_H
