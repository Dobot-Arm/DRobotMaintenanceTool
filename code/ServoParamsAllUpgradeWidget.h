#ifndef SERVOPARAMSALLUPGRADEWIDGET_H
#define SERVOPARAMSALLUPGRADEWIDGET_H

#include <QWidget>
#include <QMovie>
#include <QPainter>
#include <QtMath>
#include "Define.h"
namespace Ui {
class ServoParamsAllUpgradeWidget;
}

class ServoParamsAllUpgradeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServoParamsAllUpgradeWidget(QWidget *parent = nullptr);
    ~ServoParamsAllUpgradeWidget();
    void setStatus(int status,QString msg = QString(),QString errorParams = QString());
    void initStatus();
public slots:
    void slot_close();

protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    Ui::ServoParamsAllUpgradeWidget *ui;
};

#endif // SERVOPARAMSALLUPGRADEWIDGET_H
