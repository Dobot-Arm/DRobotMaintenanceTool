#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QMouseEvent>
#include <QPoint>
#include <QPaintEvent>
#include <QPainter>
namespace Ui {
class MessageWidget;
}

class MessageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageWidget(QWidget *parent = nullptr);
    ~MessageWidget();
    void setMessage(QString type,QString message);
    bool event(QEvent *event);
public slots:
    void slot_closeWidget();
    void slot_confirmWidget();
private:
    Ui::MessageWidget *ui;
    QPoint move_point;
    bool mouse_press;
protected:
//    //鼠标按下
//    void mousePressEvent(QMouseEvent *e);
//    //鼠标移动
//    void mouseMoveEvent(QMouseEvent *e);
//    //鼠标释放
//    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *event);
};

#endif // MESSAGEWIDGET_H
