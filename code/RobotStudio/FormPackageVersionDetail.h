#ifndef FORMPACKAGEVERSIONDETAIL_H
#define FORMPACKAGEVERSIONDETAIL_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"
#include <QResizeEvent>

namespace Ui {
class FormPackageVersionDetail;
}

class FormPackageVersionDetail : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FormPackageVersionDetail(QString str, QWidget *parent = nullptr);
    ~FormPackageVersionDetail();

private:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::FormPackageVersionDetail *ui;
};

#endif // FORMPACKAGEVERSIONDETAIL_H
