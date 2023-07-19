#ifndef COPYFOLDERAFILES_H
#define COPYFOLDERAFILES_H

#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QThread>
#include <QDebug>
#include <QCoreApplication>
#include <QQueue>
#include <QProgressDialog>
class CopyFolderAFiles : public QObject
{
    Q_OBJECT
public:
    CopyFolderAFiles();
private slots:
    bool copyFolder(const QString& fromDir, const QString& toDir, bool coverFileIfExist);
    bool deleteFolder(const QString& dirPath);
signals:
    void signal_copyFolder(const QString& fromDir, const QString& toDir, bool coverFileIfExist);
    void signal_deleteFolder(const QString& dirPath);
    void signal_copyFolderFinished(bool ok);
    void signal_copyFolderProgress(int progress,int total);
private:
    QThread* m_thread;
    bool copyFolderHandle(const QString& fromDir, const QString& toDir, bool coverFileIfExist);
};

#endif // COPYFOLDERAFILES_H
