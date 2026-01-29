#ifndef PLINKCMD_H
#define PLINKCMD_H

#include <QObject>
#include <QProcess>
#include <QSet>

class PlinkCmd : public QObject
{
    Q_OBJECT
public:
    PlinkCmd(QObject* parent=nullptr);
    ~PlinkCmd();

    void setUser(const QString& strUser){m_strUser=strUser;}
    void setPwd(const QString& strPwd){m_strPwd=strPwd;}
    void setIp(const QString& strIp){m_strIp=strIp;}

    //执行命令，如果命令有空格，需要用引号括起来，比如创建创建目录 aa bb
    // mkdir "/tmp/tUpdate/slave/aa bb"
    void setPlink(const QString& strPlink){m_strPlink=strPlink;}
    void execCmd(const QString& cmd);

    void setPscp(const QString& strPscp){m_strPscp=strPscp;}
    void execUploadFile(QList<QPair<QStringList, //arrFiles
                        QString> //strDestDir
                        > allFiles);
    void execDownloadFile(QList<QPair<QString, //download file or dir
                          QString> //local save file or path
                          > allFiles);

    void kill();

signals:
    void signalRead(QString str);
    void signalFinishedCmd();
    void signalFinishedUploadFile();
    void signalFinishedDownloadFile();
    void signalDownloadFileProgress(QString strText);

private:
    QSet<QProcess*> m_lstProcess;

    QString m_strUser="root";
    QString m_strPwd="dobot";
    QString m_strIp;

    QString m_strPlink;
    QString m_strPscp;
};

#endif // PLINKCMD_H
