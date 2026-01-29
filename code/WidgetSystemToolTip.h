#ifndef WIDGETSYSTEMTOOLTIP_H
#define WIDGETSYSTEMTOOLTIP_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class WidgetSystemToolTip;
}

class WidgetSystemToolTip : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit WidgetSystemToolTip(QWidget *parent = nullptr);
    ~WidgetSystemToolTip();
    void setRepairStatus(int status);
    void setMacAddress(QString address);
    void show();
signals:
    void signal_widgetSystemToolWifiIpRevovery();
private:
    Ui::WidgetSystemToolTip *ui;
    QString m_strMac;
    bool event(QEvent *event);
};

#endif // WIDGETSYSTEMTOOLTIP_H
