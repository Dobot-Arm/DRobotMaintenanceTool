#ifndef MAINWIDGET2_H
#define MAINWIDGET2_H

#include <QWidget>
#include <QPainter>
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

#include "WidgetSingleUpgrading.h"
#include "WidgetSingleUpgrade.h"
#include "ZipDiyManager.h"
#include "SmallControlAutoConnect.h"
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
#include "Widget2SystemControlBack.h"
#include "WidgetV4SN.h"
#include "Define.h"
#include "Widget2ServoParamsWait.h"
#include "WidgetServoParamsPassword.h"
#include "TableModel.h"
#include "TableDelegate.h"
#include "MyCheckBoxHeader.h"
#include "baseUI/BaseWidget.h"
#include "CommonData.h"
#include "ServoParamAllWriteTipWidget.h"
#include "ServoParamsAllUpgradeWidget.h"
#include "ShadowWindowPng.h"
#include "UpgradeIsOldElectronicSkinWidget.h"
#include "IsControlLogBusyWidget.h"
namespace Ui {
class MainWidget2;
}

class MainWidget2 : public BaseWidget
{
    Q_OBJECT

public:
    explicit MainWidget2(QWidget *parent = nullptr);
    ~MainWidget2();
    bool event(QEvent *event);
    void initUpgradeWidget();
    void initFirmwareCheckWidget();
    void initFirmwareCheckWidgetStatus();
    void initFileRecoveryWidget();
    void initSystemToolWidget();
    void initSingleUpgradeWidget();
    void initFileBackupWidget(bool reset = false);
    void initServoParamsWidget();
    void upgradeAllFirmwareAndXMLs();
    void readUpgradeVersion(QString parentVersion,QString version);
    bool isCurrentVersionEnableUpgrade();
    void exportServoParamsFile(QString exportFileDir);
    void initLanguage();
    void initPlink();
    int upgradeFwAndXmlStatusCheck(int upgradeFWStatus,int upgradeXMLStatus);
    void widget2UpgradeFileCopyToControlFinish(bool ok);
    void Widget2MainControlUpgrade();
    void setHideServo6JWidget(bool isHide);
public slots:
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
    void slot_expandMore(bool checked);
    void slot_onPanelBoxChanged(bool checked);
    void slot_isOnline();
    void slot_btnLanguageChange(bool isZh);
    void slot_getHttpExchange();
    void slot_upgradeProgress();
    void slot_downloadedSoftwareTime(bool ok);
    void slot_downloadAllVersionIniFinished(bool ok);
    void slot_startDiagnose();
    void slot_cSFtpClientFinishedJob(qint64 id,bool bOk, QString strErrMsg);
    void slot_singleFunc();
    void slot_backToUpgrade();
    void slot_upgradeSingleFwOrXML(QString updateFilePath,QString updateFile,QString updateFilePath2=QString(),QString updateFile2=QString());
    void slot_csFtpClientOnChannelFileInfoAvailableFinish(qint64 job, const QList<QSsh::SftpFileInfo> &fileInfoList);
    void slot_upgradeWidgetUpgradeSingle2Count(int seqId,int status);
    int setSingleFuncUpgradeStatus(int status);
private:
    Ui::MainWidget2 *ui;
    DownloadTool* m_upgradeFileZipDownload;
    DownloadTool* m_upgradeAllVersionIniDownload;
    DownloadTool* m_softwareDownload;

    ManualIpWidget* m_manualIpWidget;
    DobotProtocol* m_httpProtocol;
    CopyFolderAFiles* m_importTempVersionHandler;
    ProgressDialog* m_diyProgressDialog;
    ShadowWindow* m_manualIPShadowWindow;
    TipWidget* m_tipWidget;
    MessageWidget* m_messageWidget;

    int m_posDiyX;
    int m_posDiyY;

    QTimer* m_searchDeviceAndIsOnlineTimer;
    QTimer* m_isUpgradeSuccessTimer;
    QTimer* m_upgradeProgressTimer;
    QTimer* m_singleUpgradeTimer;
    QTimer* m_servoReadParamsTimer;
    QTimer* m_servoWriteParamsTimer;

    ServoParamsAllUpgradeWidget* m_servoParamsAllUpgradeWidget;
    Widget2ServoParamsWait* m_widget2ServoParamsWait;
    WidgetServoParamsPassword* m_widgetServoParamsPassword;
    void showServoParamWidget();
    WidgetServoParamsPassword* m_widgetSystemSyncTimePassword;
    WidgetServoParamsPassword* m_widgetSystemLaserCabPassword;
    ServoParamAllWriteTipWidget* m_servoParamAllWriteTipWidget;

    bool m_isPasswordConfirm = false;

    UpgradeIsOldElectronicSkinWidget* m_upgradeIsOldElectronicSkinWidget;

    QStringList m_ipList;
    QString m_currentIp;
    QMenu* m_controlMenu;
    QString m_propertiesCabinetType;

