#include "CurlHttpResponse.h"

namespace Dobot
{
    CurlHttpResponse::CurlHttpResponse(std::shared_ptr<CurlHttpRequest> sptr)
    {
        m_pHttpRequest = sptr;
        m_bSucceed = false;
        m_iResponseCode = 0;
    }

    CurlHttpResponse::~CurlHttpResponse()
    {

    }

    std::shared_ptr<CurlHttpRequest> CurlHttpResponse::GetHttpRequest()
    {
        return m_pHttpRequest;
    }

    bool CurlHttpResponse::IsSucceed() const
    {
        return m_bSucceed;
    }

    void CurlHttpResponse::SetSucceed(bool value)
    {
        m_bSucceed = value;
    }

    std::vector<char>* CurlHttpResponse::GetResponseData()
    {
        return &m_vcResponseData;
    }

    void CurlHttpResponse::SetResponseData(std::vector<char>* data)
    {
        m_vcResponseData = *data;
    }

    std::vector<char>* CurlHttpResponse::GetResponseHeader()
    {
        return &vc_vcResponseHeader;
    }

    void CurlHttpResponse::SetResponseHeader(std::vector<char>* data)
    {
        vc_vcResponseHeader = *data;
    }

    long CurlHttpResponse::GetResponseCode()
    {
        return m_iResponseCode;
    }

    void CurlHttpResponse::SetResponseCode(long value)
    {
        m_iResponseCode = value;
    }

    const char* CurlHttpResponse::GetErrorMessage()
    {
        return m_strErrMsg.c_str();
    }

    void CurlHttpResponse::SetErrorMessage(const char* pszMsg)
    {
        m_strErrMsg.clear();
        m_strErrMsg.assign(pszMsg);
    }
}
