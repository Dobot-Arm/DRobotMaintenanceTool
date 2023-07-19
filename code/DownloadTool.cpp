#include "DownloadTool.h"

DownloadTool::DownloadTool(QObject* parent)
    : QObject(parent)
{
}

DownloadTool::~DownloadTool() {}

void DownloadTool::startDownload(const QString& downloadUrl, QString savePath)
{
    const QUrl newUrl = QUrl::fromUserInput(QUrl::fromPercentEncoding(downloadUrl.toUtf8()));

    if (!newUrl.isValid()) {
#ifdef DOWNLOAD_DEBUG
        qDebug() << QString("Invalid URL: %1: %2").arg(downloadUrl, newUrl.errorString());
#endif // DOWNLOAD_DEBUG
        return;
    }

    QString fileName = newUrl.fileName();

    if (fileName.isEmpty()) fileName = defaultFileName;
    if (savePath.isEmpty()) { savePath = QApplication::applicationDirPath() + "/tmp"; }
    if (!QFileInfo(savePath).isDir()) {
        QDir dir;
        dir.mkpath(savePath);
    }

    fileName.prepend(savePath + '/');
    if (QFile::exists(fileName)) { QFile::remove(fileName); }
    file = openFileForWrite(fileName);
    if (!file) return;

    startRequest(newUrl);
}

void DownloadTool::cancelDownload()
{
    httpRequestAborted = true;
    reply->abort();
}

void DownloadTool::httpFinished()
{
    QFileInfo fi;
    if (file) {
        fi.setFile(file->fileName());
        file->close();
        file.reset();
    }

    if (httpRequestAborted) {
        return;
    }

    if (reply->error()) {
        QFile::remove(fi.absoluteFilePath());
#ifdef DOWNLOAD_DEBUG
        qDebug() << QString("Download failed: %1.").arg(reply->errorString());
#endif // DOWNLOAD_DEBUG
        emit sigDownloadFinished(false);
        return;
    }

    const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if (!redirectionTarget.isNull()) {
        const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
        file = openFileForWrite(fi.absoluteFilePath());
        if (!file) { return; }
        startRequest(redirectedUrl);
        return;
    }

    emit sigDownloadFinished(true);

#ifdef DOWNLOAD_DEBUG
    qDebug() << QString(tr("Downloaded %1 bytes to %2 in %3")
        .arg(fi.size()).arg(fi.fileName(), QDir::toNativeSeparators(fi.absolutePath())));
    qDebug() << "Finished";
#endif // DOWNLOAD_DEBUG
}

void DownloadTool::httpReadyRead()
{
    if (file) {file->write(reply->readAll());}else{qDebug()<<"fail (((((((((((((";};
}

void DownloadTool::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    qreal progress = qreal(bytesRead) / qreal(totalBytes);
    emit sigProgress(bytesRead, totalBytes, progress);

#ifdef DOWNLOAD_DEBUG
    qDebug() << QString::number(progress * 100, 'f', 2) << "%    "
        << bytesRead / (1024 * 1024) << "MB" << "/" << totalBytes / (1024 * 1024) << "MB";
#endif // DOWNLOAD_DEBUG
}

void DownloadTool::startRequest(const QUrl& requestedUrl)
{
    url = requestedUrl;
    httpRequestAborted = false;
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &DownloadTool::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &DownloadTool::httpReadyRead);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadTool::networkReplyProgress);

#ifdef DOWNLOAD_DEBUG
    qDebug() << QString(tr("Downloading %1...").arg(url.toString()));
#endif // DOWNLOAD_DEBUG
}

std::unique_ptr<QFile> DownloadTool::openFileForWrite(const QString& fileName)
{
    std::unique_ptr<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly)) {
#ifdef DOWNLOAD_DEBUG
        qDebug() << QString("Unable to save the file %1: %2.")
            .arg(QDir::toNativeSeparators(fileName), file->errorString());
#endif // DOWNLOAD_DEBUG
        return nullptr;
    }
    return file;
}
