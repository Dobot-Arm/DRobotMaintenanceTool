#ifndef SERVOPARAMSALLUPGRADEWIDGET_H
#define SERVOPARAMSALLUPGRADEWIDGET_H

#include <QWidget>
#include <QMovie>
#include <QPainter>
#include <QtMath>
#include "Define.h"
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class ServoParamsAllUpgradeWidget;
}

class ServoParamsAllUpgradeWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit ServoParamsAllUpgradeWidget(QWidget *parent = nullptr);
    ~ServoParamsAllUpgradeWidget();
    void setStatus(int status,QString msg = QString(),QString errorParams = QString());

public slots:
    void show();

private:
    Ui::ServoParamsAllUpgradeWidget *ui;
    void initStatus();
};

#endif // SERVOPARAMSALLUPGRADEWIDGET_H
