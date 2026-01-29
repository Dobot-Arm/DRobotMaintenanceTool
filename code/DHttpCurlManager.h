#ifndef DHTTPCURLMANAGER_H
#define DHTTPCURLMANAGER_H

#include <QByteArray>
#include <QString>
#include <QEventLoop>
#include "CurlHttpClient/CurlHttpClient.h"
#include "CurlHttpClient/CurlHttpRequest.h"
class DHttpCurlManager
{
public:
    DHttpCurlManager();
    ~DHttpCurlManager();
    QByteArray httpManagerGet(QString httpGet,int timeout = 5000);
    QByteArray httpManagerPost(QString httpPost,QString postData,int timeout = 5000);
};

#endif // DHTTPCURLMANAGER_H
