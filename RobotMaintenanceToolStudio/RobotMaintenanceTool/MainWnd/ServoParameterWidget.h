#ifndef SERVOPARAMETERWIDGET_H
#define SERVOPARAMETERWIDGET_H

#include <QWidget>

namespace Ui {
class ServoParameterWidget;
}

class ServoParameterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServoParameterWidget(QWidget *parent = nullptr);
    ~ServoParameterWidget();

private:
    Ui::ServoParameterWidget *ui;
};

#endif // SERVOPARAMETERWIDGET_H
