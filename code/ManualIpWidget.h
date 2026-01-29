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
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class ManualIpWidget;
}

class ManualIpWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit ManualIpWidget(QWidget *parent = nullptr);
    ~ManualIpWidget();
    void show();

private slots:
    void slot_addIP();
    void slot_saveIPs();
    void slot_delIPAddress(QWidget* w);

signals:
    void signal_saveIpsToMainWindow(QStringList ipList);

private:
    Ui::ManualIpWidget *ui;
    static QList<QStringList> m_allIp;

    int m_ipWndCount = 0; //ip窗口个数
    IPAdressWidget* insertAddAddress();
    int getAddressCount();
    void updateIPAddressIndex();
    void initIP();
     bool checkIpValid(QStringList ipList);
};

#endif // MANUALIPWIDGET_H
