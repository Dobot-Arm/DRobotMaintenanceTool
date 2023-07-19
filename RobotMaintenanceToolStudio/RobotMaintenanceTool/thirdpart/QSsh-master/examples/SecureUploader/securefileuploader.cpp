/**************************************************************************
**
** This file is part of QSsh
**
** Copyright (c) 2012 LVK
**
** Contact: andres.pagliano@lvklabs.com
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**************************************************************************/

#include "securefileuploader.h"

#include <QtDebug>
#include <QFileInfo>

SecureFileUploader::SecureFileUploader(QObject *parent) :
    QObject(parent), m_connection(0)
{
}

void SecureFileUploader::setInfo(const QString &host, const QString &userName, const QString &passwd)
{
    m_host = host;
    m_userName = userName;
    m_pwd = passwd;
}

void SecureFileUploader::SftpDownLoadClient()
{
    if(m_connection)
        m_connection->disconnectFromHost();
    QSsh::SshConnectionParameters params;
    params.host = m_host;
    params.userName = m_userName;
    params.password = m_pwd;
    params.authenticationType = QSsh::SshConnectionParameters::AuthenticationTypePassword;
    params.timeout = 300;
    params.port = 22;
    m_connection = new QSsh::SshConnection(params, this); // TODO free this pointer!
    connect(m_connection, SIGNAL(connected()), SLOT(onConnected()));
    connect(m_connection, SIGNAL(error(QSsh::SshError)), SLOT(onConnectionError(QSsh::SshError)));

    qDebug() << "SecureUploader: Connecting to host" << m_host;

    m_connection->connectToHost();

}

void SecureFileUploader::SftpUpLoadClient()
{
    if(m_connection)
        m_connection->disconnectFromHost();
    QSsh::SshConnectionParameters params;
    params.host = m_host;
    params.userName = m_userName;
    params.password = m_pwd;
    params.authenticationType = QSsh::SshConnectionParameters::AuthenticationTypePassword;
    params.timeout = 300;
    params.port = 22;

    m_connection = new QSsh::SshConnection(params, this); // TODO free this pointer!
    connect(m_connection, SIGNAL(connected()), SLOT(onUpConnected()));
    connect(m_connection, SIGNAL(error(QSsh::SshError)), SLOT(onConnectionError(QSsh::SshError)));

    qDebug() << "SecureUploader: Connecting to host" << m_host;

    m_connection->connectToHost();
}

void SecureFileUploader::uploadFile()
{
   QFileInfo fileInfo(upFileName);
   QString SftpSavePath  = upSavePath;
   SftpSavePath += "/";
   SftpSavePath += "K"+fileInfo.fileName();
   QSsh::SftpJobId job = m_channel->uploadFile(upFileName, SftpSavePath,QSsh::SftpOverwriteExisting);
   if (job != QSsh::SftpInvalidJob) {
       qDebug() << "SecureUploader: Starting job #" << job;
   } else {
       emit loadError();
       qDebug() << "SecureUploader: Invalid Job";
   }
}

void SecureFileUploader::downLoadFile()
{
    QFileInfo fileInfo(DownFileName);
    QString SftpSavePath  = DownSavePath;
    SftpSavePath += "/";
    SftpSavePath += fileInfo.fileName();
    QSsh::SftpJobId job = m_channel->downloadFile(DownFileName,SftpSavePath,QSsh::SftpOverwriteExisting);
    if (job != QSsh::SftpInvalidJob) {
        qDebug() << "SecureUploader: Starting job #" << job;
    } else {
        emit loadError();
        qDebug() << "SecureUploader: Invalid Job";
    }
}

void SecureFileUploader::onConnected()
{
    qDebug() << "SecureUploader: Connected";
    qDebug() << "SecureUploader: Creating SFTP channel...";

    m_channel = m_connection->createSftpChannel();

    if (m_channel) {
        connect(m_channel.data(), SIGNAL(initialized()),
                SLOT(downLoadFile()));
        connect(m_channel.data(), SIGNAL(initializationFailed(QString)),
                SLOT(onChannelError(QString)));
        connect(m_channel.data(), SIGNAL(finished(QSsh::SftpJobId, QString)),
                SLOT(onDownLoadfinished(QSsh::SftpJobId, QString)));
        m_channel->initialize();
    } else {
        qDebug() << "SecureUploader: Error null channel";
    }
}

void SecureFileUploader::onUpConnected()
{
    qDebug() << "SecureUploader: Connected";
    qDebug() << "SecureUploader: Creating SFTP channel...";

    m_channel = m_connection->createSftpChannel();

    if (m_channel) {
        connect(m_channel.data(), SIGNAL(initialized()),
                SLOT(uploadFile()));
        connect(m_channel.data(), SIGNAL(initializationFailed(QString)),
                SLOT(onChannelError(QString)));
        connect(m_channel.data(), SIGNAL(finished(QSsh::SftpJobId, QString)),
                SLOT(onUpLoadFinished(QSsh::SftpJobId,QString)));
        m_channel->initialize();
    } else {
        qDebug() << "SecureUploader: Error null channel";
    }
}

void SecureFileUploader::onConnectionError(QSsh::SshError err)
{
    qDebug() << "SecureUploader: Connection error" << err;
}

void SecureFileUploader::onChannelInitialized()
{
    qDebug() << "SecureUploader: Channel Initialized";
}

void SecureFileUploader::onChannelError(const QString &err)
{
    qDebug() << "SecureUploader: Error: " << err;
}

void SecureFileUploader::onDownLoadfinished(QSsh::SftpJobId job, const QString &err)
{
    qDebug() << "SecureUploader: Finished DownLoad job #" << job << ":" << (err.isEmpty() ? "OK" : err);
    if (DownLoadFiles.isEmpty())
    {
        delete m_connection;
        m_connection = NULL;
        emit sigDownLoadFinished();
    }
    else
    {
        DownFileName = DownLoadFiles.takeFirst();
        downLoadFile();
    }
}

void SecureFileUploader::onUpLoadFinished(QSsh::SftpJobId job, const QString &error)
{
    qDebug() << "SecureUploader: Finished UpDownLoad job #" << job << ":" << (error.isEmpty() ? "OK" : error);
    if (upLoadFiles.isEmpty())
    {
        delete m_connection;
        m_connection = NULL;
        emit sigUpLoadFinished();
    }
    else
    {
        upFileName = upLoadFiles.takeFirst();
        uploadFile();
    }
}

void SecureFileUploader::recvDownLoad(const QString &savePath, const QString &fname)
{

    DownSavePath = savePath;
    DownFileName = fname;
    SftpDownLoadClient();
}

void SecureFileUploader::recvUpFile(const QString &savePath, const QString &fname)
{
    upSavePath = savePath;
    upFileName = fname;
    SftpUpLoadClient();
}

void SecureFileUploader::recvUpFiles(const QString &savePath, const QStringList &fnames)
{
    upSavePath = savePath;
    upLoadFiles = fnames;
    upFileName = upLoadFiles.takeFirst();
    SftpUpLoadClient();
}

void SecureFileUploader::recvDownLoadFiles(const QString &savePath, const QStringList &fnames)
{
    DownSavePath = savePath;
    DownLoadFiles = fnames;
    DownFileName = DownLoadFiles.takeFirst();
    SftpDownLoadClient();
}


