#ifndef CSFTPFILECLIENT_H
#define CSFTPFILECLIENT_H

#include <QObject>
#include <ssh/sshconnection.h>
#include <ssh/sftpchannel.h>

#include "SFtpConnectParam.h"

enum SFtpOption
{
    OPT_UPLOAD_FILE,             //上传文件
    OPT_UPLOAD_DIR,              //递归上传目录以及所有子目录和文件
    OPT_DOWNLOAD_FILE,           //下载文件
    OPT_DOWNLOAD_DIR,            //递归下载目录以及所有子目录和文件
    OPT_REMOVE_REMOTE_FILE,      //删除远程文件
    OPT_REMOVE_REMOTE_DIR,       //递归删除远程目录以及所有子目录和文件
    OPT_CREATE_REMOTE_DIR,       //创建远程空目录
    OPT_LIST_REMOTE_DIR          //列出远程目录当前的子目录和文件(只遍历一级目录)
};

struct SFtpOptFile
{
    QString strLocalPath;
    QString strRemotePath;
    SFtpOption opt;
};

struct CSFtpFileClientPrivate;
class CSFtpFileClient : public QObject
{
    Q_OBJECT
public:
    explicit CSFtpFileClient();
    CSFtpFileClient(const CSFtpFileClient&)=delete;
    CSFtpFileClient(CSFtpFileClient&&)=delete;
    CSFtpFileClient& operator=(const CSFtpFileClient&)=delete;
    CSFtpFileClient& operator=(CSFtpFileClient&&)=delete;
    ~CSFtpFileClient();

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
     *功  能：ssh文件操作请求
     *参  数：opt-请求参数，具体查看后续章节
     *返回值：无
     *说  明：该函数立刻返回，通过信号的方式告知执行情况
     */
    void ssh(SFtpOptFile opt);

signals:
    //连接成功的信号
    void signalConnected();
    //断开连接的信号
    void signalDisconnected();
    //连接失败的信号，strErr表示失败原因
    void signalConnectError(QString strErr);

    //文件上传时的发射信号
    void signalBeginUploadFile(QString strLocalFile, QString strRemoteFile);
    void signalUploadFileProgress(QString strLocalFile, QString strRemoteFile,
                                  quint64 iTotalSize, //上传文件的总大小
                                  quint64 iCurrentSize);//已上传的大小
    void signalFinishedUploadFile(QString strLocalFile, QString strRemoteFile,QString strErrMsg);

    /*上传目录时发射的信号
     * 因为上传目录实际就是创建目录和上传文件的组合，所以在上传过程中，也会发射：操作文件时的信号、创建目录时的信号。
    */
    void signalBeginUploadDir(QString strLocalDir, QString strRemoteDir);
    void signalUploadDirProgress(QString strLocalDir, QString strRemoteDir,
                                 QString strCurrentPath, //当前正在上传的文件或目录
                                 quint64 iTotalCount, //上传总个数
                                 quint64 iCurrentCount); //已经上传的个数
    void signalFinishedUploadDir(QString strLocalDir, QString strRemoteDir,QString strErrMsg);

    //文件下载时的发射信号
    void signalBeginDownloadFile(QString strLocalFile, QString strRemoteFile);
    void signalDownloadFileProgress(QString strLocalFile, QString strRemoteFile,
                                    quint64 iTotalSize, //下载文件的总大小
                                    quint64 iCurrentSize);//以下载的大小
    void signalFinishedDownloadFile(QString strLocalFile, QString strRemoteFile,QString strErrMsg);

    //下载目录时发射的信号
    void signalBeginDownloadDir(QString strLocalDir, QString strRemoteDir);
    void signalDownloadDirProgress(QString strLocalDir, QString strRemoteDir,
                                 QString strCurrentPath, //当前正在下载的文件或目录
                                 quint64 iTotalCount, //下载总个数
                                 quint64 iCurrentCount); //已经下载的个数
    void signalFinishedDownloadDir(QString strLocalDir, QString strRemoteDir,QString strErrMsg);

    //删除远程文件时发射信号
    void signalBeginRemoveRemoteFile(QString strRemoteFile);
    void signalFinishedRemoveRemoteFile(QString strRemoteFile,QString strErrMsg);

    //删除远程目录时发射的信号
    void signalBeginRemoveRemoteDir(QString strRemoteDir);
    void signalRemoveRemoteDirProgress(QString strRemoteDir,
                                 QString strCurrentPath, //当前正在删除的文件或目录
                                 quint64 iTotalCount, //删除总个数
                                 quint64 iCurrentCount); //已经删除的个数
    void signalFinishedRemoveRemoteDir(QString strRemoteDir,QString strErrMsg);

    //创建目录时发射的信号
    void signalBeginCreateRemoteDir(QString strRemoteDir);
    void signalFinishedCreateRemoteDir(QString strRemoteDir,QString strErrMsg);

    //列出、遍历目录时发射的信号
    void signalBeginListRemoteDir(QString strRemoteDir);
    void signalFinishedListRemoteDir(QString strRemoteDir,QList<QSsh::SftpFileInfo> fileInfoList,QString strErrMsg);

signals:
    void signalConnectHost(SFtpConnectParam prm,QPrivateSignal);
    void signalDisConnectHost(QPrivateSignal);

private slots:
    void slotConnectHost(SFtpConnectParam prm);
    void slotDisConnectHost();

    void onConnected();
    void onDisConnected();
    void onConnectionError(QSsh::SshError err);

    void onChannelInitialized();
    void onChannelError(QString strErrMsg);
    void onChannelClosed();

    void slotSSH();

    void insertSSH(SFtpOptFile opt);

private:
    bool uploadFile(const SFtpOptFile& opt);
    bool uploadDir(const SFtpOptFile& opt);
    bool downloadFile(const SFtpOptFile& opt);
    bool downloadDir(const SFtpOptFile& opt);
    bool removeRemoteFile(const SFtpOptFile& opt);
    bool removeRemoteDir(const SFtpOptFile& opt);
    bool removeRemoteDirRecursive(const SFtpOptFile& opt);
    bool createRemoteDir(const SFtpOptFile& opt);
    bool listRemoteDir(const SFtpOptFile& opt);
    bool listRemoteDir(const SFtpOptFile& opt, QList<QSsh::SftpFileInfo>& info, QString& strErr);
    bool remotePathState(const SFtpOptFile& opt,QSsh::SftpFileInfo& info, QString& strErr);
private:
    CSFtpFileClientPrivate* m_ptr;
};

#endif // CSFTPFILECLIENT_H
