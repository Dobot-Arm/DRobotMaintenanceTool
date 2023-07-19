#include "DHttpManager.h"

DHttpManager::DHttpManager()
{
    this->setProxy(QNetworkProxy::NoProxy);
}

DHttpManager::~DHttpManager()
{

}

QByteArray DHttpManager::httpManagerGet(QString httpGet,int timeout)
{

    if(this->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        this->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    QNetworkRequest getRequest(httpGet);

    getRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply* getReply = this->get(getRequest);
    QEventLoop event;
    QByteArray reply;
    QTimer timer;
    connect(&timer,&QTimer::timeout,this,[&](){
        qDebug().noquote()<<httpGet<<" timeout  ";
//        event.quit();
        getReply->abort();
    });
    connect(getReply,&QNetworkReply::finished,[&](){
        timer.stop();
        if(getReply->error() == QNetworkReply::NoError){
             reply = getReply->readAll();
            // qDebug().noquote()<<"QNetworkReply::NoError httpGet--> "<<httpGet <<" reply--->  "<<reply;
        }else{
             qDebug().quote()<<httpGet << " QNetworkReply  Error"<<getReply->errorString();

             this->clearAccessCache();
        }

        getReply->deleteLater();
        event.quit();
    });
    timer.setSingleShot(true);
    timer.start(timeout);
    event.exec();
//    if(!timer.isActive())
//    {
//        qDebug()<<httpGet << " timeout ";
//        getReply->abort();
//        return reply;
//    }else{
//        timer.stop();
//    }
    return reply;
}

QByteArray DHttpManager::httpManagerPost(QString httpPost,QString postData)
{


    if(this->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        this->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    QNetworkRequest postRequest(httpPost);
    postRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/json;charset=utf-8");
    QNetworkReply* postReply = this->post(postRequest,postData.toUtf8());
    QEventLoop event;
    QByteArray reply;
    QTimer timer;
    connect(&timer,&QTimer::timeout,this,[&](){
        qDebug().noquote()<<httpPost<<" timeout  ";
//        event.quit();
        postReply->abort();
    });

    connect(postReply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),[=](QNetworkReply::NetworkError code){
        qDebug()<<"NetworkError code "<<code;
        qDebug()<<postReply->errorString();
        return;
    });

    connect(postReply,&QNetworkReply::finished,[&](){
        timer.stop();
        if(postReply->error() == QNetworkReply::NoError){
            reply = postReply->readAll();
        }else{
            qDebug().noquote()<<httpPost << "httpPost data-> "<<postData << " QNetworkReply  Error";
            this->clearAccessCache();
        }

        postReply->deleteLater();
        event.quit();
    });
    timer.setSingleShot(true);
    timer.start(1500);
    event.exec();
//    if(!timer.isActive())
//    {
//        qDebug()<<httpPost << " data-> "<<postData << " timeout ";
//        postReply->abort();
//        return reply;
//    }else{
//        timer.stop();
//    }

    return reply;
}
