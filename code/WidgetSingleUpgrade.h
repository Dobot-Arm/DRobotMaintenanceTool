#ifndef WIDGETSINGLEUPGRADE_H
#define WIDGETSINGLEUPGRADE_H

#include <QWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "Define.h"
#include "CommonData.h"
namespace Ui {
class WidgetSingleUpgrade;
}

class WidgetSingleUpgrade : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSingleUpgrade(QWidget *parent = nullptr);
    ~WidgetSingleUpgrade();
    void setCabinetType(QString cabinetType);
    void setSingleUpgradeFwOrXML(QString fwOrXML);
    QString m_currentServo;
    void retranslate();
    void searchMenuItems();
    bool isEnglish;
signals:
    void signal_upgradeSingleFwOrXML(QString updateFilePath, QString updateFile,QString updateFilePath2 = QString(),QString updateFile2= QString());
private:
    Ui::WidgetSingleUpgrade *ui;
    QPoint move_point;
    bool mouse_press;
    QMenu* m_singleMenu;
    QSettings m_singleAllVersionSetting;
    //0 选择升级文件 1 导入升级文件
    void setUpgradeStatus(int status);

    QString m_cabinetType;
    QString m_singleUpgradeFwOrXML;
    QString m_mainControlParam2;
    QString m_singleUpgradeFilePath;
    QString m_SCBAFilePath;
    QString m_SCBBFilePath;
    QString m_A9UpdatePath;
public slots:
    void slot_menuClicked(QAction *action);
protected:
    bool event(QEvent *event);
};

#endif // WIDGETSINGLEUPGRADE_H