    DobotType::StructCRSingleFwAndXml m_singleCRFwAndXml;
    Widget2SystemControlBack* m_widget2SystemControlBack;
    WidgetV4SN* m_widgetV4SN;

    QString m_V4ExportDir;
    QString m_V4ControlAndSN;
    QString m_strImportDirName;
    QString m_upgradeVersion;
    QString m_upgradeParentVersion;
    QString m_upgradeTime;
    QSettings m_allVersionSetting;
    QSettings m_macAddressSetting;
    QGraphicsDropShadowEffect* shadowEffect;

    Widget2ProgressDialog* m_systemLogExportDialog;
    WidgetSingleUpgrade* m_widgetSingleUpgrade;
    QByteArray m_readAllResult;
    QTranslator* m_Trans;//创建对象
    QPoint move_point;
    bool mouse_press;
    bool isOnline = false;
    int m_currentSoftwareVersion = 0;
    QString m_currentControlVersionSmbDir;
    Upgrade2Widget* m_upgrade2Widget;
    ShadowWindowPng* m_upgrade2WidgetShawdowPng;
    bool m_isInterruptUpgrade = false;

    //判断是否一键升级成功过，如果成功过，进行使用高级功能
    int m_isUpgradeSuccessStatus = 0;
    QString m_backupFileName;

    QListWidgetItem* m_item0;
    WidgetFirmwareCheckItem* m_firmwareCheckItem0;
    QListWidgetItem* m_itemDns;
    WidgetFirmwareCheckItem* m_firmwareCheckItemDns;
    QListWidgetItem* m_itemMacAddress;
    WidgetFirmwareCheckItem* m_firmwareCheckItemMacAddress;

    bool m_isControlBack = false;

    WidgetSingleUpgrading* m_widgetSingleUpgrading;
    CSFtpClient* m_csFtpClient;
    QStringList m_controllServoList;

    QProcess* m_mainControlProcess;
    QString m_A9UpdatePath;
    QProcess* m_V4ControlBackProcess;
    QString m_V4ControlBackFilePath;

    SmallControlAutoConnect* m_controlAutoConnect;

    Widget2ProgressDialog* m_widget2FileBackupProgressDialog;

    WidgetSystemToolTip* m_widgetSystemToolTip;
    WidgetSystemFileCheckTip* m_widgetSystemFileCheckTip;

    WidgetSystemLogExport* m_widgetSystemLogExport;

    ZipDiyManager* m_zipDiyManager;
    QString m_zipLogExportPassword;

    Widget2FileRecoveryTip* m_widget2FileRecoveryTip;
    Widget2ProgressDialog* m_widget2FileRecoveryProgressDialog;

    MyCheckBoxHeader* m_myCheckBoxHeader;
    TableModel* mServoParamsTableModel;
    SingleWriteDelegate* m_singleWriteDelegate;
    SingleReadDelegate* m_singleReadDelegate;
    IsCheckDelegate* m_singleIsCheckDelegate;
    int m_servoParamsCount;
    int m_servoParamsSleep;
    QProcess* m_pFileTransfer;
    QProcess* m_pFileCheck;
    QProcess* m_pPluginLaserCalibrationProcess;

    QProcess* m_pFileSingleTransfer;
    QProcess* m_pFileServoParamsTransfer;
    QString m_strLabelSingleFwAndXML;
    QString m_updateFile1;
    QString m_updateFile2;

    IsControlLogBusyWidget* m_isControlLogBusyWidget;

    QString getCurrentUseAddress();


    QString paramIndexToAddress(QString paramIndex);
    QString paramIndexToJAddress(QString ParaIndStr);
    void setbtnUpgradeServoParamAllWriteHidden(bool isHidden);
    void LangeuageFontChange(bool isEnglish);
    double dotDividedBitsValue(bool isReadMode,int dotDividedBits,double value);
    void delegateServoParamsTable();
    void importServoParamsExcel(QString filePath);
    void widgetSingleFunc();
    void widget2UpgradeDevice();
    void searchDeviceDisable(bool disable);
    void searchMenuItems();
    QString readUpgradeDetail(QString fileParentDir,QString fileName);
    bool isUpgrade(QString currentVersion,QString upgradeVersion);
    void textClear();
    void cabinetTypeInfoShow(PropertiesCabinetType propertiesCabinetType);
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
    void wigdet2UpdateCRSingleFwAndXML(QString updateFile1,QString updateFile2);
    void wigdet2UpdateM1ProSingleFwAndXML(QString strLabelSingleFwAndXML,QString currentLabelFwOrXMLLocation);
    void wigdet2UpdateMG400SingleFwAndXML(QString strLabelSingleFwAndXML,QString currentLabelFwOrXMLLocation);
    void setDiyPos();
protected:
    //鼠标按下
    void mousePressEvent(QMouseEvent *e);
    //鼠标移动
    void mouseMoveEvent(QMouseEvent *e);
    //鼠标释放
    void mouseReleaseEvent(QMouseEvent *e);

};

#endif // MAINWIDGET2_H
