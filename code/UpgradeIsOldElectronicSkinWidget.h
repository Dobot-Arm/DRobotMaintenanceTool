#ifndef UPGRADEISOLDELECTRONICSKINWIDGET_H
#define UPGRADEISOLDELECTRONICSKINWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include "baseUI/BaseWidget.h"
namespace Ui {
class UpgradeIsOldElectronicSkinWidget;
}

class UpgradeIsOldElectronicSkinWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit UpgradeIsOldElectronicSkinWidget(QWidget *parent = nullptr);
    ~UpgradeIsOldElectronicSkinWidget();
signals:
    void signal_upgradeIsOldElectronicSkinWidget_toUpgradeDevice();

private:
    Ui::UpgradeIsOldElectronicSkinWidget *ui;

protected:
//    //鼠标按下
//    void mousePressEvent(QMouseEvent *e);
//    //鼠标移动
//    void mouseMoveEvent(QMouseEvent *e);
//    //鼠标释放
//    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);
};

#endif // UPGRADEISOLDELECTRONICSKINWIDGET_H
