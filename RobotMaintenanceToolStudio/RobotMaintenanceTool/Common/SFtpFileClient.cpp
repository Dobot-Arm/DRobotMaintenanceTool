#include "SFtpFileClient.h"
#include "Logger.h"

#include <QList>
#include <QHash>
#include <QTimer>
#include <QMutex>
#include <QThread>
#include <QQueue>
#include <QStack>
#include <QFileInfo>
#include <QEventLoop>

struct CSFtpFileClientPrivate
{
    QThread* pThread;
    QSsh::SshConnection* pSshConnection;
    QSsh::SftpChannel::Ptr channel;
    QList<SFtpOptFile> ftpFileOperate;
    QTimer* pTimer;
    bool bIsBusy;
    QMutex mtx;
    bool bIsConnected;
};

static QString removeLastCharIf(QString str, QChar chRemove)
{
    while (str.size()>0)
    {
        if (str.endsWith(chRemove))
        {
            str.remove(str.length()-1,1);
        }
        else
        {
            break;
        }
    }
    return str;
}
/*
获取路径的后部分，例如：
E:/A/B/C，则返回C
E:/A/B/a.txt,则返回a.txt
*/
static QString getPathTail(QString strPath)
{
    strPath = removeLastCharIf(strPath, '/');
    if (strPath.isEmpty()) return strPath;
    int iPos = strPath.lastIndexOf('/');
    if (iPos >= 0) return strPath.mid(iPos+1);
    return strPath;
}

//按照一定规则获取所有的子目录和文件
static int getAllChildrenFileAndDir(const QString& strDir,QQueue<QStringList>& allPath)
{
    int iCount = 0;
    QDir dir(strDir);
    QStringList all = dir.entryList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot,QDir::DirsFirst);
    for(int i=0; i<all.size(); ++i) {
        all[i] = dir.absoluteFilePath(all[i]);
        all[i].replace('\\','/');
        all[i] = removeLastCharIf(all[i], '/');
    }
    allPath.enqueue(all);

    iCount += all.size();

    while(!all.isEmpty())
    {
        QString str = all.takeFirst();
        QFileInfo info(str);
        if (info.isDir()) iCount += getAllChildrenFileAndDir(str,allPath);
    }
    return iCount;
}

CSFtpFileClient::CSFtpFileClient() : QObject(nullptr)
{
    m_ptr = new CSFtpFileClientPrivate;

    m_ptr->pSshConnection = nullptr;
    m_ptr->bIsBusy = false;
    m_ptr->bIsConnected = false;

    qRegisterMetaType<SFtpConnectParam>();
    connect(this, &CSFtpFileClient::signalConnectHost, this, &CSFtpFileClient::slotConnectHost);
    connect(this, &CSFtpFileClient::signalDisConnectHost, this, &CSFtpFileClient::slotDisConnectHost);

    m_ptr->pTimer = new QTimer();
    m_ptr->pTimer->setInterval(10);
    connect(m_ptr->pTimer, &QTimer::timeout, this, &CSFtpFileClient::slotSSH);

    m_ptr->pThread = new QThread();
    this->moveToThread(m_ptr->pThread);
    m_ptr->pTimer->moveToThread(m_ptr->pThread);
    m_ptr->pThread->start();
}

CSFtpFileClient::~CSFtpFileClient()
{
    m_ptr->pTimer->deleteLater();
    disconnectHost();
    m_ptr->pThread->quit();
    m_ptr->pThread->terminate();
    m_ptr->pThread->deleteLater();
    delete m_ptr;
}

void CSFtpFileClient::connectHost(const SFtpConnectParam &prm)
{
    emit signalConnectHost(prm, QPrivateSignal());
}

