#ifndef TIPWIDGET_H
#define TIPWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class TipWidget;
}

class TipWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit TipWidget(QWidget *parent = nullptr);
    ~TipWidget();
    void setLabelMessage(QString message);

private:
    Ui::TipWidget *ui;
};

#endif // TIPWIDGET_H
