#include "HttpUserAPI.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include <QEventLoop>
#include <QTimer>
#include <QJsonDocument>

#include "Logger.h"

struct CHttpUserAPIPrivate
{
    QNetworkAccessManager* pManager;
    CHttpUserAPIPrivate()
    {
        pManager = new QNetworkAccessManager();
        QNetworkProxy proxy = pManager->proxy();
        proxy.setType(QNetworkProxy::ProxyType::NoProxy);
        pManager->setProxy(proxy);
    }
};

CHttpUserAPI::CHttpUserAPI(QObject *parent) : QObject(parent)
{
    m_ptr = new CHttpUserAPIPrivate;
}

CHttpUserAPI::~CHttpUserAPI()
{
    delete m_ptr;
}

CHttpUserAPI &CHttpUserAPI::getInstance()
{
    static CHttpUserAPI obj;
    return obj;
}

QString CHttpUserAPI::httpGet(const QString& strUrl, int iTimeoutMilliseconds)
{
    qDebug()<<"httpGet url:"<<strUrl;
    if (m_ptr->pManager->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        m_ptr->pManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    QNetworkRequest request(strUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* pReply = m_ptr->pManager->get(request);
    QTimer* pTimer = new QTimer(pReply);
    pTimer->setSingleShot(true);
    pTimer->setInterval(iTimeoutMilliseconds);
    pTimer->start();

    QEventLoop loop;
    connect(pTimer, &QTimer::timeout, this,[pReply,strUrl]{
        qDebug()<<"httpGet timeout,url:"<<strUrl;
        pReply->abort();
    });
    connect(pReply, &QNetworkReply::finished, this, [pReply,strUrl,&loop]{
        if (pReply->error() == QNetworkReply::NoError)
        {
            QString strResult = pReply->readAll();
            loop.setProperty("result", strResult);
            qDebug()<<QString("httpGet Ok(%1),read data:%2").arg(strUrl).arg(strResult);
        }
        else
        {
            qDebug()<<QString("httpGet error(%1),url:%2").arg(pReply->errorString()).arg(strUrl);
        }
        pReply->deleteLater();
        loop.quit();
    });
    loop.exec();
    return loop.property("result").toString();
}

QString CHttpUserAPI::httpPost(const QString& strUrl, const QString& strPostData, int iTimeoutMilliseconds)
{
    qDebug()<<"httpPost url:"<<strUrl<<",post data:"<<strPostData;
    if (m_ptr->pManager->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        m_ptr->pManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    QNetworkRequest request(strUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* pReply = m_ptr->pManager->post(request,strPostData.toUtf8());
    QTimer* pTimer = new QTimer(pReply);
    pTimer->setSingleShot(true);
    pTimer->setInterval(iTimeoutMilliseconds);
    pTimer->start();

    QEventLoop loop;
    connect(pTimer, &QTimer::timeout, this,[pReply,strUrl]{
        qDebug()<<"httpPost timeout,url:"<<strUrl;
        pReply->abort();
    });
    connect(pReply, &QNetworkReply::finished, this, [pReply,strUrl,&loop]{
        if (pReply->error() == QNetworkReply::NoError)
        {
            QString strResult = pReply->readAll();
            loop.setProperty("result", strResult);
            qDebug()<<QString("httpPost Ok(%1),read data:%2").arg(strUrl).arg(strResult);
        }
        else
        {
            qDebug()<<QString("httpPost error(%1),url:%2").arg(pReply->errorString()).arg(strUrl);
        }
        pReply->deleteLater();
        loop.quit();
    });
    loop.exec();
    return loop.property("result").toString();
}

bool CHttpUserAPI::checkSpecialDiskCanUpdate(const QString &strIP, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22002/update/diskCheck").arg(strIP);
    QString strPostData="{\"operationType\":\"checkP5\"}";
    QString strRetData = httpPost(strUrl, strPostData, iTimeoutMilliseconds);
    QString str = QJsonDocument::fromJson(strRetData.toUtf8()).object().value("result").toString();
    return ("success" == str);
}

bool CHttpUserAPI::updateCRFirmwareAndXml(const QString &strIP, const CRFirmwareAndXmlData &data, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22002/update/CR/fwAndXml").arg(strIP);
    QString strPostData=data.toJsonString();
    QString strRetData = httpPost(strUrl, strPostData, iTimeoutMilliseconds);
    QString str = QJsonDocument::fromJson(strRetData.toUtf8()).object().value("result").toString();
    return ("success" == str);
}

bool CHttpUserAPI::getUpdateCRFirmwareAndXmlStatus(const QString &strIP, CRFirmwareAndXmlResult &result, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22002/update/CR/status").arg(strIP);
    QString strRetData = httpGet(strUrl, iTimeoutMilliseconds);
    if (strRetData.isEmpty())
    {
        return false;
    }
    result.fromJson(strRetData);
    return true;
}

bool CHttpUserAPI::updateCRSingleFirmwareAndXml(const QString &strIP, const CRSingleFirmwareXmlData &data, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22002/update/CR/singleFwAndXml").arg(strIP);
    QString strPostData=data.toJsonString();
    QString strRetData = httpPost(strUrl, strPostData, iTimeoutMilliseconds);
    QString str = QJsonDocument::fromJson(strRetData.toUtf8()).object().value("result").toString();
    return ("success" == str);
}

UpdateState CHttpUserAPI::getUpdateSingleFirmwareAndXmlStatus(const QString &strIP, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22002/update/singleStatus").arg(strIP);
    QString strRetData = httpGet(strUrl, iTimeoutMilliseconds);
    QJsonValue jsv = QJsonDocument::fromJson(strRetData.toUtf8()).object().value("result");
    return static_cast<UpdateState>(jsv.toInt(static_cast<int>(UpdateState::E_FAILED)));
}

bool CHttpUserAPI::faultRepair(const QString &strIP, const FaultRepairData &data, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22002/fault/repair").arg(strIP);
    QString strPostData=data.toJsonString();
    QString strRetData = httpPost(strUrl, strPostData, iTimeoutMilliseconds);
    QString str = QJsonDocument::fromJson(strRetData.toUtf8()).object().value("result").toString();
    return ("success" == str);
}

QString CHttpUserAPI::getToolVersion(const QString &strIP, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22002/protocol/version").arg(strIP);
    QString strRetData = httpGet(strUrl, iTimeoutMilliseconds);
    QJsonValue jsv = QJsonDocument::fromJson(strRetData.toUtf8()).object().value("toolVersion");
    return jsv.toString();
}

bool CHttpUserAPI::modifyServoParameters(const QString &strIP, const ServoPrmModifyData &data, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22000/settings/modifyServoParams").arg(strIP);
    QString strPostData=data.toJsonString();
    QString strRetData = httpPost(strUrl, strPostData, iTimeoutMilliseconds);
    return QJsonDocument::fromJson(strRetData.toUtf8()).object().value("status").toBool();
}

bool CHttpUserAPI::getModifyServoParametersStatus(const QString &strIP, ServoPrmModifyResult &result, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22000/settings/modifyServoParams").arg(strIP);
    QString strRetData = httpGet(strUrl, iTimeoutMilliseconds);
    if (strRetData.isEmpty())
    {
        return false;
    }
    result.fromJson(strRetData);
    return true;
}

bool CHttpUserAPI::readServoParameters(const QString &strIP, const ServoPrmReadData &data, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22000/settings/readServoParams").arg(strIP);
    QString strPostData=data.toJsonString();
    QString strRetData = httpPost(strUrl, strPostData, iTimeoutMilliseconds);
    return QJsonDocument::fromJson(strRetData.toUtf8()).object().value("status").toBool();
}

bool CHttpUserAPI::getReadServoParametersStatus(const QString &strIP, ServoPrmReadResult &result, int iTimeoutMilliseconds)
{
    QString strUrl = QString("http://%1:22000/settings/readServoParams").arg(strIP);
    QString strRetData = httpGet(strUrl, iTimeoutMilliseconds);
    if (strRetData.isEmpty())
    {
        return false;
    }
    result.fromJson(strRetData);
    return true;
}