void CSFtpFileClient::slotConnectHost(SFtpConnectParam prm)
{
    disconnectHost();

    QSsh::SshConnectionParameters params;
    params.host = prm.strHost;
    params.port = prm.iPort;
    params.userName = prm.strUserName;
    params.password = prm.strPwd;
    params.authenticationType = QSsh::SshConnectionParameters::AuthenticationTypePassword;
    params.timeout = prm.iTimeoutSeconds;
    params.hostKeyCheckingMode = QSsh::SshHostKeyCheckingMode::SshHostKeyCheckingNone;
    params.options = QSsh::SshConnectionOption::SshIgnoreDefaultProxy;

    auto pSshConnection = new QSsh::SshConnection(params, this);
    m_ptr->pSshConnection = pSshConnection;
    connect(pSshConnection, &QSsh::SshConnection::connected, this, &CSFtpFileClient::onConnected);
    connect(pSshConnection, &QSsh::SshConnection::disconnected, this, &CSFtpFileClient::onDisConnected);
    connect(pSshConnection, &QSsh::SshConnection::error, this, &CSFtpFileClient::onConnectionError);
    pSshConnection->connectToHost();

    m_ptr->pTimer->start();
}

void CSFtpFileClient::onConnected()
{
    m_ptr->channel = m_ptr->pSshConnection->createSftpChannel();

    connect(m_ptr->channel.data(), &QSsh::SftpChannel::initialized, this, &CSFtpFileClient::onChannelInitialized);
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::channelError, this, &CSFtpFileClient::onChannelError);
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::closed, this, &CSFtpFileClient::onChannelClosed);
    m_ptr->channel->initialize();

    m_ptr->bIsConnected = true;

    emit signalConnected();
}

void CSFtpFileClient::disconnectHost()
{
    emit signalDisConnectHost(QPrivateSignal());
}

bool CSFtpFileClient::isConnected() const
{
    return m_ptr->bIsConnected;
}

void CSFtpFileClient::slotDisConnectHost()
{
    if (m_ptr->pSshConnection)
    {
        m_ptr->pSshConnection->disconnectFromHost();
        m_ptr->pSshConnection->deleteLater();
        m_ptr->pSshConnection = nullptr;
    }
    m_ptr->pTimer->stop();
}

void CSFtpFileClient::onDisConnected()
{
    {
        QMutexLocker guard(&m_ptr->mtx);
        m_ptr->ftpFileOperate.clear();
    }
    m_ptr->bIsConnected = false;
    m_ptr->bIsBusy = false;
    if (!m_ptr->channel.isNull())
    {
        m_ptr->channel->closeChannel();
    }
    emit signalDisconnected();
}

//将路径做裁剪，所有路径最后一位不能为/
void CSFtpFileClient::ssh(SFtpOptFile opt)
{
    opt.strLocalPath.replace('\\','/');
    opt.strLocalPath = removeLastCharIf(opt.strLocalPath, '/');

    opt.strRemotePath.replace('\\','/');
    opt.strRemotePath = removeLastCharIf(opt.strRemotePath, '/');

    QMutexLocker guard(&m_ptr->mtx);
    m_ptr->ftpFileOperate.append(opt);
}

void CSFtpFileClient::insertSSH(SFtpOptFile opt)
{
    opt.strLocalPath.replace('\\','/');
    opt.strRemotePath.replace('\\','/');

    QMutexLocker guard(&m_ptr->mtx);
    m_ptr->ftpFileOperate.prepend(opt);
}

void CSFtpFileClient::onConnectionError(QSsh::SshError err)
{
    Q_UNUSED(err)
    emit signalConnectError(m_ptr->pSshConnection->errorString());
}

void CSFtpFileClient::slotSSH()
{
    if (!m_ptr->channel.isNull() && m_ptr->channel->state()==QSsh::SftpChannel::State::Initialized)
    {
        onChannelInitialized();
    }
}

