#ifndef WIDGET2SERVOPARAMSWAIT_H
#define WIDGET2SERVOPARAMSWAIT_H

#include <QWidget>
#include <QMovie>
#include <QPainter>
#include <QtMath>
#include "Define.h"
namespace Ui {
class Widget2ServoParamsWait;
}

class Widget2ServoParamsWait : public QWidget
{
    Q_OBJECT

public:
    explicit Widget2ServoParamsWait(QWidget *parent = nullptr);
    ~Widget2ServoParamsWait();
    void setStatus(int status,QString msg = QString(),QString errorParams = QString());
    void initStatus();
public slots:
    void slot_close();

private:
    Ui::Widget2ServoParamsWait *ui;
protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif // WIDGET2SERVOPARAMSWAIT_H
