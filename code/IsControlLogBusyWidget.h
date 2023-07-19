#ifndef ISCONTROLLOGBUSYWIDGET_H
#define ISCONTROLLOGBUSYWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
namespace Ui {
class IsControlLogBusyWidget;
}

class IsControlLogBusyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IsControlLogBusyWidget(QWidget *parent = nullptr);
    ~IsControlLogBusyWidget();
    bool event(QEvent *event);
signals:
    void signal_isContinueUpgrade(bool isContinueUpgrade);
public slots:
    void slot_cancel();
    void slot_confirm();
private:
    Ui::IsControlLogBusyWidget *ui;
protected:
    void paintEvent(QPaintEvent *event);

};

#endif // ISCONTROLLOGBUSYWIDGET_H
