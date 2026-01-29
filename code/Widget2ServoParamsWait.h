#ifndef WIDGET2SERVOPARAMSWAIT_H
#define WIDGET2SERVOPARAMSWAIT_H

#include <QWidget>
#include <QMovie>
#include <QPainter>
#include <QtMath>
#include "Define.h"
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class Widget2ServoParamsWait;
}

class Widget2ServoParamsWait : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit Widget2ServoParamsWait(QWidget *parent = nullptr);
    ~Widget2ServoParamsWait();
    void setStatus(int status,QString msg = QString(),QStringList errorParams = QStringList());
    void show();

protected:
    void initStatus();
    void showEvent(QShowEvent *event) override;

private:
    Ui::Widget2ServoParamsWait *ui;
};

#endif // WIDGET2SERVOPARAMSWAIT_H
