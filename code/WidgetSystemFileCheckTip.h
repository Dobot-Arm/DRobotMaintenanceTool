#ifndef WIDGETSYSTEMFILECHECKTIP_H
#define WIDGETSYSTEMFILECHECKTIP_H

#include <QWidget>
#include <Define.h>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class WidgetSystemFileCheckTip;
}

class WidgetSystemFileCheckTip : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit WidgetSystemFileCheckTip(QWidget *parent = nullptr);
    ~WidgetSystemFileCheckTip();
    void setMessage(int status,QStringList errorfiles = QStringList());
private:
    Ui::WidgetSystemFileCheckTip *ui;
};

#endif // WIDGETSYSTEMFILECHECKTIP_H