void CSFtpFileClient::onChannelInitialized()
{
    QMutexLocker guard(&m_ptr->mtx);
    if (m_ptr->ftpFileOperate.isEmpty() || m_ptr->bIsBusy) return ;
    m_ptr->bIsBusy = true;
    SFtpOptFile opt = m_ptr->ftpFileOperate.takeFirst();
    guard.unlock();

    if (SFtpOption::OPT_UPLOAD_FILE == opt.opt)
    {
        uploadFile(opt);
    }
    else if (SFtpOption::OPT_UPLOAD_DIR == opt.opt)
    {
        uploadDir(opt);
    }
    else if (SFtpOption::OPT_DOWNLOAD_FILE == opt.opt)
    {
        downloadFile(opt);
    }
    else if (SFtpOption::OPT_DOWNLOAD_DIR == opt.opt)
    {
        downloadDir(opt);
    }
    else if (SFtpOption::OPT_REMOVE_REMOTE_FILE == opt.opt)
    {
        removeRemoteFile(opt);
    }
    else if (SFtpOption::OPT_REMOVE_REMOTE_DIR == opt.opt)
    {
        removeRemoteDirRecursive(opt);
    }
    else if (SFtpOption::OPT_CREATE_REMOTE_DIR == opt.opt)
    {
        createRemoteDir(opt);
    }
    else if (SFtpOption::OPT_LIST_REMOTE_DIR == opt.opt)
    {
        listRemoteDir(opt);
    }

    m_ptr->bIsBusy = false;

    return ;
}

void CSFtpFileClient::onChannelError(QString strErrMsg)
{
    qDebug()<<"sftp file client onChannelError:"<<strErrMsg;
    m_ptr->bIsBusy = false;
}

void CSFtpFileClient::onChannelClosed()
{
    m_ptr->bIsBusy = false;
    m_ptr->channel->closeChannel();
    m_ptr->pSshConnection->disconnectFromHost();
}

bool CSFtpFileClient::uploadFile(const SFtpOptFile &opt)
{
    QEventLoop* pLoop = new QEventLoop(this);
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::finished, pLoop, [=](QSsh::SftpJobId, const QString &error){
        emit signalFinishedUploadFile(opt.strLocalPath, opt.strRemotePath, error);
        pLoop->setProperty("ok",QVariant(error.isEmpty()));
        pLoop->quit();
    });
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::fileUploadProgress, pLoop, [=](QSsh::SftpJobId, quint64 fileSize, quint64 offset){
        emit signalUploadFileProgress(opt.strLocalPath, opt.strRemotePath,fileSize,offset);
    });

    QSsh::SftpJobId job = m_ptr->channel->uploadFile(opt.strLocalPath, opt.strRemotePath, QSsh::SftpOverwriteExisting);
    if (job != QSsh::SftpInvalidJob)
    {
        emit signalBeginUploadFile(opt.strLocalPath, opt.strRemotePath);
        pLoop->exec();
    }
    else
    {
        emit signalFinishedUploadFile(opt.strLocalPath, opt.strRemotePath, QString("starting upload file error"));
    }
    bool bOk = pLoop->property("ok").toBool();
    pLoop->deleteLater();
    return bOk;
}

bool CSFtpFileClient::uploadDir(const SFtpOptFile &opt)
{
    //上传目录被分解成创建目录和上传文件动作,因此现要获取该目录下所有文件和目录
    int iTotalCount = 0;
    QQueue<QStringList> allPath;
    allPath.append(QStringList()<<opt.strLocalPath);
    iTotalCount += 1;
    iTotalCount += getAllChildrenFileAndDir(opt.strLocalPath, allPath);

    int iCurrentCount = 0;

    emit signalBeginUploadDir(opt.strLocalPath, opt.strRemotePath);

    const QString strRemoteDir = QString(opt.strRemotePath).append('/').append(getPathTail(opt.strLocalPath));
    while (!allPath.isEmpty())
    {
        QStringList subPath = allPath.dequeue();
        for (int i=0; i<subPath.size(); ++i)
        {
            QString strLocalPath = subPath[i];
            QString strRemotePath(strRemoteDir);
            strRemotePath.append(subPath[i].mid(opt.strLocalPath.length()));

            SFtpOptFile optTmp;
            optTmp.strLocalPath = strLocalPath;
            optTmp.strRemotePath = strRemotePath;
            QFileInfo info(subPath[i]);
            if (info.isFile())
            {
                optTmp.opt = SFtpOption::OPT_UPLOAD_FILE;
                if (uploadFile(optTmp))
                {
                    ++iCurrentCount;
                    emit signalUploadDirProgress(opt.strLocalPath, opt.strRemotePath,
                                                 strLocalPath,iTotalCount,iCurrentCount);
                }
                else
                {
                    emit signalFinishedUploadDir(opt.strLocalPath, opt.strRemotePath,
                                                 QString("upload dir fail,because of upload file(%1) failed").arg(strLocalPath));
                    return false;
                }
            }
            else
            {
                optTmp.opt = SFtpOption::OPT_CREATE_REMOTE_DIR;
                if (createRemoteDir(optTmp))
                {
                    ++iCurrentCount;
                    emit signalUploadDirProgress(opt.strLocalPath, opt.strRemotePath,
                                                 strLocalPath,iTotalCount,iCurrentCount);
                }
                else
                {
                    emit signalFinishedUploadDir(opt.strLocalPath, opt.strRemotePath,
                                                 QString("upload dir fail,because of create dir(%1) failed").arg(strLocalPath));
                    return false;
                }
            }
        }
    }
    emit signalFinishedUploadDir(opt.strLocalPath, opt.strRemotePath,"");
    return true;
}

