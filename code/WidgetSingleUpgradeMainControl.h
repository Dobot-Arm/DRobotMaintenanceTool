#ifndef WIDGETSINGLEUPGRADEMAINCONTROL_H
#define WIDGETSINGLEUPGRADEMAINCONTROL_H

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QMenu>
#include <QSettings>

#include "CommonData.h"
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class WidgetSingleUpgradeMainControl;
}

class WidgetSingleUpgradeMainControl : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit WidgetSingleUpgradeMainControl(QWidget *parent = nullptr);
    ~WidgetSingleUpgradeMainControl();
    void searchMenuItems();
signals:
    void signalUpgradeSingleMainControl(QString upgradeMainControlFilePath);
private:
    Ui::WidgetSingleUpgradeMainControl *ui;
    QMenu* m_singleMenu;
    QString m_mainControlFilePath;
    //0 选择升级文件 1 导入升级文件
    void setUpgradeStatus(int status);

public slots:
    void slot_menuClicked(QAction *action);
protected:
    bool event(QEvent *event);
};

#endif // WIDGETSINGLEUPGRADEMAINCONTROL_H
