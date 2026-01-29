#include "DHttpCurlManager.h"
#include "Logger.h"
DHttpCurlManager::DHttpCurlManager()
{
//    Dobot::CurlHttpClient::GetInstance()->SetProxy("127.0.0.1",8888);
}

QByteArray DHttpCurlManager::httpManagerGet(QString httpGet, int timeout)
{
    QEventLoop eventLoop;
    QString result;
    std::shared_ptr<Dobot::CurlHttpRequest> request(new Dobot::CurlHttpRequest);
    request->SetRequestType(Dobot::CurlHttpRequest::Type::GET);
    request->SetReadTimeout((timeout+1000)/1000);
    request->AddHeader("Content-Type","application/json");
    request->SetUrl(httpGet.toUtf8().toStdString().c_str());
    request->SetResponseCallback([&](Dobot::CurlHttpClient* pClient, Dobot::CurlHttpResponse* pResponse){
        if(pResponse->GetHttpRequest()->GetUrl() != nullptr
                && pResponse->GetErrorMessage() != nullptr)
        {
            if (nullptr==strstr(pResponse->GetHttpRequest()->GetUrl(),"www.baidu.com"))
            {
                qDebug()<<" pResponse "<<pResponse->IsSucceed()
                   << " http --> "<<pResponse->GetHttpRequest()->GetUrl()
                   << " errorMsg --> "<<pResponse->GetErrorMessage()
                   << " length--> "<<pResponse->GetResponseData()->size();
            }
        }
        if(pResponse->IsSucceed())
        {
            result = QString::fromUtf8(pResponse->GetResponseData()->data(),pResponse->GetResponseData()->size());
        }
        eventLoop.quit();
    });
    Dobot::CurlHttpClient::GetInstance()->Send(request);
    eventLoop.exec();
    return result.toUtf8();
}

QByteArray DHttpCurlManager::httpManagerPost(QString httpPost, QString postData, int timeout)
{
    QEventLoop eventLoop;
    QString result;
    std::shared_ptr<Dobot::CurlHttpRequest> request(new Dobot::CurlHttpRequest);
    request->SetRequestType(Dobot::CurlHttpRequest::Type::POST);
    std::string stdStrPostData = postData.toUtf8().toStdString();
    request->SetRequestData(stdStrPostData.c_str(),stdStrPostData.size());
    request->SetReadTimeout((timeout+1000)/1000);
    request->AddHeader("Content-Type","application/json;charset=utf-8");
    request->SetUrl(httpPost.toUtf8().toStdString().c_str());
    request->SetResponseCallback([&](Dobot::CurlHttpClient* pClient, Dobot::CurlHttpResponse* pResponse){
        if(pResponse->GetHttpRequest()->GetUrl() != nullptr
                && pResponse->GetErrorMessage() != nullptr)
        {
            qDebug()<<" pResponse "<<pResponse->IsSucceed()
                   << " http --> "<<pResponse->GetHttpRequest()->GetUrl()
                   << " errorMsg --> "<<pResponse->GetErrorMessage()
                   << " length--> "<<pResponse->GetResponseData()->size();
        }
        if(pResponse->IsSucceed())
        {
            result =QString::fromUtf8(pResponse->GetResponseData()->data(),pResponse->GetResponseData()->size());
        }
        else
        {
            QString str = QString::fromUtf8(pResponse->GetErrorMessage());
            qDebug()<<" pResponse errmsg:"<<str;
        }
        eventLoop.quit();
    });
    Dobot::CurlHttpClient::GetInstance()->Send(request);
    eventLoop.exec();
    return result.toUtf8();
}
