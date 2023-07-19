#ifndef WIDGETSERVOPARAMSPASSWORD_H
#define WIDGETSERVOPARAMSPASSWORD_H

#include <QWidget>
#include <QPainter>
#include <QtMath>
#include <QTime>
#include "Define.h"
namespace Ui {
class WidgetServoParamsPassword;
}

class WidgetServoParamsPassword : public QWidget
{
    Q_OBJECT
signals:
    void signal_servoParamsPassword(bool isServoParamsConfirmed);
public:
    explicit WidgetServoParamsPassword(QWidget *parent = nullptr);
    ~WidgetServoParamsPassword();
    void initStatus();
    int getRandom();
    int m_currentRandom;
private:
    Ui::WidgetServoParamsPassword *ui;
protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);

};

#endif // WIDGETSERVOPARAMSPASSWORD_H
