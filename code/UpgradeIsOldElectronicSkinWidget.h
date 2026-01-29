#ifndef UPGRADEISOLDELECTRONICSKINWIDGET_H
#define UPGRADEISOLDELECTRONICSKINWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class UpgradeIsOldElectronicSkinWidget;
}

class UpgradeIsOldElectronicSkinWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit UpgradeIsOldElectronicSkinWidget(QWidget *parent = nullptr);
    ~UpgradeIsOldElectronicSkinWidget();

signals:
    void signal_toUpgradeDevice();

private:
    Ui::UpgradeIsOldElectronicSkinWidget *ui;
};

#endif // UPGRADEISOLDELECTRONICSKINWIDGET_H
