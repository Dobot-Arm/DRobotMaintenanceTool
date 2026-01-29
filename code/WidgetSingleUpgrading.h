#ifndef WIDGETSINGLEUPGRADING_H
#define WIDGETSINGLEUPGRADING_H

#include <QWidget>
#include <QMovie>
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class WidgetSingleUpgrading;
}

class WidgetSingleUpgrading : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit WidgetSingleUpgrading(QWidget *parent = nullptr);
    ~WidgetSingleUpgrading();
    void retranslate();
    void setSingleUpgradeStatus(int status,QString errorMsg = QString());
    void show();

private:
    Ui::WidgetSingleUpgrading *ui;
    QMovie* m_pMovie;
protected:
    bool event(QEvent *event);
};

#endif // WIDGETSINGLEUPGRADING_H
