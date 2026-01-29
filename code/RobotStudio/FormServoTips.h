#ifndef FORMSERVOTIPS_H
#define FORMSERVOTIPS_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class FormServoTips;
}

class FormServoTips : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FormServoTips(QWidget *parent = nullptr);
    ~FormServoTips();
    void setText(QString str);
    void show();
private:
    Ui::FormServoTips *ui;
};

#endif // FORMSERVOTIPS_H
