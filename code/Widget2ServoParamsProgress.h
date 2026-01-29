#ifndef WIDGET2SERVOPARAMSPROGRESS_H
#define WIDGET2SERVOPARAMSPROGRESS_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"
#include <QMovie>

namespace Ui {
class Widget2ServoParamsProgress;
}

class Widget2ServoParamsProgress : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit Widget2ServoParamsProgress(QWidget *parent = nullptr);
    ~Widget2ServoParamsProgress();
    void show();

    void loading();
    void setMessage(QString strMsg, bool bOk, QStringList errorParams=QStringList());
private:
    Ui::Widget2ServoParamsProgress *ui;

    QMovie* m_pMovie;
};

#endif // WIDGET2SERVOPARAMSPROGRESS_H
