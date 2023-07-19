#ifndef SZRSFTPTOOLS_H
#define SZRSFTPTOOLS_H
#include <QString>
#include <QObject>

#ifdef SZRSFTP_LIB
# define SZRSFTP_EXPORT Q_DECL_EXPORT
#else
# define SZRSFTP_EXPORT Q_DECL_IMPORT
#endif

class SecureFileUploader;
class SZRSFTP_EXPORT SZRSFtpTools: public QObject
{
    Q_OBJECT
public:
    SZRSFtpTools(QObject* parent = NULL);
    ~SZRSFtpTools();
    void setSftpInfo(const QString& host,const QString& userName,const QString& pwd);
    void downLoadFile(const QString &savePath, const QString &fname);   // SFTP下载文件
    void downLoadFiles(const QString &savePath, const QStringList &fnames);   // SFTP下载文件
    void uploadFile(const QString &savePath, const QString &fname);    // SFTP上传文件
    void uploadFiles(const QString &savePath, const QStringList &fnames);    // SFTP上传文件
signals:
    void DownloadFinished();
    void UploadFinished();
    void LoadFiled();
private:
    QString m_host;
    QString m_userName;
    QString m_pwd;
    SecureFileUploader* m_sftp;
};

#endif // SZRSFTPTOOLS_H
