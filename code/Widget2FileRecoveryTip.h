#ifndef WIDGET2FILERECOVERYTIP_H
#define WIDGET2FILERECOVERYTIP_H

#include <QWidget>
#include <QPainter>
#include <QtMath>
#include <QMouseEvent>
namespace Ui {
class Widget2FileRecoveryTip;
}

class Widget2FileRecoveryTip : public QWidget
{
    Q_OBJECT

public:
    explicit Widget2FileRecoveryTip(QWidget *parent = nullptr);
    ~Widget2FileRecoveryTip();
    void setWidgetStatus(bool isFinish,QString msg = QString());

signals:
    void signalsFileRecoveryStart();

private:
    Ui::Widget2FileRecoveryTip *ui;
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

#endif // WIDGET2FILERECOVERYTIP_H
