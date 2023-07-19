#ifndef WIDGETV4SN_H
#define WIDGETV4SN_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>
#include <QFileDialog>
namespace Ui {
class WidgetV4SN;
}

class WidgetV4SN : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetV4SN(QWidget *parent = nullptr);
    ~WidgetV4SN();
    QString getMessage();
    QString setControl(QString control);
    void retranslate();
signals:
    void singal_wigdetV4SNToUpgradeDevice(QString controlAndSN,QString V4ExportDir);
private:
    Ui::WidgetV4SN *ui;
    QString m_controlAndSN;
    QString m_v4ExportDir;
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

#endif // WIDGETV4SN_H
