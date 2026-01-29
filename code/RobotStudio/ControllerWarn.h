#ifndef CONTROLLERWARN_H
#define CONTROLLERWARN_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class ControllerWarn;
}

class ControllerWarn : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit ControllerWarn(QWidget *parent = nullptr);
    ~ControllerWarn();

private:
    Ui::ControllerWarn *ui;
};

#endif // CONTROLLERWARN_H
