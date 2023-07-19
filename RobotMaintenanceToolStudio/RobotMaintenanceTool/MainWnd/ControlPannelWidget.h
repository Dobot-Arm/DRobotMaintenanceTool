#ifndef CONTROLPANNELWIDGET_H
#define CONTROLPANNELWIDGET_H

#include <QWidget>

namespace Ui {
class ControlPannelWidget;
}

class ControlPannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPannelWidget(QWidget *parent = nullptr);
    ~ControlPannelWidget();

private:
    Ui::ControlPannelWidget *ui;
};

#endif // CONTROLPANNELWIDGET_H
