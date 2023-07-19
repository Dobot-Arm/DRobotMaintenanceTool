#include "MainWidget.h"
#include "Common/Logger.h"

#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(true);

    QSharedMemory shared("RobotMaintenanceTool_QSharedMemory_Running");
    if (!shared.create(1))
    {
        printf("the first time to create shared memory fail:%s\n",shared.errorString().toStdString().c_str());
        if (shared.error()==QSharedMemory::AlreadyExists)
        {
            shared.attach();
            shared.detach();
            if (!shared.create(1))
            {
                printf("the second time to create shared memory fail:%s\n",shared.errorString().toStdString().c_str());
                printf("app start fail,because the app is running!!!!!\n");
                QMessageBox::warning(QApplication::desktop(), "", "The program is already running! \r\nPlease do not open it again!");
                return 0;
            }
        }
    }

    CLogger::getInstance()->startLogServer();
    CLogger::getInstance()->setLogOnFile(true);
    qDebug()<<"app starting........";

    MainWidget w;
    w.show();

    int iRet = app.exec();
    qDebug()<<"app finished!!!!";
    CLogger::getInstance()->stopLogServer();

    return iRet;
}
