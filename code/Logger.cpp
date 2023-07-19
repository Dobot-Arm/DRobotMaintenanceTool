#include "Logger.h"
#include <QCoreApplication>
#include <QPair>

#if defined(Q_OS_WIN)
#include <windows.h>
#endif

static constexpr qint64 LOG_FILE_MAX_SIZE = 100*1024*1024;
void myPrivateLogOutputCallback(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type)
    Q_UNUSED(context)
    if ((CLogger::getInstance()->isLogOnFile() || CLogger::getInstance()->isLogEmitted()) && !msg.isEmpty())
    {
        QString strPrefix(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
        switch(type)
        {
        case QtMsgType::QtDebugMsg:
            strPrefix.append(",DEBUG");
            break;
        case QtMsgType::QtInfoMsg:
            strPrefix.append(",INFO");
            break;
        case QtMsgType::QtWarningMsg:
            strPrefix.append(",WARN");
            break;
        case QtMsgType::QtCriticalMsg:
            strPrefix.append(",ERROR");
            break;
        case QtMsgType::QtFatalMsg:
            strPrefix.append(",FATAL");
            break;
        }

        if (nullptr!=context.file && nullptr!=context.function)
        {
            const char* pos = strrchr(context.file,QDir::separator().unicode());
            if (nullptr!=pos)
            {
                pos++;
            }
            else
            {
                pos = context.file;
            }
            strPrefix.append(QString::asprintf("(%s,%d,%s):",pos,context.line, context.function));
        }
        else
        {
            strPrefix.append(":");
        }

        CLogger::getInstance()->insertLog(QPair<QString,QString>(strPrefix,msg));
    }
}

CLogger::CLogger(QObject *parent) :
    QObject(parent),m_mtx(QMutex::NonRecursive),m_bThdRunning(false)
{
    m_iFileCurrentSize = 0;
    m_bIsPrintLogFile = false;
    m_bIsEmitted = false;
    m_logCallback = nullptr;
    m_strLogDir = qApp->applicationDirPath()+QDir::separator()+"Log-"+qApp->applicationName()+QDir::separator();
    QDir dir(m_strLogDir);
    if (!dir.exists())
    {
        dir.mkpath(m_strLogDir);
    }
    timeCleanLogs();

    std::thread thd(&CLogger::run,this);
    m_thdLog.swap(thd);
}

CLogger::~CLogger()
{
    m_bThdRunning = false;
    m_cond.notify_all();
    if (m_thdLog.joinable())
    {
        m_thdLog.join();
    }

    stopLogServer();
}

CLogger* CLogger::getInstance()
{
    static CLogger obj;
    return &obj;
}

void CLogger::startLogServer()
{
    QMutexLocker guard(&m_mtx);

    if (!m_logCallback)
    {
        m_logCallback = myPrivateLogOutputCallback;
        qInstallMessageHandler(m_logCallback);
    }

    return ;
}

void CLogger::stopLogServer()
{
    QMutexLocker guard(&m_mtx);

    if (m_logCallback)
    {
        m_logCallback = nullptr;
        qInstallMessageHandler(nullptr);
    }

    if (m_logFile.isOpen())
    {
        while (m_logCache.size()>0)
        {
            QPair<QString,QString> pk = m_logCache.takeFirst();
            QString str(pk.first+pk.second+"\n");
            m_logFile.write(str.toUtf8());
        }
        m_logFile.close();
    }

    return ;
}

void CLogger::setLogOnFile(bool bFlag)
{
    QMutexLocker guard(&m_mtx);
    m_bIsPrintLogFile = bFlag;
    installOrUninstallDebug();
}

void CLogger::setLogEmitted(bool bFlag)
{
    QMutexLocker guard(&m_mtx);
    m_bIsEmitted = bFlag;
    installOrUninstallDebug();
}

