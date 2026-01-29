#ifndef UPGRADE2WIDGET_H
#define UPGRADE2WIDGET_H

#include "Define.h"
#include <QWidget>
#include <QMouseEvent>
#include <QStyleOption>
#include <QtMath>
#include <QDesktopWidget>
#include <QLabel>
#include <QtMath>
#include "DobotProtocol.h"
#include "RoundProgressBar.h"
#include "BubbleTipsWidget.h"
#include "CommonData.h"
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class Upgrade2Widget;
}

class Upgrade2Widget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit Upgrade2Widget(QWidget *parent = nullptr);
    ~Upgrade2Widget();

    void setErrorCode(int errorCode);
    void initStatus();
    void setUpgradeStatus(QList<int> intUpgradeStatusList, bool bIsOutage);
    void setDownloadFirmwareStatus(int ok,QString text = QString());
    void setDiskUpdateStatus(int ok,QString text = QString());
    void hideLabelUpdateDisk();
    void setDiskCheckStatus(int ok,QString text = QString());

    int getIdxFWUniIO() const;
    int getIdxFWMainCtrl() const;
    int getIdxFWFeedback() const;;
    int getIdxFWSafeIOB() const;
    int getIdxFWSafeIO() const;;
    int getIdxFWJ1() const;
    int getIdxFWJ2() const;
    int getIdxFWJ3() const;
    int getIdxFWJ4() const;
    int getIdxFWJ5() const;
    int getIdxFWJ6() const;
    int getIdxFWTerminal() const;

    int getIdxXMLUniIO() const;
    int getIdxXMLSafeIO() const;
    int getIdxXMLJ1() const;
    int getIdxXMLJ2() const;
    int getIdxXMLJ3() const;
    int getIdxXMLJ4() const;
    int getIdxXMLJ5() const;
    int getIdxXMLJ6() const;
    int getIdxXMLTerminal() const;
private:
    void setCabinetType();
    int setFeedbackStatus(int ok,QString text = QString());
    int setIOFirmwareStatus(int ok,QString text = QString());
    int setMainControlStatus(int ok,QString text = QString());
    int setSafeIOStatus(int ok,QString text = QString());
    int handleServoStatus(QList<int> upgradeStatusList);
    int setServoStatus(int ok,QString text = QString());
    int setTerminalStatus(int ok,QString text = QString());
    int setUnIOStatus(int ok,QString text = QString());
    void setIconStatus(QLabel* iconLabel,int ok);
    void setLabelWarningText(QString text);
    void setRoundBarStatus(int status);
    void initSingleUpgradeIdx(int idx);
    bool isCanSingleUpgradeIdx(int idx);

private:
    Ui::Upgrade2Widget *ui;
    QPoint move_point;
    bool mouse_press;
    BubbleTipsWidget *m_tips;
    QGraphicsDropShadowEffect* shadowEffect;
    bool m_isUprgadeSuccess;
    void sleep(int milliseconds);
    RoundProgressBar* m_roundProgressBar;
    int m_errorCode;

    int m_posDiyX;
    int m_posDiyY;
    QList<int> m_intUpgradeStatusList;
    QString m_currentControlVersion;
    QHash<int, //slaveid
        int //次数
    > m_checkUpdateFailTimes; //连续检测升级失败的次数
    int m_retryUpdateTimes = 0; //重试升级的次数

    QHash<QString,int> m_hashFW; //fw的索引值
    QHash<QString,int> m_hashXML; //xml的索引值

signals:
    void signal_interruptUpgradeUpgrade2Widget(bool isInterrupt);
    void signal_upgrade2WidgetFinishUpgrade(int status);
    void singal_upgradeSingle2Count();
    void signal_ExportLogWhenError(int iType);
protected slots:
    void slot_closeWidget();
    void onExportLog();
protected:
//    //鼠标按下
//    void mousePressEvent(QMouseEvent *e);
//    //鼠标移动
//    void mouseMoveEvent(QMouseEvent *e);
//    //鼠标释放
//    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent* e);
    bool event(QEvent *event);
};

#endif // UPGRADE2WIDGET_H
