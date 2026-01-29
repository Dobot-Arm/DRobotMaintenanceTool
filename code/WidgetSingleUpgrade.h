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
#include <QLineEdit>
#include <QVector>
#include "Define.h"
#include "CommonData.h"
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class WidgetSingleUpgrade;
}

class WidgetSingleUpgrade : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit WidgetSingleUpgrade(QWidget *parent = nullptr);
    ~WidgetSingleUpgrade();
    void setSingleUpgradeFwOrXML(QString fwOrXML);
    void retranslate();
    void searchMenuItems();
    void show();

    bool isEnglish;
signals:
    void signal_upgradeSingleFwOrXML(QStringList allUpdateFilePath, QStringList allUpdateFile);
private:
    Ui::WidgetSingleUpgrade *ui;
    QPoint move_point;
    bool mouse_press;
    QMenu* m_singleMenu;
    //0 选择升级文件 1 导入升级文件
    void setUpgradeStatus(int status);

    QString m_singleUpgradeFwOrXML;
    QString m_singleUpgradeFileDir;

    QVector<QLabel*> m_allLabel;
    QVector<QPair<QLineEdit*,QPushButton*>> m_allLine;

protected slots:
    void slot_menuClicked(QAction *action);
    void slot_clickSelectFile(QLineEdit* p);

protected:
    bool event(QEvent *event);
    void showCC262(const QFileInfoList& fileInfoList,const QString upgradeFW,
                   const bool isXML, const QString fileParentDir);
    void showCC162(const QFileInfoList& fileInfoList,
                          const QString upgradeFW, const bool isXML);
    void showCCBox(const QFileInfoList& fileInfoList,
                          const QString upgradeFW, const bool isXML);
    void showE6(const QFileInfoList& fileInfoList,
                const QString upgradeFW, const bool isXML);

    void adjustWidgetSize();
};

#endif // WIDGETSINGLEUPGRADE_H
