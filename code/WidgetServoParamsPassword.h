#ifndef WIDGETSERVOPARAMSPASSWORD_H
#define WIDGETSERVOPARAMSPASSWORD_H

#include <QWidget>
#include <QPainter>
#include <QtMath>
#include <QTime>
#include "Define.h"
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class WidgetServoParamsPassword;
}

class WidgetServoParamsPassword : public UIBaseWidget
{
    Q_OBJECT
signals:
    void signal_servoParamsPassword(bool isServoParamsConfirmed);
public:
    explicit WidgetServoParamsPassword(QWidget *parent = nullptr);
    ~WidgetServoParamsPassword();
    void show(){
        raise();
        QWidget::show();
    }
private:
    Ui::WidgetServoParamsPassword *ui;
    int m_currentRandom;
    void initStatus();
    int getRandom();
};

#endif // WIDGETSERVOPARAMSPASSWORD_H
