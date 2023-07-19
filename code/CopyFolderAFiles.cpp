#include "CopyFolderAFiles.h"

CopyFolderAFiles::CopyFolderAFiles()
{
    connect(this,&CopyFolderAFiles::signal_copyFolder,this,&CopyFolderAFiles::copyFolder);
    m_thread = new QThread();
    this->moveToThread(m_thread);
    m_thread->start();
}

bool CopyFolderAFiles::copyFolder(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
    if(!copyFolderHandle(fromDir,toDir,coverFileIfExist)){
        emit signal_copyFolderFinished(false);
        return false;
    }
    emit signal_copyFolderFinished(true);
    return true;
}



bool CopyFolderAFiles::deleteFolder(const QString &dirPath)
{
    QDir dir;
    dir.setPath(dirPath);
    return dir.removeRecursively();
}

bool CopyFolderAFiles::copyFolderHandle(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
    QDir srcDir(fromDir);
    QString srcDirName =srcDir.dirName();
    srcDir.cdUp();
    QString srcDirPathName = srcDir.canonicalPath();
    qDebug()<<"srcDirName --- " <<srcDirName ;

    QDir dstDir(toDir);
    if (!dstDir.exists()) { //目的文件目录不存在则创建文件目录
       if (!dstDir.mkpath(dstDir.absolutePath()))
           return false;
    }

    QQueue<QString> srcDirAFileQQueue;
    QStringList srcDirAFileCount;
    srcDirAFileQQueue.enqueue(fromDir);
    while(!srcDirAFileQQueue.isEmpty()){
        QDir dir(srcDirAFileQQueue.dequeue());
        QFileInfoList fileInfoList = dir.entryInfoList();
        foreach(QFileInfo fileInfo, fileInfoList){
            if (fileInfo.fileName() == "." || fileInfo.fileName() == "..") continue;
            srcDirAFileCount.append(dir.filePath(fileInfo.fileName()));
            srcDirAFileQQueue.enqueue(dir.filePath(fileInfo.fileName()));
        }
    }
    QString dstDirPath = dstDir.absolutePath()+"/"+srcDirName;
    qDebug()<<"dstFilePath "<<dstDirPath;
    QDir dstFileDir(dstDirPath);
    if(!dstFileDir.exists()){
        dstFileDir.mkpath(dstDirPath);
    }
    int progresssCount = 0;
    for(QString pathAFile:srcDirAFileCount){
        QString dstFile = pathAFile;
        dstFile = dstFile.replace(0,srcDirPathName.length(),toDir);
//        qDebug()<< "pathAFile -- "<<dstFile;
        QDir transFormDir(pathAFile);
        QFileInfo transFormFileInfo(pathAFile);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        if(transFormFileInfo.isDir()){
            transFormDir.mkpath(dstFile);
        }else{
            if(QFile::exists(dstFile)&&coverFileIfExist){
                QFile::remove(dstFile);
            }
            if (!QFile::copy(pathAFile,dstFile)) {
                qDebug()<<" fail "<<pathAFile << "  "<<dstFile;
                dstFileDir.removeRecursively();
                return false;
            }
        }
        signal_copyFolderProgress(++progresssCount,srcDirAFileCount.length());


    }
    return true;

//    QDir srcDir(fromDir);
//    QDir dstDir(toDir);
//    if (!dstDir.exists()) { //目的文件目录不存在则创建文件目录
//       if (!dstDir.mkpath(dstDir.absolutePath()))
//           return false;
//    }
//    QFileInfoList fileInfoList = srcDir.entryInfoList();
//    foreach(QFileInfo fileInfo, fileInfoList) {
//       if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
//           continue;

//       if (fileInfo.isDir()) {    // 当为目录时，递归的进行copy
//           if (!copyFolderHandle(fileInfo.filePath(),dstDir.filePath(fileInfo.fileName()),coverFileIfExist))
//               return false;
//       }
//       else {            //当允许覆盖操作时，将旧文件进行删除操作
//           if (coverFileIfExist && dstDir.exists(fileInfo.fileName())) {
//               dstDir.remove(fileInfo.fileName());
//           }
//           /// 进行文件copy
//           if (!QFile::copy(fileInfo.filePath(), dstDir.filePath(fileInfo.fileName()))) {
//               return false;
//           }
//       }
//    }
//     return true;
}
