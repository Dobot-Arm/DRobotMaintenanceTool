#ifndef CLOGGER_H
#define CLOGGER_H

#include <QObject>
#include <QMessageLogger>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QMutex>
#include <QWaitCondition>
#include <QLinkedList>
#include <thread>

/*
#define LOG_TRACE(logAPI) CDebugLogger(__FILE__,__LINE__,__FUNCTION__).logAPI() \
                          .setDebug(CLogger::getInstance()->isLogOnFile()||CLogger::getInstance()->isLogEmitted())

#define LOG_DEBUG() LOG_TRACE(debug)
#define LOG_INFO() LOG_TRACE(info)
#define LOG_WARN() LOG_TRACE(warning)
#define LOG_ERROR() LOG_TRACE(critical)
*/

#ifdef qDebug
#undef qDebug
#endif
//#define qDebug LOG_DEBUG
#define qDebug() QMessageLogger(__FILE__,__LINE__,__FUNCTION__).debug().nospace().noquote()

#ifdef qInfo
#undef qInfo
#endif
//#define qInfo LOG_INFO
#define qInfo() QMessageLogger(__FILE__,__LINE__,__FUNCTION__).info().nospace().noquote()

#ifdef qWarning
#undef qWarning
#endif
//#define qWarning LOG_WARN
#define qWarning() QMessageLogger(__FILE__,__LINE__,__FUNCTION__).warning().nospace().noquote()

#ifdef qCritical
#undef qCritical
#endif
//#define qCritical LOG_ERROR
#define qCritical() QMessageLogger(__FILE__,__LINE__,__FUNCTION__).critical().nospace().noquote()

class CLogger : public QObject
{
    Q_OBJECT
public:
    Q_DISABLE_COPY(CLogger)
    ~CLogger();

    static CLogger* getInstance();

    void startLogServer();
    void stopLogServer();

    //set/get wheter the log print to file
    void setLogOnFile(bool bFlag);
    inline bool isLogOnFile() const{return m_bIsPrintLogFile;}
    //set/get wheter emit signalPrintLogMsg signal
    void setLogEmitted(bool bFlag);
    inline bool isLogEmitted() const{return m_bIsEmitted;}

    QString getLogDir() const;

    void timeCleanLogs();
    void clearAllLogFile();

signals:
    void signalPrintLogMsg(const QString& strLog); //may be used to print log on UI

private:
    explicit CLogger(QObject *parent = nullptr);

    friend void myPrivateLogOutputCallback(QtMsgType type, const QMessageLogContext &context, const QString &msg);


    void installOrUninstallDebug();

    void createNewLogFile();

    void insertLog(const QPair<QString,QString>& strLog);

    void run();
    void writeLog(const QString& strLogPrefix, const QString& strLogMsg);

private:
    QMutex m_mtx;
    QWaitCondition m_cond;
    QLinkedList<QPair<QString,QString> > m_logCache;
    bool m_bThdRunning;
    std::thread m_thdLog;

    QtMessageHandler m_logCallback;

    QFile m_logFile;
    QString m_strLogDir;
    qint64 m_iFileCurrentSize;

    bool m_bIsPrintLogFile;
    bool m_bIsEmitted;
};

#endif // CLOGGER_H
