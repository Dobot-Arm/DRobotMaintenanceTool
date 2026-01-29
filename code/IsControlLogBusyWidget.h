#ifndef ISCONTROLLOGBUSYWIDGET_H
#define ISCONTROLLOGBUSYWIDGET_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class IsControlLogBusyWidget;
}

class IsControlLogBusyWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit IsControlLogBusyWidget(QWidget *parent = nullptr);
    ~IsControlLogBusyWidget();
    bool event(QEvent *event);
    void show();

signals:
    void signal_isContinueUpgrade(bool isContinueUpgrade);
public slots:
    void slot_cancel();
    void slot_confirm();
private:
    Ui::IsControlLogBusyWidget *ui;
};

#endif // ISCONTROLLOGBUSYWIDGET_H
