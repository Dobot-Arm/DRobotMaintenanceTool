#include "SFtpClient.h"
#include <QFileInfo>

CSFtpClient::CSFtpClient(QObject *parent) : QObject(parent)
{
    m_pSshConnection = nullptr;
    m_bIsBusy = false;
    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(10);
    connect(m_pTimer, &QTimer::timeout, this, &CSFtpClient::slotSSH);

    qRegisterMetaType<SFtpConnectParam>();
    connect(this, &CSFtpClient::signalConnectHost, this, &CSFtpClient::slotConnectHost);
    connect(this, &CSFtpClient::signalDisConnectHost, this, &CSFtpClient::slotDisConnectHost);
}

CSFtpClient::~CSFtpClient()
{
    m_pTimer->deleteLater();
    disconnectHost();
}

void CSFtpClient::connectHost(const SFtpConnectParam &prm)
{
    emit signalConnectHost(prm, QPrivateSignal());
}

void CSFtpClient::slotConnectHost(SFtpConnectParam prm)
{
    disconnectHost();

    QSsh::SshConnectionParameters params;
    params.host = prm.strHost;
    params.port = prm.iPort;
    params.userName = prm.strUserName;
    params.password = prm.strPwd;
    params.authenticationType = QSsh::SshConnectionParameters::AuthenticationTypePassword;
    params.timeout = prm.iTimeoutSeconds;

    auto pSshConnection = new QSsh::SshConnection(params, this);
    m_pSshConnection = pSshConnection;
    connect(pSshConnection, &QSsh::SshConnection::connected, this, &CSFtpClient::onConnected);
    connect(pSshConnection, &QSsh::SshConnection::disconnected, this, &CSFtpClient::onDisConnected);
    connect(pSshConnection, &QSsh::SshConnection::error, this, &CSFtpClient::onConnectionError);
    pSshConnection->connectToHost();

    m_pTimer->start();
}

void CSFtpClient::disconnectHost()
{
    emit signalDisConnectHost(QPrivateSignal());
}

void CSFtpClient::slotDisConnectHost()
{
    if (m_pSshConnection)
    {
        m_pSshConnection->disconnectFromHost();
        m_pSshConnection->deleteLater();
        m_pSshConnection = nullptr;
    }
    m_pTimer->stop();
}

void CSFtpClient::onConnected()
{
    m_channel = m_pSshConnection->createSftpChannel();

    connect(m_channel.data(), &QSsh::SftpChannel::initialized, this, &CSFtpClient::onChannelInitialized);
    connect(m_channel.data(), &QSsh::SftpChannel::channelError, this, &CSFtpClient::onChannelError);
    connect(m_channel.data(), &QSsh::SftpChannel::finished, this, &CSFtpClient::onChannelFinished);
    connect(m_channel.data(), &QSsh::SftpChannel::closed, this, &CSFtpClient::onChannelClosed);
    connect(m_channel.data(), &QSsh::SftpChannel::fileInfoAvailable,this,&CSFtpClient::slotOnChannelFileInfoAvailable);
    m_channel->initialize();

    emit signalConnected();
}

void CSFtpClient::onDisConnected()
{
    {
        QMutexLocker guard(&m_mtx);
        m_ftpFileOperate.clear();
    }
    m_bIsBusy = false;
    m_cacheJobId.clear();
    if (!m_channel.isNull())
    {
        m_channel->closeChannel();
    }
    emit signalDisconnected();
}

void CSFtpClient::onConnectionError(QSsh::SshError )
{
    emit signalConnectError(m_pSshConnection->errorString());
}

void CSFtpClient::onChannelInitialized()
{
    QMutexLocker guard(&m_mtx);
    if (m_ftpFileOperate.isEmpty() || m_bIsBusy) return ;
    m_bIsBusy = true;
    SFtpOptFile opt = m_ftpFileOperate.takeFirst();
    guard.unlock();

    m_fileInfoList.clear();
    QSsh::SftpJobId job;
    if (SFtpOption::OPT_DOWNLOAD == opt.opt)
    {
        job = m_channel->downloadFile(opt.strRemotePath,opt.strLocalPath, QSsh::SftpOverwriteExisting);
    }
    else if (SFtpOption::OPT_UPLOAD == opt.opt)
    {
        job = m_channel->uploadFile(opt.strLocalPath, opt.strRemotePath, QSsh::SftpOverwriteExisting);
    }
    else if (SFtpOption::OPT_UPLOADDIR == opt.opt)
    {
        job = m_channel->uploadDir(opt.strLocalPath, opt.strRemotePath);
    }
    else if (SFtpOption::OPT_REMOVEFILE == opt.opt)
    {
        job = m_channel->removeFile(opt.strRemotePath);
    }else if (SFtpOption::OPT_LISTDIR == opt.opt)
    {
        job = m_channel->listDirectory(opt.strRemotePath);
    }
    else
    {
        m_bIsBusy = false;
        return ;
    }
    if (job != QSsh::SftpInvalidJob)
    {
        m_cacheJobId.insert(job,opt.id);
        //LOG_DEBUG()<<"Starting job ok,jobId="<<job<<",file id="<<opt.id;
        emit signalBeginJob(opt.id);
    }
    else
    {
        m_bIsBusy = false;
        //LOG_DEBUG()<<"Starting job fail,jobId="<<job<<",file id="<<opt.id;
        emit signalFinishedJob(opt.id, m_fileInfoList, false, QString("starting job error"));
    }
}

void CSFtpClient::onChannelError(QString strErrMsg)
{
    qDebug()<<"onDowloadChannelError:"<<strErrMsg;
    m_bIsBusy = false;
}

void CSFtpClient::onChannelFinished(QSsh::SftpJobId job, const QString &err)
{
    //LOG_DEBUG()<<"onDownloadFinished jobid:"<<job<<":"<<(err.isEmpty()?"OK":err);

    //如果有多个文件需要下载，则在这里可以继续下载下一个，然后不要关闭连接
    /*m_channel->closeChannel();
    m_pSshConnection->disconnectFromHost();*/

    qint64 id = m_cacheJobId.value(job,0);
    m_cacheJobId.remove(job);
    emit signalFinishedJob(id, m_fileInfoList, err.isEmpty(), err);

    m_bIsBusy = false;
}

void CSFtpClient::onChannelClosed()
{
    m_bIsBusy = false;
    m_channel->closeChannel();
    m_pSshConnection->disconnectFromHost();
}

void CSFtpClient::slotOnChannelFileInfoAvailable(QSsh::SftpJobId job, const QList<QSsh::SftpFileInfo> &fileInfoList)
{
    qint64 id = m_cacheJobId.value(job,0);
    m_fileInfoList.append(fileInfoList);
    emit signalOnChannelFileInfoAvailableFinish(id, fileInfoList);
}

void CSFtpClient::slotSSH()
{
    if (!m_channel.isNull() && m_channel->state()==QSsh::SftpChannel::State::Initialized)
    {
        m_pTimer->stop();
        onChannelInitialized();
        m_pTimer->start();
    }
}

void CSFtpClient::ssh(SFtpOptFile opt)
{
    QMutexLocker guard(&m_mtx);
    m_ftpFileOperate.append(opt);
}
