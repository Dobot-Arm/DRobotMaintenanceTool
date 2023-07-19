#ifndef WIDGETSYSTEMFILECHECKTIP_H
#define WIDGETSYSTEMFILECHECKTIP_H

#include <QWidget>
#include <Define.h>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
namespace Ui {
class WidgetSystemFileCheckTip;
}

class WidgetSystemFileCheckTip : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSystemFileCheckTip(QWidget *parent = nullptr);
    ~WidgetSystemFileCheckTip();
    void setMessage(int status,QString errorfiles = QString());
private:
    Ui::WidgetSystemFileCheckTip *ui;
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

#endif // WIDGETSYSTEMFILECHECKTIP_H