bool CSFtpFileClient::downloadFile(const SFtpOptFile &opt)
{
    QFileInfo fileInfo(opt.strLocalPath);
    QDir dir(fileInfo.absolutePath());
    if (!dir.exists())
    {
        if (!dir.mkpath(dir.absolutePath()))
        {
            emit signalFinishedDownloadFile(opt.strLocalPath, opt.strRemotePath, QString("starting download file error,because of cannot create locale dir"));
            return false;
        }
    }

    QEventLoop* pLoop = new QEventLoop(this);
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::finished, pLoop, [=](QSsh::SftpJobId, const QString &error){
        emit signalFinishedDownloadFile(opt.strLocalPath, opt.strRemotePath, error);
        pLoop->setProperty("ok",QVariant(error.isEmpty()));
        pLoop->quit();
    });
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::fileDownloadProgress, pLoop, [=](QSsh::SftpJobId, quint64 fileSize, quint64 offset){
        emit signalDownloadFileProgress(opt.strLocalPath, opt.strRemotePath,fileSize,offset);
    });

    QSsh::SftpJobId job = m_ptr->channel->downloadFile(opt.strRemotePath,opt.strLocalPath, QSsh::SftpOverwriteExisting);
    if (job != QSsh::SftpInvalidJob)
    {
        emit signalBeginDownloadFile(opt.strLocalPath, opt.strRemotePath);
        pLoop->exec();
    }
    else
    {
        emit signalFinishedDownloadFile(opt.strLocalPath, opt.strRemotePath, QString("starting download file error"));
    }
    bool bOk = pLoop->property("ok").toBool();
    pLoop->deleteLater();
    return bOk;
}

