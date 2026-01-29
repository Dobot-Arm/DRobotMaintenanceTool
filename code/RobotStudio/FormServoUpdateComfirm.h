#ifndef FORMSERVOUPDATECOMFIRM_H
#define FORMSERVOUPDATECOMFIRM_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"
#include <QResizeEvent>

namespace Ui {
class FormServoUpdateComfirm;
}

class FormServoUpdateComfirm : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FormServoUpdateComfirm(QWidget *parent = nullptr);
    ~FormServoUpdateComfirm();
    void setText(QString str);

signals:
    void signalOk();

private:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::FormServoUpdateComfirm *ui;
};

#endif // FORMSERVOUPDATECOMFIRM_H
