#ifndef DOWLOADTIPWIDGET_H
#define DOWLOADTIPWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include "baseUI/BaseWidget.h"
namespace Ui {
class DowloadTipWidget;
}

class DowloadTipWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit DowloadTipWidget(QWidget *parent = nullptr);
    ~DowloadTipWidget();
    void setMessage(QString msg);
public slots:
    void slot_confirm();
private:
    Ui::DowloadTipWidget *ui;
};

#endif // DOWLOADTIPWIDGET_H
