#ifndef CSFTPCLIENT_H
#define CSFTPCLIENT_H

#include <QObject>
#include <ssh/sshconnection.h>
#include <ssh/sftpchannel.h>
#include <QList>
#include <QHash>
#include <QTimer>
#include <QMutex>

struct SFtpConnectParam
{
    QString strHost;
    quint16 iPort;
    QString strUserName;
    QString strPwd;
    int iTimeoutSeconds;
};
Q_DECLARE_METATYPE(SFtpConnectParam)

enum SFtpOption
{
    OPT_UPLOAD,
    OPT_DOWNLOAD,
    OPT_UPLOADDIR,
    OPT_REMOVEFILE,
    OPT_LISTDIR
};

struct SFtpOptFile
{
    QString strLocalPath;
    QString strRemotePath;
    SFtpOption opt;
    qint64 id;
};

class CSFtpClient : public QObject
{
    Q_OBJECT
public:
    explicit CSFtpClient(QObject *parent = nullptr);
    ~CSFtpClient();

    void connectHost(const SFtpConnectParam& prm);
    void disconnectHost();

    void ssh(SFtpOptFile opt);

signals:
    void signalConnectHost(SFtpConnectParam prm,QPrivateSignal);
    void signalDisConnectHost(QPrivateSignal);
    void signalOnChannelFileInfoAvailableFinish(qint64 job, const QList<QSsh::SftpFileInfo> &fileInfoList);
    void signalFinishedJob(qint64 id, const QList<QSsh::SftpFileInfo> &fileInfoList,bool bOk, QString strErrMsg);

    void signalConnected();
    void signalDisconnected();
    void signalConnectError(QString strErr);

    void signalBeginJob(qint64 id);
private slots:
    void slotConnectHost(SFtpConnectParam prm);
    void slotDisConnectHost();

    void onConnected();
    void onDisConnected();
    void onConnectionError(QSsh::SshError err);

    void onChannelInitialized();
    void onChannelError(QString strErrMsg);
    void onChannelFinished(QSsh::SftpJobId job, const QString &err);
    void onChannelClosed();
    void slotOnChannelFileInfoAvailable(QSsh::SftpJobId job, const QList<QSsh::SftpFileInfo> &fileInfoList);

    void slotSSH();



private:
    QSsh::SshConnection* m_pSshConnection;
    QSsh::SftpChannel::Ptr m_channel;
    QList<SFtpOptFile> m_ftpFileOperate;
    QTimer* m_pTimer;
    bool m_bIsBusy;
    QHash<QSsh::SftpJobId,qint64> m_cacheJobId;
    QMutex m_mtx;
    QList<QSsh::SftpFileInfo> m_fileInfoList;
};

#endif // CSFTPCLIENT_H
