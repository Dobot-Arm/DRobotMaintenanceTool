#ifndef WIDGETSYSTEMTOOLTIP_H
#define WIDGETSYSTEMTOOLTIP_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
namespace Ui {
class WidgetSystemToolTip;
}

class WidgetSystemToolTip : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSystemToolTip(QWidget *parent = nullptr);
    ~WidgetSystemToolTip();
    void setRepairStatus(int status);
    void setMacAddress(QString address);
    void retranslate();
signals:
    void signal_widgetSystemToolWifiIpRevovery();
private:
    Ui::WidgetSystemToolTip *ui;
    QPoint move_point;
    bool mouse_press;
protected:
    //鼠标按下
    void mousePressEvent(QMouseEvent *e);
    //鼠标移动
    void mouseMoveEvent(QMouseEvent *e);
    //鼠标释放
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);
};

#endif // WIDGETSYSTEMTOOLTIP_H
