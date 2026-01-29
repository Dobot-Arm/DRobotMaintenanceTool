#ifndef MAINWIDGET2_H
#define MAINWIDGET2_H

#include <QWidget>
#include <QPainter>
#include <QCloseEvent>
#include <QTableWidgetItem>
#include <QtMath>
#include <QMouseEvent>
#include <QMenu>
#include <QStringList>
#include <QDebug>
#include <QProgressDialog>
#include <QTimer>
#include <QLibrary>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QHostInfo>
#include <QProcess>
#include <QtGlobal>
#include <QPainter>
#include <QSettings>
#include <QGraphicsDropShadowEffect>
#include <QDir>
#include <QFile>
#include <QtConcurrent>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QHeaderView>
#include <QScreen>
#include "WidgetSingleUpgrading.h"
#include "WidgetSingleUpgrade.h"
#include "ZipDiyManager.h"
#include "DobotProtocol.h"
#include "DownloadTool.h"
#include "ManualIpWidget.h"
#include "CopyFolderAFiles.h"
#include "ProgressDialog.h"
#include "TipWidget.h"
#include "ShadowWindow.h"
#include "DowloadTipWidget.h"
#include "Logger.h"
#include "Upgrade2Widget.h"
#include "WidgetFirmwareCheckItem.h"
#include "SFtpClient.h"
#include "WidgetSystemToolTip.h"
#include "WidgetSystemFileCheckTip.h"
#include "WidgetSystemLogExport.h"
#include "Widget2ProgressDialog.h"
#include "Widget2FileRecoveryTip.h"
#include "WidgetServoParamsPassword.h"
#include "TableModel.h"
#include "TableDelegate.h"
#include "MyCheckBoxHeader.h"
#include "baseUI/BaseWidget.h"
#include "CommonData.h"
#include "ShadowWindowPng.h"
#include "UpgradeIsOldElectronicSkinWidget.h"
#include "IsControlLogBusyWidget.h"
#include "IsDisableControlWidget.h"
#include "WidgetSingleUpgradeMainControl.h"
#include "ServoParamBubbleTipWidget.h"
#include "SNAlarmBubbleTipWidget.h"
#include "PlinkCmd.h"
#include "UpdateConfigBean.h"
#include "RobotStudio/FormAbout.h"
#include "RobotStudio/FormServoTips.h"
#include "RobotStudio/FormServoWriteProgress.h"
#include "RobotStudio/FormMsgBox.h"
#include "RobotStudio/DlgMsgBox.h"
namespace Ui {
class MainWidget2;
}

class MainWidget2 : public BaseWidget
{
    Q_OBJECT

public:
    explicit MainWidget2(QWidget *parent = nullptr);
    ~MainWidget2();
    DobotProtocol* GetDobotProtocol(){return m_httpProtocol;}
    bool event(QEvent *event);
    void initUpgradeWidget();
    void initFirmwareCheckWidget();
    void initFirmwareCheckWidgetStatus();
    void initFileRecoveryWidget();
    void initSystemToolWidget();
    void initSystemToolDectect();
    void initSingleUpgradeWidget();
    void showSingleUpgradeWidget(QString fwOrXML);
    void initBackupToolWidget(bool reset = false);
    void initServoParamsWidget();
    void upgradeAllFirmwareAndXMLs();
    void readUpgradeVersionOld(QString parentVersion,QString version);
    void readUpgradeVersion(QString parentVersion,QString version);
    bool isCurrentVersionEnableUpgrade();
    void exportServoParamsFile(QString exportFileDir);
    void initLanguage();
    int upgradeFwAndXmlStatusCheck(int upgradeFWStatus,int upgradeXMLStatus);
    void widget2UpgradeFileCopyToControlFinish(bool ok);
    void Widget2MainControlUpgrade();
    void setHideServo6JWidget(bool isHide);
public slots:
    void slotCheckServoParam();
    void slotClickVersionText();
    void slot_btnAboutMe();
    void slot_btnLogExport();
    void slot_btnSystemMacView();
    void slot_btnLookXmlVersion();
    void slot_ExportLogWhenError(int iType);
    void slot_menuClicked(QAction *action);
    void slot_connectClicked();
    void slot_searchDevice();
    void sleep(int milliseconds);
    void slot_openManuallyIPWidget();
    void slot_saveIpsFromManual(QStringList ipList);
    void slot_download();
    void slot_downloadProgress(qint64 bytesRead, qint64 totalBytes, qreal progress);
    void slot_downloadFinished(bool ok);
    void slot_importTempVersion();
    void slot_importFinishedTempVersion(bool ok);
    void slot_upgradeDevice();
    void slot_importTempVersionProgress(int progress,int total);
    void slot_closeWindow();
    void slot_miniWindow();
    void slot_maxWindow(bool ok);
    void slot_onPanelBoxChanged(bool checked);
    void slot_isOnline();
    void slot_btnLanguageChange(bool isZh);
    void slot_upgradeProgress();
    void slot_downloadedSoftwareTime(bool ok);
    void slot_downloadAllVersionIniFinished(bool ok);
    void slot_downloadUpdateConfigJsonFinished(bool ok);
    void slot_downloadUpdatefirmLuaFinished(bool ok);
    void slot_downloadStartserverToolFinished(bool ok);
    void slot_downloadServoJointTableFinished(bool ok);
    void slot_downloadControllerScriptFinished(bool ok);
    void slot_startDiagnose();
    void slot_cSFtpClientFinishedJob(qint64 id, const QList<QSsh::SftpFileInfo> &fileInfoList,bool bOk, QString strErrMsg);
    void slot_singleFunc();
    void slot_backToUpgrade();
    void slot_upgradeSingleFwOrXML(QStringList allUpdateFilePath, QStringList allUpdateFile);
    void slot_csFtpClientOnChannelFileInfoAvailableFinish(qint64 job, const QList<QSsh::SftpFileInfo> &fileInfoList);
    void slot_upgradeWidgetUpgradeSingle2Count();
    int setSingleFuncUpgradeStatus(int status);
    void slot_SystemLogExport(QString projectName,QString dstExportDir);
    void slot_DownloadRecovery();
    void slot_RecoveryFactory();

