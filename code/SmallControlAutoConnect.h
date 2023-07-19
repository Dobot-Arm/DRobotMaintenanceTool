#ifndef SMALLCONTROLAUTOCONNECT_H
#define SMALLCONTROLAUTOCONNECT_H

#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QtMath>
#include <QPainter>
#include <QMovie>
namespace Ui {
class SmallControlAutoConnect;
}

class SmallControlAutoConnect : public QWidget
{
    Q_OBJECT

public:
    explicit SmallControlAutoConnect(QWidget *parent = nullptr);
    ~SmallControlAutoConnect();
    void setUpgadeStatus(int status);
    void initStatus();
public slots:
    void slot_close();

private:
    Ui::SmallControlAutoConnect *ui;
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

#endif // SMALLCONTROLAUTOCONNECT_H
