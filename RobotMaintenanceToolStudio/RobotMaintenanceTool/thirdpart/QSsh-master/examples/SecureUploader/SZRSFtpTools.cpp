#include "SZRSFtpTools.h"
#include "securefileuploader.h"
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

SZRSFtpTools::SZRSFtpTools(QObject* parent):
    QObject(parent)
{
    m_sftp = new SecureFileUploader(this);
    connect(m_sftp,&SecureFileUploader::sigDownLoadFinished,this,&SZRSFtpTools::DownloadFinished);
    connect(m_sftp,&SecureFileUploader::sigUpLoadFinished,this,&SZRSFtpTools::UploadFinished);
    connect(m_sftp,&SecureFileUploader::loadError,this,&SZRSFtpTools::LoadFiled);
}

SZRSFtpTools::~SZRSFtpTools()
{

}

void SZRSFtpTools::setSftpInfo(const QString &host, const QString &userName, const QString &pwd)
{
    m_sftp->setInfo(host,userName,pwd);
    m_host = host;
    m_userName = userName;
    m_pwd = pwd;
}

void SZRSFtpTools::downLoadFile(const QString &savePath, const QString &fname)
{   
    m_sftp->recvDownLoad(savePath,fname);
}

void SZRSFtpTools::downLoadFiles(const QString &savePath, const QStringList &fnames)
{
    m_sftp->recvDownLoadFiles(savePath,fnames);
}

void SZRSFtpTools::uploadFile(const QString &savePath, const QString &fname)
{
    m_sftp->recvUpFile(savePath,fname);
}

void SZRSFtpTools::uploadFiles(const QString &savePath, const QStringList &fnames)
{
    m_sftp->recvUpFiles(savePath,fnames);
}