    bool uncompressStartserverToolZip();
    bool isCanUseSN();
    void slotSNAlarmed(bool isAlarm);
    void slotFromTitleBarToSN(bool isSNEmpty);
    void slotSystemSNWrite(bool bNeedCheckValid=true);
    void slotSystemSNRepair();
    void writeE6SN();
    void writeCC162OrCCBOXSN();
    void writeCC26XSN();

    bool servoParamsAllWrite(QString arrJointCSVFile[6]);

private:
    Ui::MainWidget2 *ui;
    DownloadTool* m_upgradeFileZipDownload;
    DownloadTool* m_upgradeAllVersionIniDownload;
    DownloadTool* m_softwareDownload;
    DownloadTool* m_updateConfigJsonDownload;

    DownloadTool* m_downloadUpdatefirmLua = nullptr;
    DownloadTool* m_downloadStarserverTool = nullptr;
    DownloadTool* m_downloadServoJointTable = nullptr;

    DownloadTool* m_downloadControllerScript = nullptr;

    UpdateConfigObject m_localUpdateConfig;
    UpdateConfigObject m_remoteUpdateConfig;

    DobotProtocol* m_httpProtocol;
    CopyFolderAFiles* m_importTempVersionHandler;
    ProgressDialog* m_diyProgressDialog;

    QTimer* m_searchDeviceAndIsOnlineTimer;
    QTimer* m_upgradeProgressTimer;
    QTimer* m_singleUpgradeTimer;

    void showServoParamWidget();

    bool m_isPasswordConfirm = false;

    bool mouse_press=false;
    QStringList m_ipList;
    QMenu* m_pMenu = nullptr;

    DobotType::StructCRSingleFwAndXml m_singleCRFwAndXml;
    QList<int> m_jallServoId;

    QString m_V4ExportDir;
    QString m_V4ControlAndSN;
    QString m_strImportDirName;
    QString m_upgradeVersion;
    QString m_upgradeParentVersion;
    QSettings* m_pAllVersionSetting;
    QSettings m_macAddressSetting;
    QGraphicsDropShadowEffect* shadowEffect;

    WidgetSingleUpgrade* m_widgetSingleUpgrade;
    QByteArray m_readAllResult;
    QTranslator* m_Trans;//创建对象
    QPoint move_point;
    bool isOnline = false;
    int m_currentSoftwareVersion = 0;
    Upgrade2Widget* m_upgrade2Widget;
    bool m_isInterruptUpgrade = false;
    QString m_strServoParameterPath;

    //判断是否一键升级成功过，如果成功过，进行使用高级功能
    int m_isUpgradeSuccessStatus = 0;
    QString m_backupFileName;

    QListWidgetItem* m_item0;
    WidgetFirmwareCheckItem* m_firmwareCheckItem0;
    QListWidgetItem* m_itemDns;
    WidgetFirmwareCheckItem* m_firmwareCheckItemDns;
    QListWidgetItem* m_itemMacAddress;
    WidgetFirmwareCheckItem* m_firmwareCheckItemMacAddress;

    SNAlarmBubbleTipWidget* m_pSNAlarmBubbleTipWidget = nullptr;

    WidgetSingleUpgrading* m_widgetSingleUpgrading;
    CSFtpClient* m_csFtpClient;
    QStringList m_controllServoList;

    WidgetSingleUpgradeMainControl* m_widgetSingleUpgradeMainControl;
    QString m_singleUpgradeMainControl;

