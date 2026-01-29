#include "ZipDiyManager.h"
#include <memory>
#include <QProcess>
#include <QApplication>
#include <QEventLoop>
TCHAR *CharToWchar(const QString &str);
QString WcharToChar(const TCHAR *wp, size_t codePage);
struct ZipFileInfo{
    QString dstFilePath;
    QFileInfo fileInfo;
    QString srcFilePath;
    bool isDir;
};

ZipDiyManager::ZipDiyManager():QObject(nullptr)
{

}

bool ZipDiyManager::zipDiyCompress(QString strSrcDir, QString strDstZip,QString password)
{
    QDir srcDir(strSrcDir);
    if(srcDir.isEmpty()){
        return false;
    }
    HZIP hz = CreateZip(CharToWchar(strDstZip),password.toStdString().c_str());
    int dirCount = 0;
    int fileCount = 0;
    QFileInfoList fileInfoList =  srcDir.entryInfoList();
    QList<QFileInfo> fileInfoQueue;
    fileInfoQueue = fileInfoList;
    while(!fileInfoQueue.isEmpty()){
        QFileInfo fileInfo = fileInfoQueue.takeFirst();
        if(fileInfo.fileName()=="."||fileInfo.fileName()==".."){
            continue;
        }
        QString dstFilePath = fileInfo.absoluteFilePath();
        dstFilePath.remove(0,srcDir.absolutePath().count());
        dstFilePath.prepend(srcDir.dirName());
        qDebug()<<"dstFilePath  "<<dstFilePath;
        if(fileInfo.isDir()){
            fileInfoQueue.append(QDir(fileInfo.absoluteFilePath()).entryInfoList());
//            qDebug()<<"fileInfo.fileName()  "<<fileInfo.fileName();
//            qDebug()<<"fileInfo.absoluteFilePath()  "<<fileInfo.absoluteFilePath();
            if(ZipAddFolder(hz,CharToWchar(dstFilePath+"/"))!=0){
                QDir removeDir(strDstZip);
                removeDir.removeRecursively();
                return false;
            }
            dirCount++;
        }else{
//            qDebug()<<"fileInfo.fileName()  "<<fileInfo.fileName();
//            qDebug()<<"fileInfo.absoluteFilePath()  "<<fileInfo.absoluteFilePath();
            if(ZipAdd(hz,CharToWchar(dstFilePath),CharToWchar(fileInfo.absoluteFilePath()))!=0){
                QDir removeDir(strDstZip);
                removeDir.removeRecursively();
                return false;
            }
            fileCount++;
        }
    }
    qDebug()<<"fileCount"<< fileCount;
    qDebug()<<"dirCount"<< dirCount;
//    long result1 = ZipAdd(hz,L"mywritetest.txt", L"\\\\192.168.1.6\\project\\mywritetest.txt");
//    long result2 = ZipAdd(hz,L"simple.txt", L"f:\\simple.txt");
//    long result3 = ZipAddFolder(hz,L"Tencent");
//    qDebug()<<QString("%1 %2 %3").arg(result1).arg(result2).arg(result3);
    CloseZip(hz);
    return true;
}

bool ZipDiyManager::zipUnCompress(QString strSrcZip, QString strDstPath,QString password)
{
    QFileInfo srcZip(strSrcZip);
    if(!srcZip.exists()){
        qDebug()<<"压缩包不存在";
        return false;
    }

    QString strPwd(password);
    QString strFile(strSrcZip);
    QString strSavePath(strDstPath);
    QEventLoop loop;
    QProcess p;
    p.setArguments(QStringList()<<"x"
                    <<strFile<<QString("-o%1").arg(strSavePath)
                   <<QString("-p%1").arg(strPwd)
                   <<"-y");
    p.setProgram(QCoreApplication::applicationDirPath()+"/tool/7-Zip/7z.exe");
    connect(&p, &QProcess::errorOccurred, this, [&](QProcess::ProcessError error){
        qDebug().noquote()<<"zipuncompress fail:"<<p.errorString();
        loop.quit();
    });
    connect(&p, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished), this, [&](int exitCode, QProcess::ExitStatus exitStatus){
        qDebug().noquote()<<"zipuncompress finished:"<<exitStatus;
        loop.quit();
    });
    p.start();
    loop.exec();
    QString strInfo = p.readAll();
    qDebug().noquote()<<"strInfo-------------------> "<<strInfo;
    if (strInfo.contains("Everything is Ok"))
    {
        return true;
    }
    return false;
}

