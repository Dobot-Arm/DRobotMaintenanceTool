#ifndef SYSTEMTOOLSNWRITEWIDGET_H
#define SYSTEMTOOLSNWRITEWIDGET_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"
#include <QEventLoop>
namespace Ui {
class SystemToolSNWriteWidget;
}

class SystemToolSNWriteWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit SystemToolSNWriteWidget(QWidget *parent = nullptr);
    ~SystemToolSNWriteWidget();
    bool isClose(){return m_isClose;}
    bool isSNMacthSuccess(){return m_isSNMacthSuccess;};
    QString getRobotArmSN(){return m_robotArmSN;};
    QString getControllCabinetSN(){return m_controllCabinetSN;};

    void exec();

private slots:
    void slotClose();
    void slotWirteABind();

private:
    void initWidget();
    bool m_isClose = false;
    bool m_isSNMacthSuccess = false;
    QString m_robotArmSN;
    QString m_controllCabinetSN;
    Ui::SystemToolSNWriteWidget *ui;

    QEventLoop m_loopExit;
};

#endif // SYSTEMTOOLSNWRITEWIDGET_H