bool CSFtpFileClient::downloadDir(const SFtpOptFile &opt)
{
    QSharedPointer<QStringList> remoteAllFiles(new QStringList);
    QSharedPointer<QStringList> remoteAllDirs(new QStringList);
    remoteAllDirs->append(opt.strRemotePath);

    emit signalBeginDownloadDir(opt.strLocalPath, opt.strRemotePath);

    //先遍历远程上所有的目录和子目录
    while (remoteAllDirs->size()>0)
    {
        QString strRemotePath = remoteAllDirs->takeFirst();
        SFtpOptFile optTmp;
        optTmp.strLocalPath = "";
        optTmp.strRemotePath = strRemotePath;
        optTmp.opt = SFtpOption::OPT_LIST_REMOTE_DIR;

        QString strErrMsg;
        QList<QSsh::SftpFileInfo> allSftpFile;
        if (!listRemoteDir(optTmp,allSftpFile,strErrMsg))
        {
            emit signalFinishedDownloadDir(opt.strLocalPath, opt.strRemotePath,
                                           QString("starting download file error,because of list dir(%1) fail,%2")
                                           .arg(strRemotePath).arg(strErrMsg));
            return false;
        }
        for (auto info : allSftpFile)
        {
            QString str(strRemotePath);
            str.append('/').append(info.name);
            if (QSsh::SftpFileType::FileTypeRegular == info.type)
            {
                remoteAllFiles->append(str);
            }
            else if (QSsh::SftpFileType::FileTypeDirectory == info.type)
            {
                if (info.name != "." && info.name != "..")
                {
                    remoteAllDirs->append(str);
                }
            }
        }
    }

    //开始下载文件
    const QString strLocalDir = QString(opt.strLocalPath).append('/').append(getPathTail(opt.strRemotePath));
    if (remoteAllFiles->isEmpty())
    {//说明本身就是一个空目录，那么在本地也新建一个目录吧
        QDir dir(strLocalDir);
        if (!dir.exists())
        {
            if (!dir.mkpath(dir.absolutePath()))
            {
                emit signalFinishedDownloadDir(opt.strLocalPath,opt.strRemotePath, QString("download dir fail, because of create local path(%1) fail").arg(dir.absolutePath()));
                return false;
            }
        }
        emit signalFinishedDownloadDir(opt.strLocalPath, opt.strRemotePath,"");
        return true;
    }
    int iCurrentCount = 0;
    int iTotalCount = remoteAllFiles->size();
    for (int i=0; i<remoteAllFiles->size(); ++i)
    {
        QString strRemotePath = remoteAllFiles->at(i);
        QString strLocalPath = strLocalDir;
        strLocalPath.append(strRemotePath.mid(opt.strRemotePath.length()));

        SFtpOptFile optTmp;
        optTmp.strLocalPath = strLocalPath;
        optTmp.strRemotePath = strRemotePath;
        optTmp.opt = SFtpOption::OPT_DOWNLOAD_FILE;
        if (downloadFile(optTmp))
        {
            ++iCurrentCount;
            emit signalDownloadDirProgress(opt.strLocalPath, opt.strRemotePath,
                                         strRemotePath,iTotalCount,iCurrentCount);
        }
        else
        {
            emit signalFinishedDownloadDir(opt.strLocalPath, opt.strRemotePath,
                                         QString("download dir fail,because of download file(%1) failed").arg(strRemotePath));
            return false;
        }
    }
    emit signalFinishedDownloadDir(opt.strLocalPath, opt.strRemotePath,"");
    return true;
}

bool CSFtpFileClient::removeRemoteFile(const SFtpOptFile &opt)
{
    QEventLoop* pLoop = new QEventLoop(this);
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::finished, pLoop, [=](QSsh::SftpJobId, const QString &error){
        QString strErr = error;
        if (strErr.compare("No such file", Qt::CaseSensitivity::CaseInsensitive))
        {
            strErr = "";
        }
        emit signalFinishedRemoveRemoteFile(opt.strRemotePath, strErr);
        pLoop->setProperty("ok",QVariant(strErr.isEmpty()));
        pLoop->quit();
    });

    QSsh::SftpJobId job = m_ptr->channel->removeFile(opt.strRemotePath);
    if (job != QSsh::SftpInvalidJob)
    {
        emit signalBeginRemoveRemoteFile(opt.strRemotePath);
        pLoop->exec();
    }
    else
    {
        emit signalFinishedRemoveRemoteFile(opt.strRemotePath, QString("starting remove remote file error"));
    }
    bool bOk = pLoop->property("ok").toBool();
    pLoop->deleteLater();
    return bOk;
}

bool CSFtpFileClient::removeRemoteDir(const SFtpOptFile &opt)
{
    QEventLoop* pLoop = new QEventLoop(this);
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::finished, pLoop, [=](QSsh::SftpJobId, const QString &error){
        QString strErr = error;
        if (strErr.compare("No such file", Qt::CaseSensitivity::CaseInsensitive))
        {
            strErr = "";
        }
        pLoop->setProperty("ok",QVariant(strErr.isEmpty()));
        pLoop->quit();
    });

    QSsh::SftpJobId job = m_ptr->channel->removeDirectory(opt.strRemotePath);
    if (job != QSsh::SftpInvalidJob)
    {
        pLoop->exec();
    }
    bool bOk = pLoop->property("ok").toBool();
    pLoop->deleteLater();
    return bOk;
}

