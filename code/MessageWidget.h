#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"
#include <QResizeEvent>

namespace Ui {
class MessageWidget;
}

class MessageWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit MessageWidget(QWidget *parent = nullptr);
    ~MessageWidget();
    void setMessage(QString type,QString message);
    void setBtnText(QString txt);
    void show();

private:
    bool event(QEvent *event) override;

private:
    Ui::MessageWidget *ui;
};

#endif // MESSAGEWIDGET_H
