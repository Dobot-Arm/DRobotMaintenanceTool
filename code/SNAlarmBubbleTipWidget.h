#ifndef SNALARMBUBBLETIPWIDGET_H
#define SNALARMBUBBLETIPWIDGET_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class SNAlarmBubbleTipWidget;
}

class SNAlarmBubbleTipWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit SNAlarmBubbleTipWidget(QWidget *parent = nullptr);
    ~SNAlarmBubbleTipWidget();

private:
    Ui::SNAlarmBubbleTipWidget *ui;
};

#endif // SNALARMBUBBLETIPWIDGET_H
