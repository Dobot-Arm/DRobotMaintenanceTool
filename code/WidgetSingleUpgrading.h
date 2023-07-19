#ifndef WIDGETSINGLEUPGRADING_H
#define WIDGETSINGLEUPGRADING_H

#include <QWidget>
#include <QMovie>
namespace Ui {
class WidgetSingleUpgrading;
}

class WidgetSingleUpgrading : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSingleUpgrading(QWidget *parent = nullptr);
    ~WidgetSingleUpgrading();
    void retranslate();
    void setSingleUpgradeStatus(int status,QString errorMsg = QString());

private:
    Ui::WidgetSingleUpgrading *ui;

protected:
    bool event(QEvent *event);
};

#endif // WIDGETSINGLEUPGRADING_H
