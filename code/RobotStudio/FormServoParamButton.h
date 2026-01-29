#ifndef FORMSERVOPARAMBUTTON_H
#define FORMSERVOPARAMBUTTON_H

#include <QWidget>
#include <QEvent>
#include <QShowEvent>
#include <QHideEvent>
#include "baseUI/UIBaseWidget.h"
#include <QMovie>

namespace Ui {
class FormServoParamButton;
}

class FormServoParamButton : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FormServoParamButton(QWidget *parent = nullptr);
    ~FormServoParamButton();

    void showServoParamLabel(bool bChecking, QVariant bNeedUpdate, bool bFail=false,QString strMsgTxt="");

signals:
    void signalCheckServoParam();
    void signalUpdateServoParam();

private:
    bool event(QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::FormServoParamButton *ui;
    QMovie* m_pMovie;
    bool m_bChecking = false;
    bool m_bFail = false;
    QVariant m_bNeedUdate = QVariant();
};

#endif // FORMSERVOPARAMBUTTON_H
