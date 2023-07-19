#ifndef MANUALIPWIDGET_H
#define MANUALIPWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QMessageBox>
#include <QDebug>
#include <QListWidget>
#include <QTime>
#include "IPAdressWidget.h"
#include "MessageWidget.h"
#include "baseUI/BaseWidget.h"
namespace Ui {
class ManualIpWidget;
}

class ManualIpWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit ManualIpWidget(QWidget *parent = nullptr);
    ~ManualIpWidget();
    void initIP();
    bool event(QEvent *event);
private:
    Ui::ManualIpWidget *ui;
    QPoint move_point;
    bool mouse_press;
    bool checkIP();
    void sleep(int milliseconds);
    MessageWidget* m_messageWidget;
public slots:
    void slot_addIP();
    void slot_Cancel();
    void slot_saveIPs();
    void slot_closeWidget();
    void slot_checkIPAddressOrder();
    void slot_delIPAddress(QWidget* w);
signals:
    void signal_saveIpsToMainWindow(QStringList ipList);
protected:
//    //鼠标按下
//    void mousePressEvent(QMouseEvent *e);
//    //鼠标移动
//    void mouseMoveEvent(QMouseEvent *e);
//    //鼠标释放
//    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *event);
};

#endif // MANUALIPWIDGET_H