void CLogger::installOrUninstallDebug()
{
    if (m_bIsPrintLogFile || m_bIsEmitted)
    {
        if (!m_logCallback)
        {
            m_logCallback = myPrivateLogOutputCallback;
            qInstallMessageHandler(m_logCallback);
        }
    }
    else
    {
        if (m_logCallback)
        {
            m_logCallback = nullptr;
            qInstallMessageHandler(nullptr);
        }
    }
}

QString CLogger::getLogDir() const
{
    return m_strLogDir;
}

void CLogger::createNewLogFile()
{
    m_iFileCurrentSize = 0;
    if (m_logFile.isOpen())
    {
        m_logFile.close();
    }
    QString strNew = m_strLogDir+QDateTime::currentDateTime().toString("yyyyMMddhhmmss.zzz")+"-dbg.log";
    m_logFile.setFileName(strNew);
    if (m_logFile.open(QFile::NewOnly|QFile::WriteOnly|QFile::Append|QFile::Text))
    {
        //open ok
    }

    return ;
}

void CLogger::timeCleanLogs()
{
    QDir dir(m_strLogDir);
    QStringList filter;
    filter << "*-dbg.log";
    QFileInfoList fileInfo = dir.entryInfoList(filter);
    QDateTime deadTime = QDateTime::currentDateTime().addDays(-10);
    for (int i=0; i<fileInfo.count(); i++)
    {
        if (fileInfo.at(i).lastModified() < deadTime)
        {
            QFile::remove(fileInfo.at(i).absoluteFilePath());
        }
    }
}

void CLogger::clearAllLogFile()
{
    QDir dir(m_strLogDir);
    QStringList filter;
    filter << "*-dbg.log";
    QStringList fileInfo = dir.entryList(filter, QDir::Files|QDir::Writable, QDir::Name);
    foreach(QString strFile, fileInfo)
    {
        if (strFile != m_logFile.fileName())
        {
            QString path = dir.absolutePath() + QDir::separator() + strFile;
            QFile::remove(path);
        }
    }
}

void CLogger::insertLog(const QPair<QString,QString>& strLog)
{
    m_mtx.lock();
    m_logCache.append(strLog);
    m_mtx.unlock();
    m_cond.notify_all();

    return ;
}

void CLogger::run()
{
    m_bThdRunning = true;
    while (m_bThdRunning)
    {
        QPair<QString,QString> strLog;
        m_mtx.lock();
        if (m_logCache.isEmpty())
        {
            m_cond.wait(&m_mtx);
        }
        else
        {
            strLog = m_logCache.takeFirst();
        }
        m_mtx.unlock();
        if (!strLog.second.isEmpty())
        {
            writeLog(strLog.first, strLog.second);
        }
    }
    m_cond.notify_all();

    return ;
}

void CLogger::writeLog(const QString& strLogPrefix, const QString& strLogMsg)
{
#ifdef QT_DEBUG
        QString strLogPrint = strLogPrefix+strLogMsg;
    #if defined(Q_OS_WIN)
        OutputDebugString(reinterpret_cast<const wchar_t*>(strLogPrint.utf16()));
    #else
        fprintf(stderr, "%s", strLogPrint.toLocal8Bit().constData());
        fflush(stderr);
    #endif
#endif
    if (m_bIsPrintLogFile && !strLogMsg.isEmpty())
    {
        QMutexLocker guard(&m_mtx);
        if (!m_logFile.isOpen())
        {
            createNewLogFile();
        }
        if (m_logFile.isOpen())
        {
            QString str(strLogPrefix+strLogMsg+"\n");
            qint64 iRet = m_logFile.write(str.toUtf8());
            if (iRet > 0)
            {
                m_iFileCurrentSize += iRet;
                m_logFile.flush();
                if (m_iFileCurrentSize>=LOG_FILE_MAX_SIZE)
                {
                    m_iFileCurrentSize = 0;
                    createNewLogFile();
                }
            }
        }
    }

    if (m_bIsEmitted && !strLogMsg.isEmpty())
    {
        emit signalPrintLogMsg(strLogMsg);
    }

    return ;
}