    QString m_A9UpdatePath;
    QString m_V4ControlBackFilePath;
    QList<QPair<QString, //name
        QString> //mac
    > m_allNetworkCardInfo;

    WidgetSystemToolTip* m_widgetSystemToolTip;
    FormAbout* m_pFormAbout = nullptr;

    ZipDiyManager* m_zipDiyManager;
    QString m_zipLogExportPassword;

    MyCheckBoxHeader* m_myCheckBoxHeader;
    TableModel* mServoParamsTableModel;
    ReadAWriteBtnDelegate* m_singleReadAWriteDelegate;
    IsCheckDelegate* m_singleIsCheckDelegate;
    int m_servoParamsCount;
    int m_servoParamsSleep;

    QString m_strProtocolVersionSlaveFiles;
    QString m_strProtocolVersionControl;
    QString m_strProtocolVersionKernel;
    QString m_strProtocolVersionV4RestoreBackup;
    PlinkCmd* m_pFileTransfer;

    QProcess* m_pFileSingleTransfer;
    QString m_strLabelSingleFwAndXML;
    QStringList m_allUpdateFile;

    IsControlLogBusyWidget* m_isControlLogBusyWidget;
    IsDisableControlWidget* m_isDisableControlWidget;

    QString getCurrentUseAddress();

    int m_currentPasswordRandom;
    QString m_strCurrentIP;
    bool m_bIsCheckingServoParam = false;

    QString paramIndex2Address(QString ParaIndStr);
    QString address2ParamIndex(QString address);
    void showUpgradeServoParamAllWriteUI(bool show);
    void LangeuageFontChange(bool isEnglish);
    double dotDividedBitsValue(bool isReadMode,int dotDividedBits,double value);
    void delegateServoParamsTable();
    void importServoParamsExcel(QString filePath);
    void widgetSingleFunc();
    void showFirmwareAdvanceUI();
    void widget2UpgradeDevice();
    void searchDeviceDisable(bool disable);
    void searchMenuItems();
    void textClear();
    void cabinetTypeInfoShow();
    void showCurrentAndUpgradeFirmware(QLabel* labelTheme, QLabel* labelCurrent,QLabel* labelUpgrade,bool isShow);

    void setStackedWidgetDisable(bool isDisabled,bool is02ServerStarted);
    bool m_bIsStackedWidgetDisable = false;

    void setLabelToolTip(QLabel* label,QString text);
    void systemToolSyncTime();
    void widget2UpgradeFirmwareDowloadFinish(bool ok);
    void widget2StartServer();
    void widget2StartServerFinish(bool ok);
    void widget2UpdateDiskKernel();
    void wigdet2ControllCompatibleUpdateDiskKernel();
    void wigdet2UpdateCRSingleFwAndXML(QStringList allUpdateFile);
#if 0
    void wigdet2UpdateM1ProSingleFwAndXML(QString strLabelSingleFwAndXML,QString currentLabelFwOrXMLLocation);
    void wigdet2UpdateMG400SingleFwAndXML(QString strLabelSingleFwAndXML,QString currentLabelFwOrXMLLocation);
#endif
    void setDiyPos();
    void disableWidgetWhenConnected();

    void widget2FileRecoveryTip(const QString& strSelectedBackupFileName);

    int stringVersionToIntVersion(QString version);
    void readToolParamsToErrorCodes();

    void changeMenuButtonText(QString strText, bool bHasPackage=false);
    void showServoParamLabel(bool bHasPack);
    bool checkSveroParameters(bool& bNeedUpdate, QList<int>& bJointNotEqual, QString arrJointCSVFile[6], const bool bShowTips=false);

    QHash<QString,QString> m_servoJointTable;
    void readServoJointTable(); //读取伺服关节电机厂家配置对照表
    QPair<bool,QString> getServoJointTable(int iChangJia, int iDianJiXingHao); //厂家，电机型号

    /*有一种奇怪的设备，升级到一定程度比如94%后，自动掉电了，也没有任何提示，控制器重启后也无法恢复http服务。所以为了弥补这种缺陷，
    大家一致决定，当/update/CR/status这个接口连续15s内请求都失败，则认为掉电，提示用户重启，并关闭升级的窗口*/
    struct ExceptionOutage{
        bool bStart = false; //开始计时
        QElapsedTimer timer;
        constexpr static int deltaTime = 20*1000;//20s
    };
    ExceptionOutage m_exceptionOutage;
    QString m_strServoFuncCanUseTips; //伺服参数按钮不能点击时的提示语

    bool m_bGetUpdateCRStatus = false;
protected:
    //鼠标按下
    void mousePressEvent(QMouseEvent *e);
    //鼠标移动
    void mouseMoveEvent(QMouseEvent *e);
    //鼠标释放
    void mouseReleaseEvent(QMouseEvent *e);
    void closeEvent(QCloseEvent* e) override;

};

#endif // MAINWIDGET2_H