bool ZipDiyManager::zipCompressFilterDir(QString strSrcDir, QString strDstZip, QString password, QStringList filteredDirList,QString strSelectedDir)
{
    QList<ZipFileInfo> allZipFileInfoQueue;
    QDir srcDir(strSrcDir);
    if(srcDir.isEmpty()){
        return false;
    }
    HZIP hz = CreateZip(CharToWchar(strDstZip),password.toStdString().c_str());

    QFileInfoList fileInfoList = srcDir.entryInfoList();
    for(QString filteredDir:filteredDirList){
        fileInfoList.removeOne(strSrcDir+"/"+filteredDir);
    }

    for(int i=0; i<fileInfoList.count(); i++){
        QFileInfo fileInfo = fileInfoList.at(i);
        if(fileInfo.fileName()=="."||fileInfo.fileName()==".."){
            continue;
        }
        ZipFileInfo zipFileInfo;
        QString dstFilePath = fileInfo.absoluteFilePath();
        dstFilePath.remove(0,srcDir.absolutePath().count());
        dstFilePath.prepend(srcDir.dirName());
        zipFileInfo.dstFilePath = dstFilePath;
        zipFileInfo.srcFilePath = fileInfo.absoluteFilePath();
        zipFileInfo.fileInfo = fileInfo;
//        qDebug()<<"dstFilePath  "<<dstFilePath;
        if(fileInfo.isDir()){
            fileInfoList.append(QDir(fileInfo.absoluteFilePath()).entryInfoList());
            zipFileInfo.isDir = true;
        }else{
            zipFileInfo.isDir = false;
        }
        allZipFileInfoQueue.append(zipFileInfo);

    }

    if(!strSelectedDir.isEmpty()){
        QDir selectedDir(strSelectedDir);
        QList<QFileInfo> selectedFileInfoQueue;
        selectedFileInfoQueue = selectedDir.entryInfoList();
        while(!selectedFileInfoQueue.isEmpty()){
            QFileInfo fileInfo = selectedFileInfoQueue.takeFirst();
            if(fileInfo.fileName()=="."||fileInfo.fileName()==".."){
                continue;
            }
            ZipFileInfo zipFileInfo;
            QString selectedDstFilePath = fileInfo.absoluteFilePath();
            selectedDstFilePath.remove(0,selectedDir.absolutePath().count());
            selectedDstFilePath.prepend(selectedDir.dirName());
            qDebug()<<"dstFilePath  "<<selectedDstFilePath;
            zipFileInfo.dstFilePath = selectedDstFilePath;
            zipFileInfo.srcFilePath = fileInfo.absoluteFilePath();
            zipFileInfo.fileInfo = fileInfo;
            if(fileInfo.isDir()){
                selectedFileInfoQueue.append(QDir(fileInfo.absoluteFilePath()).entryInfoList());
                zipFileInfo.isDir = true;
            }else{
                zipFileInfo.isDir = false;
            }
            allZipFileInfoQueue.append(zipFileInfo);
        }
    }

    qDebug()<<"allZipFileInfoQueue.count()  "<<allZipFileInfoQueue.count();
    for(ZipFileInfo zipFileInfo : allZipFileInfoQueue){
        if(zipFileInfo.isDir){
            if(ZipAddFolder(hz,CharToWchar(zipFileInfo.dstFilePath+"/"))!=0){
                QDir removeDir(strDstZip);
                removeDir.removeRecursively();
                CloseZip(hz);
                return false;
            }
        }else{
            if(ZipAdd(hz,CharToWchar(zipFileInfo.dstFilePath),CharToWchar(zipFileInfo.srcFilePath))!=0){
                QDir removeDir(strDstZip);
                removeDir.removeRecursively();
                CloseZip(hz);
                return false;
            }
        }
    }
    CloseZip(hz);
    return true;
}
TCHAR *CharToWchar(const QString &str)
{
    QByteArray ba = str.toUtf8();
    char *data = ba.data(); //以上两步不能直接简化为“char *data = str.toUtf8().data();”
    int charLen = strlen(data);
    int len = MultiByteToWideChar(CP_UTF8, 0, data, charLen, NULL, 0);
    TCHAR *buf = new TCHAR[len + 1];
    MultiByteToWideChar(CP_UTF8, 0, data, charLen, buf, len);
    buf[len] = '\0';
    return buf;
}

QString WcharToChar(const TCHAR *wp, size_t codePage)
{
    QString str;
    int len = WideCharToMultiByte(codePage, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
    char *p = new char[len + 1];
    memset(p, 0, len + 1);
    WideCharToMultiByte(codePage, 0, wp, wcslen(wp), p, len, NULL, NULL);
    p[len] = '\0';
    str = QString(p);
    delete p;
    p = NULL;
    return str;
}

