#ifndef SERVOPARAMALLWRITETIPWIDGET_H
#define SERVOPARAMALLWRITETIPWIDGET_H

#include <QWidget>
#include <QMovie>
#include "baseUI/BaseWidget.h"
namespace Ui {
class ServoParamAllWriteTipWidget;
}

class ServoParamAllWriteTipWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit ServoParamAllWriteTipWidget(QWidget *parent = nullptr);
    ~ServoParamAllWriteTipWidget();
    void setMessage(bool isNew, QString text);
    void initStatus();

    void setServoParameterPath(QString servoParameterTemplatePath){m_servoParameterTemplatePath=servoParameterTemplatePath;}
    const QString getServoParameterPath()const{return m_servoParameterTemplatePath;}
signals:
    void signal_servoParamsAllWrite();
private:
    Ui::ServoParamAllWriteTipWidget *ui;
    QString m_servoParameterTemplatePath;

protected:
    bool event(QEvent *event);
};

#endif // SERVOPARAMALLWRITETIPWIDGET_H
