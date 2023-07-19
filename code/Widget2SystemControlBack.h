#ifndef WIDGET2SYSTEMCONTROLBACK_H
#define WIDGET2SYSTEMCONTROLBACK_H

#include <QWidget>
#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>
namespace Ui {
class Widget2SystemControlBack;
}

class Widget2SystemControlBack : public QWidget
{
    Q_OBJECT

public:
    explicit Widget2SystemControlBack(QWidget *parent = nullptr);
    ~Widget2SystemControlBack();
    void retranslate();
signals:
    void signal_systemControlBack(QString filePath);

private:
    Ui::Widget2SystemControlBack *ui;
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

#endif // WIDGET2SYSTEMCONTROLBACK_H
