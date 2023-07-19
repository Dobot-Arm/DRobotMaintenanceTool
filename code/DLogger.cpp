#include "DLogger.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QTextStream>
#include <QDir>
#include <QMutex>
#include <QFile>
#include <QDebug>
#include <string>


namespace DLogger
{
    static void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &message);
    void initLog()
    {
#ifndef QT_DEBUG
        qInstallMessageHandler(outputMessage);
#endif
    }
    static void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        static QMutex mutex;
        mutex.lock();

        QString text;
        switch (type) {
        case QtDebugMsg:
            text = QString("debug:");
            break;
        case QtWarningMsg:
            text = QString("warning:");
            break;
        }

        QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
        QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString current_date = QString("[%1]").arg(current_date_time);
        QString message = QString("%1 %2 %3 %4").arg(current_date).arg(text).arg(context_info).arg(msg);

        QString filePath = QCoreApplication::applicationDirPath()+"/log/log.txt";

        QFileInfo fileInfo(filePath);
        QDateTime dt = fileInfo.created();//获取文件创建时间
        int days = dt.daysTo(QDateTime::currentDateTime());//获取文件相对于当前时间的天数
        if (days > 7)
        {
            QFile::remove(filePath);//大于7天，移除文件
        }


        qDebug()<<"QCoreApplication::applicationDirPath()+/log/log.txt" <<filePath;
        QFile file(filePath);
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream text_stream(&file);
        text_stream << message << "\r\n";
        file.flush();
        file.close();

        mutex.unlock();
    }


}

