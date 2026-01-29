#include "DHttpManager.h"

DHttpManager::DHttpManager()
{
//    this->setProxy(QNetworkProxy::NoProxy);
}

DHttpManager::~DHttpManager()
{

}

QByteArray DHttpManager::httpManagerGet(QString httpGet,int timeout)
{

    m_count++;
    m_id++;
    if(this->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        this->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    QNetworkRequest getRequest(httpGet);

    getRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    getRequest.setRawHeader(QByteArray("m_mmmid"), QString("%1").arg(m_id).toUtf8());
    QNetworkReply* getReply = this->get(getRequest);
    QEventLoop event;
    QByteArray reply;
    int idd = m_id;
    QTimer timer;
    timer.setProperty("ttt",idd);
    qDebug()<<" httpGet "<<httpGet<<"  --->" <<m_id<<" "<<&timer;
    connect(&timer,&QTimer::timeout,this,[&,idd](){
        qDebug().noquote()<<httpGet<<" timeout  "<<timeout<<" idd "<<idd<<"===="<<sender()->property("ttt").toInt()<<" getReply->readAll()  "<<QString(getReply->readAll());
//        event.quit();
        getReply->abort();
    });
    connect(getReply,&QNetworkReply::finished,[&](){
        m_count--;
        qDebug()<<" --------------- +++++++++++++++++ "<<m_count;
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
    return reply;
}

QByteArray DHttpManager::httpManagerPost(QString httpPost,QString postData,int timeout)
{

    m_count++;
    m_id++;
    if(this->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        this->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    QNetworkRequest postRequest(httpPost);
    postRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/json;charset=utf-8");
    postRequest.setRawHeader(QByteArray("m_mmmid"), QString("%1").arg(m_id).toUtf8());
    QNetworkReply* postReply = this->post(postRequest,postData.toUtf8());
    QEventLoop event;
    QByteArray reply;
    int idd = m_id;
    QTimer timer;
    timer.setProperty("ttt",idd);
    qDebug()<<" httpPost "<<httpPost<<"  --->" <<m_id<<" "<<&timer;
    connect(&timer,&QTimer::timeout,this,[&,idd](){
        qDebug().noquote()<<httpPost<<" timeout  "<<timeout<<" idd "<<idd<<"===="<<sender()->property("ttt").toInt()<<" postReply->readAll()  "<<QString(postReply->readAll());
//        event.quit();
        postReply->abort();
    });

    connect(postReply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),[=](QNetworkReply::NetworkError code){
        qDebug()<<"NetworkError code "<<code;
        qDebug()<<postReply->errorString();
        return;
    });

    connect(postReply,&QNetworkReply::finished,[&](){
        m_count--;
        qDebug()<<" --------------- +++++++++++++++++ "<<m_count;
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
    timer.start(timeout);
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
