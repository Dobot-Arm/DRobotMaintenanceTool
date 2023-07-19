#ifndef SYSTEMTOOLWIDGET_H
#define SYSTEMTOOLWIDGET_H

#include <QWidget>

namespace Ui {
class SystemToolWidget;
}

class SystemToolWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SystemToolWidget(QWidget *parent = nullptr);
    ~SystemToolWidget();

private:
    Ui::SystemToolWidget *ui;
};

#endif // SYSTEMTOOLWIDGET_H