bool CSFtpFileClient::removeRemoteDirRecursive(const SFtpOptFile &opt)
{
    emit signalBeginRemoveRemoteDir(opt.strRemotePath);

    //远程文件或目录,bool为true表示dir，false表示文件
    QStack<QHash<QString,bool>> allChildrenFileAndDirs;
    allChildrenFileAndDirs.push(QHash<QString,bool>({{opt.strRemotePath,true}}));

    int iTotalCount = 1;
    int iCurrentCount = 0;

    /******先遍历远程上所有的目录和子目录*******/
    while (true)
    {
        //找到所有的目录
        QStringList allRemoteDir;
        {
            QHash<QString,bool>& tmpDirFiles = allChildrenFileAndDirs.top();
            for(auto itr=tmpDirFiles.begin(); itr!=tmpDirFiles.end(); ++itr)
            {
                if (itr.value())
                {
                    allRemoteDir.append(itr.key());
                }
            }
        }
        //没有目录说明已经到了树的最深层叶子节点
        if (allRemoteDir.isEmpty()){ break; }

        //获取当前树层级的每个目录下的子目录
        QHash<QString,bool> subDirFile;
        for (int i=0; i<allRemoteDir.size(); ++i)
        {
            QString strRemotePath = allRemoteDir[i];
            SFtpOptFile optTmp;
            optTmp.strLocalPath = "";
            optTmp.strRemotePath = strRemotePath;
            optTmp.opt = SFtpOption::OPT_LIST_REMOTE_DIR;

            QString strErrMsg;
            QList<QSsh::SftpFileInfo> allSftpFile;
            if (!listRemoteDir(optTmp,allSftpFile,strErrMsg))
            {
                emit signalFinishedRemoveRemoteDir(opt.strRemotePath, QString("list remote dir(%1) fail,%2")
                                                   .arg(strRemotePath).arg(strErrMsg));
                return false;
            }
            for (auto info : allSftpFile)
            {
                subDirFile.insert(optTmp.strRemotePath+'/'+info.name,QSsh::SftpFileType::FileTypeDirectory==info.type);
            }
        }
        if (subDirFile.isEmpty()) break;

        allChildrenFileAndDirs.push(subDirFile);
        iTotalCount += subDirFile.size();
    }

    /*******开始删除目录了，从子节点向上删除*******/
    while (!allChildrenFileAndDirs.isEmpty())
    {
        QHash<QString,bool> remoteDirFiles = allChildrenFileAndDirs.pop();
        for (auto itr=remoteDirFiles.begin(); itr!=remoteDirFiles.end(); ++itr)
        {
            SFtpOptFile optTmp;
            optTmp.strLocalPath = "";
            optTmp.strRemotePath = itr.key();
            if (itr.value())
            {
                optTmp.opt = SFtpOption::OPT_REMOVE_REMOTE_DIR;
                if (removeRemoteDir(optTmp))
                {
                    ++iCurrentCount;
                    emit signalRemoveRemoteDirProgress(opt.strRemotePath, optTmp.strRemotePath,
                                                 iTotalCount,iCurrentCount);
                }
                else
                {
                    emit signalFinishedRemoveRemoteDir(opt.strRemotePath,
                                                 QString("remove remote dir fail,because of remove dir(%1) failed")
                                                       .arg(optTmp.strRemotePath));
                    return false;
                }
            }
            else
            {
                optTmp.opt = SFtpOption::OPT_REMOVE_REMOTE_FILE;
                if (removeRemoteFile(optTmp))
                {
                    ++iCurrentCount;
                    emit signalRemoveRemoteDirProgress(opt.strRemotePath, optTmp.strRemotePath,
                                                 iTotalCount,iCurrentCount);
                }
                else
                {
                    emit signalFinishedRemoveRemoteDir(opt.strRemotePath,
                                                 QString("remove remote dir fail,because of remove file(%1) failed")
                                                       .arg(optTmp.strRemotePath));
                    return false;
                }
            }
        }
    }

    emit signalFinishedRemoveRemoteDir(opt.strRemotePath,"");
    return true;
}

