#ifndef SERVOPARAMBUBBLETIPWIDGET_H
#define SERVOPARAMBUBBLETIPWIDGET_H

#include <QWidget>
namespace Ui {
class ServoParamBubbleTipWidget;
}

class ServoParamBubbleTipWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServoParamBubbleTipWidget(QWidget *parent = nullptr);
    ~ServoParamBubbleTipWidget();
    void setContent(QString text);
private:
    Ui::ServoParamBubbleTipWidget *ui;
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // SERVOPARAMBUBBLETIPWIDGET_H
