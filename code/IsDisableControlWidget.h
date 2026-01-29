#ifndef ISDISABLECONTROLWIDGET_H
#define ISDISABLECONTROLWIDGET_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class IsDisableControl;
}

class IsDisableControlWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit IsDisableControlWidget(QWidget *parent = nullptr);
    ~IsDisableControlWidget();
signals:
    void signalDiableControlMode();
    void signalDisconnect();
private:
    Ui::IsDisableControl *ui;
protected:
    bool event(QEvent *event);
};

#endif // ISDISABLECONTROLWIDGET_H
