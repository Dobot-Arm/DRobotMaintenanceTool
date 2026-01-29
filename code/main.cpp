#include "TipWidget.h"
#include "MainWidget2.h"
#include "Upgrade2Widget.h"
#include "SmallControlAutoConnect.h"
#include <QApplication>
#include <QLibrary>
#include <QDebug>
#include <QDesktopWidget>
#include "SFtpClient.h"
#include "ZipDiyManager.h"
#include "CommonData.h"
#include "Logger.h"
#include <windows.h>
#include <QPaintDevice>
#include <QScreen>
#include <QProcess>
#include <private/qhighdpiscaling_p.h>


int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setApplicationVersion(QT_VERSION_STR);

    QApplication a(argc, argv);
    a.setApplicationName("DOBOT Maintenance tool");
    // 除以96之后即可转换成dpi的数值,以2k屏幕为例,Windows默认dpi是125%,这里的值就是:1.25
    QScreen* pScreen = QGuiApplication::primaryScreen();
    double dpi  = pScreen->logicalDotsPerInch() / 96;
    // set scale factor for screen
    if(dpi < 1)
    {
        QHighDpiScaling::setScreenFactor(pScreen,dpi);
        QHighDpiScaling::setGlobalFactor(dpi);
    }

    a.setQuitOnLastWindowClosed(false);
    CLogger::getInstance()->startLogServer();
    QFont font;
    font.setFamily("Microsoft YaHei UI");
    a.setFont(font);
    CLogger::getInstance()->setLogOnFile(true);

    qDebug()<<"app build version:"<<__DATE__<<"  "<<__TIME__;
    qDebug()<<"logicalDotsPerInch="<<pScreen->logicalDotsPerInch()
            <<",devicePixelRatio="<<pScreen->devicePixelRatio()
            <<",dpi="<<dpi;
    qDebug()<<"app started..";
    MainWidget2 w;
    w.setWindowTitle(a.applicationName());
    PublicSetMainWindow(&w);
    w.show();
    QFile qss(":/qss/myqss/mainwidget.qss");
    if (qss.open(QFile::ReadOnly)){
        qApp->setStyleSheet(qss.readAll());
    }

    a.exec();
    CLogger::getInstance()->stopLogServer();
    if (g_bReStartApp)
    {
        QProcess::startDetached(a.applicationFilePath(), a.arguments());
    }
    return 0;
}
