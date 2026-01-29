#ifndef WIDGET2FILERECOVERYTIP_H
#define WIDGET2FILERECOVERYTIP_H

#include <QWidget>
#include <QPainter>
#include <QtMath>
#include <QMouseEvent>
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class Widget2FileRecoveryTip;
}

class Widget2FileRecoveryTip : public UIBaseWidget
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
};

#endif // WIDGET2FILERECOVERYTIP_H
