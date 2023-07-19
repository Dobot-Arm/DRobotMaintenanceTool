#ifndef CSHELLCMD_H
#define CSHELLCMD_H

#include <QObject>
#include <ssh/sshconnection.h>
#include <ssh/sshremoteprocess.h>

#include "SFtpConnectParam.h"

struct CShellCmdPrivate;
class CShellCmd : public QObject
{
    Q_OBJECT
public:
    explicit CShellCmd();
    CShellCmd(const CShellCmd&)=delete;
    CShellCmd(CShellCmd&&)=delete;
    CShellCmd& operator=(const CShellCmd&)=delete;
    CShellCmd& operator=(CShellCmd&&)=delete;
    ~CShellCmd();

    /*
     *功  能：连接ssh
     *参  数：prm-连接参数，具体查看后续章节
     *返回值：无
     */
    void connectHost(const SFtpConnectParam& prm);

    /*
     *功  能：断开ssh连接
     *参  数：无
     *返回值：无
     */
    void disconnectHost();

    /*
     * 功  能
     * 参  数
     * 返回值：true表示已连接，false表示未连接
    */
    bool isConnected() const;

    /*
     *功  能：写cmd
     *参  数：strCmd-命令
     *返回值：无
     */
    void writeCmd(const QString& strCmd);

signals:
    //连接成功的信号
    void signalConnected();
    //断开连接的信号
    void signalDisconnected();
    //连接失败的信号，strErr表示失败原因
    void signalConnectError(QString strErr);

    /**命令读到消息
     * bOk：true-表示writeCmd成功，false-表示writeCmd失败
     * strData：为writeCmd写入后读取到的消息内容，
     *         如果bOk为false则strData表示的是writeCmd执行失败的原因。
     *         如果bOk为true则strData表示writeCmd执行成功后读取到的内容。
    **/
    void signalDataArrived(bool bOk, QString strData);

signals:
    void signalConnectHost(SFtpConnectParam prm,QPrivateSignal);
    void signalDisConnectHost(QPrivateSignal);
    void signalWriteCmd(QString strCmd, QPrivateSignal);

private slots:
    void slotConnectHost(SFtpConnectParam prm);
    void slotDisConnectHost();

    void onConnected();
    void onDisConnected();
    void onConnectionError(QSsh::SshError err);

    void slotHandleShellWrite(QString strCmd);
    void slotHandleShellStarted();
    void slotHandleRemoteStdout();
    void slotHandleRemoteStderr();
    void slotHandleChannelClosed(int exitStatus);

private:
    CShellCmdPrivate* m_ptr;
};

#endif // CSHELLCMD_H