bool CSFtpFileClient::createRemoteDir(const SFtpOptFile &opt)
{
    QString strErrMsg;
    QSsh::SftpFileInfo sftpFile;
    if (remotePathState(opt,sftpFile,strErrMsg))
    {//已经存在了，就没必要
        emit signalBeginCreateRemoteDir(opt.strRemotePath);
        emit signalFinishedCreateRemoteDir(opt.strRemotePath, "");
        return true;
    }

    QEventLoop* pLoop = new QEventLoop(this);
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::finished, pLoop, [=](QSsh::SftpJobId, const QString &error){
        emit signalFinishedCreateRemoteDir(opt.strRemotePath, error);
        pLoop->setProperty("ok",QVariant(error.isEmpty()));
        pLoop->quit();
    });

    QSsh::SftpJobId job = m_ptr->channel->createDirectory(opt.strRemotePath);
    if (job != QSsh::SftpInvalidJob)
    {
        emit signalBeginCreateRemoteDir(opt.strRemotePath);
        pLoop->exec();
    }
    else
    {
        emit signalFinishedCreateRemoteDir(opt.strRemotePath, QString("starting remove remote file error"));
    }
    bool bOk = pLoop->property("ok").toBool();
    pLoop->deleteLater();
    return bOk;
}

bool CSFtpFileClient::listRemoteDir(const SFtpOptFile &opt)
{
    emit signalBeginListRemoteDir(opt.strRemotePath);

    QString strErrMsg;
    QList<QSsh::SftpFileInfo> info;
    bool bOk = listRemoteDir(opt, info, strErrMsg);

    emit signalFinishedListRemoteDir(opt.strRemotePath, info, strErrMsg);
    return bOk;
}

bool CSFtpFileClient::listRemoteDir(const SFtpOptFile &opt, QList<QSsh::SftpFileInfo> &info, QString& strErr)
{
    QSharedPointer<QList<QSsh::SftpFileInfo>> sftpInfo(new QList<QSsh::SftpFileInfo>);
    QEventLoop* pLoop = new QEventLoop(this);
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::finished, pLoop, [=](QSsh::SftpJobId, const QString &error){
        pLoop->setProperty("errmsg",QVariant(error));
        pLoop->quit();
    });
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::fileInfoAvailable, pLoop,
            [=](QSsh::SftpJobId, const QList<QSsh::SftpFileInfo> &fileInfoList){
        for (auto tmp : fileInfoList)
        {
            if ("."!=tmp.name && ".."!=tmp.name)
            {
                sftpInfo->append(tmp);
            }
        }
    });

    QSsh::SftpJobId job = m_ptr->channel->listDirectory(opt.strRemotePath);
    if (job != QSsh::SftpInvalidJob)
    {
        pLoop->exec();
    }
    strErr = pLoop->property("errmsg").toString();
    pLoop->deleteLater();
    info = *sftpInfo.get();
    return strErr.isEmpty();
}

bool CSFtpFileClient::remotePathState(const SFtpOptFile& opt,QSsh::SftpFileInfo& info, QString& strErr)
{
    QSharedPointer<QSsh::SftpFileInfo> sftpInfo(new QSsh::SftpFileInfo);
    QEventLoop* pLoop = new QEventLoop(this);
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::finished, pLoop, [=](QSsh::SftpJobId, const QString &error){
        pLoop->setProperty("errmsg",QVariant(error));
        pLoop->quit();
    });
    connect(m_ptr->channel.data(), &QSsh::SftpChannel::fileInfoAvailable, pLoop,
            [=](QSsh::SftpJobId, const QList<QSsh::SftpFileInfo> &fileInfoList){
        for (auto tmp : fileInfoList)
        {
            (*sftpInfo.get()) = tmp;
            break;
        }
    });

    QSsh::SftpJobId job = m_ptr->channel->statFile(opt.strRemotePath);
    if (job != QSsh::SftpInvalidJob)
    {
        pLoop->exec();
    }
    strErr = pLoop->property("errmsg").toString();
    pLoop->deleteLater();
    info = *sftpInfo.get();
    return strErr.isEmpty();
}
