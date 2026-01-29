#ifndef CURL_HTTP_REQUEST_H
#define CURL_HTTP_REQUEST_H

#include <string>
#include <vector>
#include <functional>

namespace Dobot
{
    class CurlHttpClient;
    class CurlHttpResponse;

    class CurlHttpRequest
    {
    public:
        enum Type
        {
            GET,
            POST,
            PUT,
//            DELETE,
            UNKNOWN,
        };

        CurlHttpRequest();
        CurlHttpRequest(const CurlHttpRequest&) = default;
        CurlHttpRequest(CurlHttpRequest&&) = default;
        CurlHttpRequest& operator=(const CurlHttpRequest&) = default;
        CurlHttpRequest& operator=(CurlHttpRequest&&) = default;
        ~CurlHttpRequest();

        void SetRequestType(Type type);
        Type GetRequestType() const;

        void SetUrl(const char* pszUrl);
        const char* GetUrl() const;

        void SetConnectTimeout(int iSeconds);
        int GetConnectTimeout() const;

        void SetReadTimeout(int iSeconds);
        int GetReadTimeout() const;

        void SetRequestData(const char* pData, size_t nSize);
        const char* GetRequestData() const;
        size_t GetRequestDataSize() const;

        void AddHeader(const std::vector<std::string>& vcHeader);
        void AddHeader(const char* pszName, const char* pszValue);
        std::vector<std::string> GetHeaders() const;
        void ClearHeaders();

        void SetTagId(int64_t id);
        int64_t GetTagId() const;

        void SetUserData(void* pUserData);
        void* GetUserData();

        void SetResponseCallback(const std::function<void(CurlHttpClient* pClient, CurlHttpResponse* pResponse)>& callback);
        std::function<void(CurlHttpClient* pClient, CurlHttpResponse* pResponse)> GetResponseCallback() const;

    private:
        Type m_requestType;
        std::string m_strUrl;
        int m_iConnectTimeoutSeconds;
        int m_iReadTimeoutSeconds;
        std::vector<char> m_vcRequestData;
        std::vector<std::string> m_vcRequestHeader;
        int64_t m_iTag;
        void* m_pUserData;
        std::function<void(CurlHttpClient* pClient, CurlHttpResponse* pResponse)> m_OnResponseCallback;
    };
}
#endif // CURL_HTTP_REQUEST_H
