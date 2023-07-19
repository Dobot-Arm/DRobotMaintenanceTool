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

namespace Ui {
class Upgrade2Widget;
}

class Upgrade2Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Upgrade2Widget(QWidget *parent = nullptr);
    ~Upgrade2Widget();
    void initStatus();
    void setDiskCheckStatus(int ok,QString text = QString());
    void setDiskUpdateStatus(int ok,QString text = QString());
    void hideLabelUpdateDisk();
    void setDownloadFirmwareStatus(int ok,QString text = QString());
    int setFeedbackStatus(int ok,QString text = QString());
    int setIOFirmwareStatus(int ok,QString text = QString());
    void setMainControlStatus(int ok,QString text = QString());
    int setSafeIOStatus(int ok,QString text = QString());
    int handleServoStatus(QList<int> upgradeStatusList);
    int setServoStatus(int ok,QString text = QString());
    int setTerminalStatus(int ok,QString text = QString());
    int setUnIOStatus(int ok,QString text = QString());
    void setIconStatus(QLabel* iconLabel,int ok);
    void setLabelWarningText(QString text);
    void setCabinetType();
    void setRoundBarStatus(int status);
    void setUpgradeStatus(QList<int> intUpgradeStatusList);
    int m_posDiyX;
    int m_posDiyY;
    QList<int> m_intUpgradeStatusList;
    QString m_currentControlVersion;
    bool m_bIsOutage = false;
    int m_singleUpgradeSlaveId = -1;
    void initSingleUpgradeSlaveId(int slaveId);
    bool isCanSingleUpgradeSlave(int slaveId);
//    int m_terminalIOSingleCount;
//    int m_servoSingleCount;
//    int m_safeIOSingleCount;
//    int m_uniIOSingleCount;
//    int m_CCBOXSingleCount;

private:
    Ui::Upgrade2Widget *ui;
    QPoint move_point;
    bool mouse_press;
    BubbleTipsWidget *m_tips;
    QGraphicsDropShadowEffect* shadowEffect;
    bool m_isUprgadeSuccess;
    void sleep(int milliseconds);
    RoundProgressBar* m_roundProgressBar;
public slots:
    void slot_closeWidget();
signals:
    void signal_interruptUpgradeUpgrade2Widget(bool isInterrupt);
    void signal_upgrade2WidgetFinishUpgrade(int status);
    void singal_upgradeSingle2Count(int seqId,int status = 0);
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
