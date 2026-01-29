#ifndef FORMSERVOWRITEPROGRESS_H
#define FORMSERVOWRITEPROGRESS_H

#include <QWidget>
#include <QResizeEvent>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class FormServoWriteProgress;
}

class FormServoWriteProgress : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FormServoWriteProgress(QWidget *parent = nullptr);
    ~FormServoWriteProgress();

    void showSuccess();
    void showFail(QString strErrMsg="");
    void setUpgrading(int iPercent);

private:
    Ui::FormServoWriteProgress *ui;
};

#endif // FORMSERVOWRITEPROGRESS_H
