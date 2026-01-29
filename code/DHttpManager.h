#ifndef DHTTPMANAGER_H
#define DHTTPMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QMutex>
#include <QEventLoop>
#include <QTimer>
#include <QNetworkProxy>
class HttpResponse;
class DHttpManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    DHttpManager();
    ~DHttpManager();
    QByteArray httpManagerGet(QString httpGet,int timeout = 5000);
    QByteArray httpManagerPost(QString httpPost,QString postData,int timeout = 5000);
signals:
    void signal_httpGetReply(QString protocol,QString);
    void signal_httpPostReply(QString);

private:
    int m_count = 0;
    int m_id = 0;

};

#endif // DHTTPMANAGER_H
