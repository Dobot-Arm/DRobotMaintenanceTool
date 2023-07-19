#include "ShellCmd.h"
#include "Logger.h"

#include <QThread>

struct CShellCmdPrivate
{
    QThread* pThread;
    QSsh::SshConnection* pSshConnection;
    QSsh::SshRemoteProcess::Ptr shell;
    bool bIsConnected;
};

CShellCmd::CShellCmd() : QObject(nullptr)
{
    m_ptr = new CShellCmdPrivate;

    m_ptr->pSshConnection = nullptr;
    m_ptr->bIsConnected = false;

    qRegisterMetaType<SFtpConnectParam>();
    connect(this, &CShellCmd::signalConnectHost, this, &CShellCmd::slotConnectHost);
    connect(this, &CShellCmd::signalDisConnectHost, this, &CShellCmd::slotDisConnectHost);
    connect(this, &CShellCmd::signalWriteCmd, this, &CShellCmd::slotHandleShellWrite);

    m_ptr->pThread = new QThread();
    this->moveToThread(m_ptr->pThread);
    m_ptr->pThread->start();
}

CShellCmd::~CShellCmd()
{
    disconnectHost();
    m_ptr->pThread->quit();
    m_ptr->pThread->terminate();
    m_ptr->pThread->deleteLater();
    delete m_ptr;
}

void CShellCmd::connectHost(const SFtpConnectParam &prm)
{
    emit signalConnectHost(prm, QPrivateSignal());
}

void CShellCmd::disconnectHost()
{
    emit signalDisConnectHost(QPrivateSignal());
}

bool CShellCmd::isConnected() const
{
    return m_ptr->bIsConnected;
}

void CShellCmd::writeCmd(const QString &strCmd)
{
    QString str = strCmd+"\n";
    emit signalWriteCmd(str, QPrivateSignal());
}

void CShellCmd::slotConnectHost(SFtpConnectParam prm)
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
    connect(pSshConnection, &QSsh::SshConnection::connected, this, &CShellCmd::onConnected);
    connect(pSshConnection, &QSsh::SshConnection::disconnected, this, &CShellCmd::onDisConnected);
    connect(pSshConnection, &QSsh::SshConnection::error, this, &CShellCmd::onConnectionError);
    connect(pSshConnection, &QSsh::SshConnection::dataAvailable, this, [](QString strMsg){
        qDebug()<<"sshconnection dataavailable:"<<strMsg;
    });
    pSshConnection->connectToHost();
}

void CShellCmd::slotDisConnectHost()
{
    m_ptr->bIsConnected = false;
    if (m_ptr->pSshConnection)
    {
        m_ptr->pSshConnection->disconnectFromHost();
        m_ptr->pSshConnection->deleteLater();
        m_ptr->pSshConnection = nullptr;
    }
}

void CShellCmd::onConnected()
{
    m_ptr->shell = m_ptr->pSshConnection->createRemoteShell();
    connect(m_ptr->shell.data(), SIGNAL(started()), SLOT(slotHandleShellStarted()));
    connect(m_ptr->shell.data(), SIGNAL(readyReadStandardOutput()), SLOT(slotHandleRemoteStdout()));
    connect(m_ptr->shell.data(), SIGNAL(readyReadStandardError()), SLOT(slotHandleRemoteStderr()));
    connect(m_ptr->shell.data(), SIGNAL(closed(int)), SLOT(slotHandleChannelClosed(int)));
    m_ptr->shell->start();

    emit signalConnected();
}

void CShellCmd::onDisConnected()
{
    m_ptr->bIsConnected = false;
    if (!m_ptr->shell.isNull())
    {
        m_ptr->shell->close();
    }
    emit signalDisconnected();
}

void CShellCmd::onConnectionError(QSsh::SshError err)
{
    Q_UNUSED(err)
    emit signalConnectError(m_ptr->pSshConnection->errorString());
}

void CShellCmd::slotHandleShellWrite(QString strCmd)
{
    qint64 iSize = m_ptr->shell->write(strCmd.toUtf8());
    if (iSize <= -1)
    {
        emit signalDataArrived(false,m_ptr->shell->errorString());
    }
    else if (iSize != strCmd.length())
    {
        emit signalDataArrived(false,"write error");
    }
}

void CShellCmd::slotHandleShellStarted()
{
    m_ptr->bIsConnected = true;
    qDebug()<<"slotHandleShellStarted";
}

void CShellCmd::slotHandleRemoteStdout()
{
    QString allText;
    while (m_ptr->shell->canReadLine())
    {
        QString strData(m_ptr->shell->readAllStandardOutput());
        allText.append(strData);
    }
    emit signalDataArrived(true,allText);
}

void CShellCmd::slotHandleRemoteStderr()
{
    QString allText;
    while (m_ptr->shell->canReadLine())
    {
        QString strData(m_ptr->shell->readAllStandardError());
        allText.append(strData);
    }
    emit signalDataArrived(true,allText);
}

void CShellCmd::slotHandleChannelClosed(int exitStatus)
{
    m_ptr->bIsConnected = false;
    qDebug()<<"slotHandleChannelClosed,exitStatus="<<exitStatus;
}
