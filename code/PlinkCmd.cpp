#include "PlinkCmd.h"
#include "Logger.h"

PlinkCmd::PlinkCmd(QObject* parent):QObject(parent)
{
}

PlinkCmd::~PlinkCmd()
{
    for(QProcess* p:m_lstProcess)
    {
        p->kill();
        p->deleteLater();
    }
    m_lstProcess.clear();
}

void PlinkCmd::execCmd(const QString &cmd)
{
    //plink.exe -ssh -l root -pw dobot 194.168.5.1 "mkdir /tmp/a && rm -rf /tmp/aa"
    QStringList args;
    args<<"-ssh"<<"-l"<<m_strUser<<"-pw"<<m_strPwd<<m_strIp;
    args.append(cmd);
    //执行命令
    QProcess *pProg = new QProcess(this);
    m_lstProcess.insert(pProg);
    connect(pProg, &QProcess::readyRead, this, [this,pProg]{
        QString str = pProg->readAll();
        qDebug()<<"[PlinkCmd::execCmd->read]"<<str;
        emit signalRead(str);
    });
    connect(pProg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,[this,pProg](int exitCode, QProcess::ExitStatus exitStatus){
        qDebug()<<"[PlinkCmd::execCmd-finished]exitCode="<<exitCode<<",exitStatus="<<exitStatus;
        m_lstProcess.remove(pProg);
        pProg->deleteLater();
        emit signalFinishedCmd();
    });
    pProg->setProgram(m_strPlink);
    pProg->setArguments(args);
    qDebug()<<"plink:"<<m_strPlink;
    qDebug()<<"arguments:"<<pProg->arguments();

    pProg->start();
}

void PlinkCmd::execUploadFile(QList<QPair<QStringList, //arrFiles
                              QString> //strDestDir
                              > allFiles)
{
    int iCount = 0;
    for(auto itr=allFiles.begin(); itr!=allFiles.end(); ++itr)
    {
        if (itr->first.isEmpty()) continue;
        iCount++;
    }

    std::shared_ptr<int> ptrRefCount(new int(iCount));
    for(auto itr=allFiles.begin(); itr!=allFiles.end(); ++itr)
    {
        if (itr->first.isEmpty()) continue;

        //pscp.exe -pw dobot -r 本地文件1 本地文件2...本地文件n root@192.168.5.1:远程目录
        //pscp.exe -pw dobot -r 本地目录1 本地目录2...本地目录n root@192.168.5.1:远程目录
        //pscp.exe -pw dobot -r 本地文件1 本地目录1 ... 本地文件n 本地目录n root@192.168.5.1:远程目录
        //pscp.exe -pw dobot -r 本地文件 root@192.168.5.1:远程文件
        //远程目录可以以斜杠结尾
        QStringList args;
        args<<"-pw"<<m_strPwd<<"-r";
        args.append(itr->first);
        args<<QString("%1@%2:%3").arg(m_strUser).arg(m_strIp).arg(itr->second);

        //执行命令
        QProcess *pProg = new QProcess(this);
        m_lstProcess.insert(pProg);
        connect(pProg, &QProcess::errorOccurred, this,[pProg](QProcess::ProcessError err){
           qDebug()<<"[PlinkCmd::execUploadFile->errorOccurred]"<<err<<","<<pProg->errorString();
        });
        connect(pProg, &QProcess::readyRead, this, [this,pProg]{
            QString str = pProg->readAll();
            qDebug()<<"[PlinkCmd::execUploadFile->read]"<<str;
            emit signalRead(str);
        });
        connect(pProg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,[this,pProg,ptrRefCount](int exitCode, QProcess::ExitStatus exitStatus){
            qDebug()<<"[PlinkCmd::execUploadFile-finished]exitCode="<<exitCode<<",exitStatus="<<exitStatus;
            m_lstProcess.remove(pProg);
            pProg->deleteLater();

            (*ptrRefCount)--;
            if ((*ptrRefCount)<=0)
            {
                emit signalFinishedUploadFile();
            }
        });
        pProg->setProgram(m_strPscp);
        pProg->setArguments(args);
        qDebug()<<"pscp:"<<m_strPscp;
        qDebug()<<"arguments:"<<pProg->arguments();
        pProg->start();
    }
}

void PlinkCmd::execDownloadFile(QList<QPair<QString, QString> > allFiles)
{
    int iCount = 0;
    for(auto itr=allFiles.begin(); itr!=allFiles.end(); ++itr)
    {
        if (itr->first.isEmpty()) continue;
        iCount++;
    }

    std::shared_ptr<int> ptrRefCount(new int(iCount));
    for(auto itr=allFiles.begin(); itr!=allFiles.end(); ++itr)
    {
        if (itr->first.isEmpty() || itr->second.isEmpty()) continue;
        //下载文件/目录，一次只能下载一个
        //pscp.exe -pw dobot -r root@192.168.5.1:远程文件 本地目录
        //pscp.exe -pw dobot -r root@192.168.5.1:远程文件 本地文件
        //pscp.exe -pw dobot -r root@192.168.5.1:远程目录 本地目录
        //远程目录不要以*或者斜杠结尾，本地目录不能以斜杠结尾，且尽量保证本地目录存在
        //下载文件时，如果指定本地文件则用本地文件命名保存，如果指定目录则以下载的文件名命名保存
        QStringList args;
        args<<"-pw"<<m_strPwd<<"-r";
        args<<QString("%1@%2:%3").arg(m_strUser).arg(m_strIp).arg(itr->first);
        args<<itr->second;

        //执行命令
        QProcess *pProg = new QProcess(this);
        m_lstProcess.insert(pProg);
        connect(pProg, &QProcess::readyRead, this, [this,pProg]{
            QString str = pProg->readAll();
            qDebug()<<"[PlinkCmd::execDownloadFile->read]"<<str;
            emit signalDownloadFileProgress(str);
        });
        connect(pProg, &QProcess::errorOccurred, this,[pProg](QProcess::ProcessError err){
           qDebug()<<"[PlinkCmd::execDownloadFile->errorOccurred]"<<err<<","<<pProg->errorString();
        });
        connect(pProg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,[this,pProg,ptrRefCount](int exitCode, QProcess::ExitStatus exitStatus){
            qDebug()<<"[PlinkCmd::execDownloadFile-finished]exitCode="<<exitCode<<",exitStatus="<<exitStatus;
            m_lstProcess.remove(pProg);
            pProg->deleteLater();

            (*ptrRefCount)--;
            if ((*ptrRefCount)<=0)
            {
                emit signalFinishedDownloadFile();
            }
        });
        pProg->setProgram(m_strPscp);
        pProg->setArguments(args);
        qDebug()<<"pscp:"<<m_strPscp;
        qDebug()<<"arguments:"<<pProg->arguments();
        pProg->start();
    }
}

void PlinkCmd::kill()
{
    for(QProcess* p:m_lstProcess)
    {
        p->kill();
        p->deleteLater();
    }
    m_lstProcess.clear();
}
