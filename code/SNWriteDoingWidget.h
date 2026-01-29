#ifndef SNWRITEDOINGWIDGET_H
#define SNWRITEDOINGWIDGET_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"
#include <QEventLoop>
namespace Ui {
class SNWriteDoingWidget;
}

class SNWriteDoingWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit SNWriteDoingWidget(QWidget *parent = nullptr);
    ~SNWriteDoingWidget();
    void setControllCabinetSN(QString controllCabinetSN);
    void setRobotArmSN(QString robotArmSN);
    bool isRobotArmWriteSuccess();
    bool isConrtolWriteSuccess();
    bool isAlarmClearSuccess();

    void exec();
private:
    Ui::SNWriteDoingWidget *ui;
    QString m_controllCabinetSN;
    QString m_robotArmSN;
    bool m_isRobotArmWriteSuccess = false;
    bool m_isConrtolWriteSuccess = false;
    bool m_isAlarmClearSuccess = false;
    QEventLoop m_loopExit;
};

#endif // SNWRITEDOINGWIDGET_H
