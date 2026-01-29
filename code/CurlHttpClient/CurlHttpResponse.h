#ifndef CURL_HTTP_RESPONSE_H
#define CURL_HTTP_RESPONSE_H

#include <memory>

#include "CurlHttpRequest.h"

namespace Dobot
{
    class CurlHttpResponse
    {
    public:
        CurlHttpResponse(std::shared_ptr<CurlHttpRequest> sptr);
        ~CurlHttpResponse();

        std::shared_ptr<CurlHttpRequest> GetHttpRequest();

        bool IsSucceed() const;
        void SetSucceed(bool value);

        std::vector<char>* GetResponseData();
        void SetResponseData(std::vector<char>* data);

        std::vector<char>* GetResponseHeader();
        void SetResponseHeader(std::vector<char>* data);

        long GetResponseCode();
        void SetResponseCode(long value);

        const char* GetErrorMessage();
        void SetErrorMessage(const char* pszMsg);

        private:
            std::shared_ptr<CurlHttpRequest> m_pHttpRequest;
            bool m_bSucceed;
            std::vector<char> m_vcResponseData;
            std::vector<char> vc_vcResponseHeader;
            long m_iResponseCode;
            std::string m_strErrMsg; //if m_iResponseCode != 200, please read m_strErrMsg to find the reason
    };
}

#endif // CURL_HTTP_RESPONSE_H
