#include "CurlHttpRequest.h"

namespace Dobot
{
    CurlHttpRequest::CurlHttpRequest()
    {
        m_requestType = Type::UNKNOWN;
        m_iConnectTimeoutSeconds = 30;
        m_iReadTimeoutSeconds = 60;
        m_iTag = 0;
        m_pUserData = nullptr;
        m_OnResponseCallback = nullptr;
    }

    CurlHttpRequest::~CurlHttpRequest()
    {

    }

    void CurlHttpRequest::SetRequestType(CurlHttpRequest::Type type)
    {
        m_requestType = type;
    }

    CurlHttpRequest::Type CurlHttpRequest::GetRequestType() const
    {
        return m_requestType;
    }

    void CurlHttpRequest::SetUrl(const char* pszUrl)
    {
        m_strUrl = pszUrl;
    }

    const char* CurlHttpRequest::GetUrl() const
    {
        return m_strUrl.c_str();
    }

    void CurlHttpRequest::SetConnectTimeout(int iSeconds)
    {
        m_iConnectTimeoutSeconds = iSeconds;
    }

    int CurlHttpRequest::GetConnectTimeout() const
    {
        return m_iConnectTimeoutSeconds;
    }

    void CurlHttpRequest::SetReadTimeout(int iSeconds)
    {
        m_iReadTimeoutSeconds = iSeconds;
    }

    int CurlHttpRequest::GetReadTimeout() const
    {
        return m_iReadTimeoutSeconds;
    }

    void CurlHttpRequest::SetRequestData(const char* pData, size_t nSize)
    {
        m_vcRequestData.assign(pData, pData + nSize);
    }

    const char* CurlHttpRequest::GetRequestData() const
    {
        if(!m_vcRequestData.empty())
        {
            return &(m_vcRequestData.front());
        }
        return "";
    }

    size_t CurlHttpRequest::GetRequestDataSize() const
    {
        return m_vcRequestData.size();
    }

    void CurlHttpRequest::AddHeader(const std::vector<std::string>& vcHeader)
    {
        m_vcRequestHeader.insert(m_vcRequestHeader.end(), vcHeader.begin(), vcHeader.end());
    }

    void CurlHttpRequest::AddHeader(const char* pszName, const char* pszValue)
    {
        std::string str(pszName);
        str.append(":");
        str.append(pszValue);
        m_vcRequestHeader.push_back(str);
    }

    std::vector<std::string> CurlHttpRequest::GetHeaders() const
    {
        return m_vcRequestHeader;
    }

    void CurlHttpRequest::ClearHeaders()
    {
        m_vcRequestHeader.clear();
    }

    void CurlHttpRequest::SetTagId(int64_t id)
    {
        m_iTag = id;
    }

    int64_t CurlHttpRequest::GetTagId() const
    {
        return m_iTag;
    }

    void CurlHttpRequest::SetUserData(void* pUserData)
    {
        m_pUserData = pUserData;
    }

    void* CurlHttpRequest::GetUserData()
    {
        return m_pUserData;
    }

    void CurlHttpRequest::SetResponseCallback(const std::function<void(CurlHttpClient* pClient, CurlHttpResponse* pResponse)>& callback)
    {
        m_OnResponseCallback = callback;
    }

    std::function<void(CurlHttpClient* pClient, CurlHttpResponse* pResponse)> CurlHttpRequest::GetResponseCallback() const
    {
        return m_OnResponseCallback;
    }
}

