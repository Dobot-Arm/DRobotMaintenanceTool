#ifndef SYSTEMTOOLSNREPAIRWIDGET_H
#define SYSTEMTOOLSNREPAIRWIDGET_H

#include <QWidget>
#include <QEventLoop>
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class SystemToolSNRepairWidget;
}

class SystemToolSNRepairWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit SystemToolSNRepairWidget(QWidget *parent = nullptr);
    ~SystemToolSNRepairWidget();
    bool isClose(){return m_isClose;}
    bool isSNMacthSuccess();
    QString getRobotArmSN();
    QString getControllCabinetSN();

    void exec();

private slots:
    void slotClose();
    void slotWirteABind();
private:
    Ui::SystemToolSNRepairWidget *ui;
    bool m_isClose = false;
    bool m_isSNMacthSuccess = false;
    QString m_robotArmSN;
    QString m_controllCabinetSN;
    QEventLoop m_loopExit;

    void initWidget();
};

#endif // SYSTEMTOOLSNREPAIRWIDGET_H
