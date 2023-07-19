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
int main(int argc, char *argv[])
{

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    CLogger::getInstance()->startLogServer();
    QFont font;
    font.setFamily("Microsoft YaHei UI");

    CLogger::getInstance()->setLogOnFile(true);

    MainWidget2 w;
    w.show();
    QFile qss(":/qss/myqss/mainwidget.qss");
    if (qss.open(QFile::ReadOnly)){
        qApp->setStyleSheet(qss.readAll());
    }

    a.setFont(font);

    a.exec();
    CLogger::getInstance()->stopLogServer();
    return 0;
}
