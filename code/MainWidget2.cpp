#include "MainWidget2.h"
#include "ui_MainWidget2.h"
#include "LoadingUI.h"
#include "Define.h"
#include <QLocale>
#include "LuaApi.h"
#include "md5.h"
#include "LuaDebugForm.h"
#include "EnvDebugConfig.h"
#include "RobotStudio/WidgetToast.h"
#include "RobotStudio/ControllerWarn.h"
#include "RobotStudio/FormLookXmlVersion.h"
#include "RobotStudio/FormMacDetail.h"
#include "RobotStudio/QMenuIconStyle.h"
#include "RobotStudio/FormPackageVersionDetail.h"
#include "RobotStudio/FormServoUpdateComfirm.h"
#include "Widget2ServoParamsProgress.h"
typedef long (*ZWZipCompress)(const char* lpszSourceFiles, const char* lpszDestFile, bool bUtf8);
typedef long (*ZWZipExtract)(const char* lpszSourceFiles, const char* lpszDestFile);

#include "FileBackupWidget.h"
#include "FileRecoveryWidget.h"
MainWidget2::MainWidget2(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::MainWidget2)
  , m_macAddressSetting(QCoreApplication::applicationDirPath()+"/config/macAddress.ini",QSettings::IniFormat)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,true);

    m_pAllVersionSetting = new QSettings(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+g_strAllVersion+".cfg",QSettings::IniFormat);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);//无边框，置顶
    m_Trans = new QTranslator(this);

    //不同的环境请求地址不一样
    ui->widgetPlaceHolder->hide();
    if (EnvDebugConfig::isDebugEnv())
    {
        g_strHttpDobotPrefix = g_strHttpDobotPrefixDebug;
        ui->btnLookXmlVersion->show();
        ui->widgetServoParams->show();
        ui->widgetSystemSN->show();
        ui->widgetSpecialSN->show();
        ui->widgetSystemLaserCab->show();
        ui->widgetRecoveryFactory->show();
        ui->labelTitle->show();
    }
    else
    {
        g_strHttpDobotPrefix = g_strHttpDobotPrefixRealse;
        ui->btnLookXmlVersion->hide();
        ui->widgetServoParams->hide();
        ui->widgetSystemSN->hide();
        ui->widgetSpecialSN->hide();
        ui->widgetSystemLaserCab->hide();
        ui->widgetRecoveryFactory->hide();
        ui->labelTitle->hide();
    }

    uncompressStartserverToolZip();

    connect(ui->btnLookXmlVersion, &QPushButton::clicked, this, &MainWidget2::slot_btnLookXmlVersion);

    ui->widgetFirmwareCheck->hide();
    //ui->widgetSystemTool->hide();
    readToolParamsToErrorCodes();
    connect(ui->btnLanguage,&QPushButton::clicked,this,&MainWidget2::slot_btnLanguageChange);
    m_httpProtocol = new DobotProtocol();
    m_zipLogExportPassword = "dobot123";
    m_zipDiyManager = new ZipDiyManager();
    m_csFtpClient = new CSFtpClient();
    connect(m_csFtpClient,&CSFtpClient::signalConnectError,this,[&](QString strErr){
        qDebug()<<"  signalConnectError -------- "<<strErr;
    });

    m_pFileTransfer = new PlinkCmd();
    connect(m_pFileTransfer,&PlinkCmd::signalFinishedCmd,[this](){
        qDebug()<<"pFileTransfer signalFinishedCmd:isInterruptUpgrade="<<m_isInterruptUpgrade;
        if(m_isInterruptUpgrade)
        {
            return;
        }
        QStringList projectList = m_pFileTransfer->property("projectList").toStringList();
        QString projectListDest = m_pFileTransfer->property("projectListDest").toString();

        QStringList mainControlFiles = m_pFileTransfer->property("mainControlFiles").toStringList();
        QString mainControlFilesDest = m_pFileTransfer->property("mainControlFilesDest").toString();

        QStringList kernelFiles = m_pFileTransfer->property("kernelFiles").toStringList();
        QString kernelFilesDest = m_pFileTransfer->property("kernelFilesDest").toString();

        QList<QPair<QStringList,QString>> allFiles;
        allFiles.append(QPair<QStringList,QString>(projectList,projectListDest));
        allFiles.append(QPair<QStringList,QString>(mainControlFiles,mainControlFilesDest));
        allFiles.append(QPair<QStringList,QString>(kernelFiles,kernelFilesDest));
        m_pFileTransfer->execUploadFile(allFiles);
    });
    connect(m_pFileTransfer,&PlinkCmd::signalFinishedUploadFile,[this](){
        qDebug()<<"pFileTransfer signalFinishedUploadFile:isInterruptUpgrade="<<m_isInterruptUpgrade;
        if(m_isInterruptUpgrade)
        {
            return;
        }
        qDebug()<<" widget2UpgradeFileCopyToControlFinish not 3541 351  ";
        widget2UpgradeFileCopyToControlFinish(true);
    });

    m_pFileSingleTransfer = new QProcess();
    connect(m_pFileSingleTransfer,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<"m_pFileSingleTransfer,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished)  ";
        qDebug()<<"allUpdateFile  "<<m_allUpdateFile;
        wigdet2UpdateCRSingleFwAndXML(m_allUpdateFile);
    });

    m_widgetSingleUpgrade = new WidgetSingleUpgrade(this);
    m_widgetSingleUpgrade->setGeometry(0,0,width(),height());
    m_widgetSingleUpgrade->hide();
    connect(m_widgetSingleUpgrade,&WidgetSingleUpgrade::signal_upgradeSingleFwOrXML,this,&MainWidget2::slot_upgradeSingleFwOrXML);
    connect(m_csFtpClient,&CSFtpClient::signalOnChannelFileInfoAvailableFinish,this,&MainWidget2::slot_csFtpClientOnChannelFileInfoAvailableFinish);
    connect(m_csFtpClient,&CSFtpClient::signalFinishedJob,this,&MainWidget2::slot_cSFtpClientFinishedJob);

    connect(ui->btnClose,&QPushButton::clicked,this,&MainWidget2::slot_closeWindow);
    connect(ui->btnMini,&QPushButton::clicked,this,&MainWidget2::slot_miniWindow);
    //connect(ui->btnMax,&QPushButton::clicked,this,&MainWidget2::slot_maxWindow); //去掉最大化按钮
    ui->btnMax->hide();

    connect(ui->btnBackToUpgrade,&QPushButton::clicked,this,&MainWidget2::slot_backToUpgrade);
    connect(ui->btnSingleFunc,&QPushButton::clicked,this,&MainWidget2::slot_singleFunc);

    searchDeviceDisable(true);

    QSettings softwareSetting("config/softWareInfo.ini",QSettings::IniFormat);
    if(!softwareSetting.value("software/version").toString().isEmpty()){
        QStringList currentSoftwareVersions= softwareSetting.value("software/version").toString().split(".");
        QString currentSoftwareVersion;
        for(QString softwareVersion:currentSoftwareVersions){
            currentSoftwareVersion+=softwareVersion;
        }
        m_currentSoftwareVersion = currentSoftwareVersion.toInt();
        qDebug()<<"m_currentSoftwareTime  "<<m_currentSoftwareVersion;
    }
#ifndef V136_NOVA_UPDATE
    m_softwareDownload = new DownloadTool();
    connect(m_softwareDownload,&DownloadTool::sigDownloadFinished,this,&MainWidget2::slot_downloadedSoftwareTime);
    m_softwareDownload->startDownload(g_strHttpDobotPrefix+"/softWareInfo.ini"
                            ,QCoreApplication::applicationDirPath()+"/config");
#endif

    connect(ui->btnFirmwareUpgrade,&QPushButton::clicked,this,&MainWidget2::slot_onPanelBoxChanged);
    connect(ui->btnFirmwareCheck,&QPushButton::clicked,this,&MainWidget2::slot_onPanelBoxChanged);
    connect(ui->btnSystemTool,&QPushButton::clicked,this,&MainWidget2::slot_onPanelBoxChanged);
    connect(ui->btnBackup,&QPushButton::clicked,this,&MainWidget2::slot_onPanelBoxChanged);
    connect(ui->btnServoParams,&QPushButton::clicked,this,&MainWidget2::slot_onPanelBoxChanged);

    initUpgradeWidget();
    initSingleUpgradeWidget();
    //故障排查
    connect(ui->btnDiagnose,&QPushButton::clicked,this,&MainWidget2::slot_startDiagnose,Qt::UniqueConnection);
    connect(ui->btnRestartDiagnose,&QPushButton::clicked,this,[&]{
        ui->btnAllRepair->hide();
        slot_startDiagnose();
    },Qt::UniqueConnection);
    connect(ui->btnAllRepair,&QPushButton::clicked,this,[&]{
        if(m_firmwareCheckItemDns->theme().contains("异常")){
            m_firmwareCheckItemDns->setLabelStatus(1);
            QString result = m_httpProtocol->postFaultRepair(g_strCurrentIP,"dns");
            if(result =="success"){
                m_firmwareCheckItemDns->setLabelStatus(2);
            }else{
                m_firmwareCheckItemDns->setLabelStatus(-1);
            }
        }
        if(m_firmwareCheckItemMacAddress->theme().contains("异常")){
            m_firmwareCheckItemMacAddress->setLabelStatus(1);
            if(m_httpProtocol->postFaultRepair(g_strCurrentIP,"mac",getCurrentUseAddress())=="success"){
                m_firmwareCheckItemMacAddress->setLabelStatus(2);
            }else{
                m_firmwareCheckItemMacAddress->setLabelStatus(-1);
            }
        }
    },Qt::UniqueConnection);
    initFirmwareCheckWidget();

    initSystemToolWidget();
    initBackupToolWidget();

    connect(ui->btnFileRevoverySelectDir,&QPushButton::clicked,this,[&]{
        QString fileName = QFileDialog::getOpenFileName(nullptr,QString(),
                                                        "/home",
                                                        tr("Zip (*.zip)"));
        if(!fileName.isEmpty()){
            ui->lineFileRecoveryDir->setText(fileName);
        }
        ui->lineFileRecoveryDir->repaint();
        return;
    });

    //ui->btnAboutMe->setDisabled(true);
    connect(ui->btnAboutMe,&QPushButton::clicked,this,[&]{
        slot_btnAboutMe();
    });
    connect(ui->btnSystemSNWrite, &QPushButton::clicked, this, [this]{
        if(m_isPasswordConfirm || EnvDebugConfig::isDebugEnv())
        {
            slotSystemSNWrite();
        }
        else
        {
            auto ptr = new WidgetServoParamsPassword(this);
            connect(ptr,&WidgetServoParamsPassword::signal_servoParamsPassword,this,[this,ptr]{
                m_isPasswordConfirm = true;
                slotSystemSNWrite();
                ptr->deleteLater();
            });
            ptr->setGeometry(0,0,width(),height());
            ptr->show();
        }
    });
    connect(ui->btnSystemSNRepair, &QPushButton::clicked, this, [this]{
        if(m_isPasswordConfirm || EnvDebugConfig::isDebugEnv())
        {
            slotSystemSNRepair();
        }
        else
        {
            auto ptr = new WidgetServoParamsPassword(this);
            connect(ptr,&WidgetServoParamsPassword::signal_servoParamsPassword,this,[this,ptr]{
                m_isPasswordConfirm = true;
                slotSystemSNRepair();
                ptr->deleteLater();
            });
            ptr->setGeometry(0,0,width(),height());
            ptr->show();
        }
    });
    connect(ui->btnSpecialSNWrite, &QPushButton::clicked, this, [this]{
        if(m_isPasswordConfirm || EnvDebugConfig::isDebugEnv())
        {
            slotSystemSNWrite(false);
        }
        else
        {
            auto ptr = new WidgetServoParamsPassword(this);
            connect(ptr,&WidgetServoParamsPassword::signal_servoParamsPassword,this,[this,ptr]{
                m_isPasswordConfirm = true;
                slotSystemSNWrite(false);
                ptr->deleteLater();
            });
            ptr->setGeometry(0,0,width(),height());
            ptr->show();
        }
    });

    // 2、设置阴影边框;
    shadowEffect = new QGraphicsDropShadowEffect(this);
    // 阴影偏移
    shadowEffect->setOffset(0, 0);
    // 阴影颜色;
    shadowEffect->setColor(QColor(0, 0, 0, 60));
    // 阴影半径;
    shadowEffect->setBlurRadius(18);
    // 给窗口设置上当前的阴影效果;
    //    this->setGraphicsEffect(shadowEffect);
    //    this->setAttribute(Qt::WA_TranslucentBackground);
    //    this->setContentsMargins(6,6,3,3);

    initLanguage();
    initServoParamsWidget();
    initSystemToolDectect();

    setDiyPos();
    ui->btnFirmwareUpgrade->clicked(true);
    setStackedWidgetDisable(true,false);
    connect(ui->btnLogExport, &QPushButton::clicked, this, &MainWidget2::slot_btnLogExport);

    textClear();
    connect(ui->btnVersionText, &QPushButton::clicked, this, &MainWidget2::slotClickVersionText);
}

MainWidget2::~MainWidget2()
{
    delete ui;
}

void MainWidget2::searchDeviceDisable(bool disable){
    ui->btnConnect->setDisabled(disable);
    ui->btnDownload->setDisabled(disable);
    ui->btnImport->setDisabled(disable);
    ui->btnMenu->setDisabled(disable);
    ui->btnUpgrade->setDisabled(disable);
    ui->labelDeviceIcon->setDisabled(disable);
}

void MainWidget2::searchMenuItems()
{
    if (!m_pMenu)
    {
        m_pMenu = new QMenu(this);
        m_pMenu->setStyle(new QMenuIconStyle());
        m_pMenu->setWindowFlag(Qt::NoDropShadowWindowHint);
        QString strMenuQss = "QMenu{"
                "background-color: #FAFDFF;"
                "font-size: 14px;"
                "font-weight: 350;"
                "color: rgba(45, 52, 64, 1);"
                "border: 0.5px solid #DFE3E5;"
                "border-radius: 4px;}"
            "QMenu::item {"
                "background-color: transparent;"
                "height:44px;"
                "padding-left: 16px;}"
            "QMenu::item:selected {"
                "background-color: rgba(0, 71, 187, 0.1); "
                "color: #0047BB;}";
        m_pMenu->setStyleSheet(strMenuQss);
        connect(m_pMenu,&QMenu::triggered,this,&MainWidget2::slot_menuClicked);
        ui->btnMenu->setMenu(m_pMenu);
        ui->btnMenu->setOnEventBeforeShowMenu([this]{
            if (this->m_bIsCheckingServoParam)
            {
                WidgetToast::warn(tr("请等待伺服参数对比完成"), 2000);
                return false;
            }
            return true;
        });
    }
    m_pMenu->clear();
    QString strTest="\r\nQMenu::item {padding-left:4px;}";
    QString strQss = m_pMenu->styleSheet();
    if (!strQss.contains(strTest)){
        strQss += strTest;
    }
    /******************************************************************************/
    QHash<QString,QStringList> versions;

    QDir dir(QCoreApplication::applicationDirPath()+"/upgradeFiles");
    if(!dir.exists()){
        dir.mkpath(dir.absolutePath());
    }
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QFileInfo fileInfo, fileInfoList) {
        QDir sonDir(dir.filePath(fileInfo.fileName()));
        QStringList sonVersions;
        QFileInfoList sonFileInfoList = sonDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach(QFileInfo sonFileInfo, sonFileInfoList){
            sonVersions.append(sonFileInfo.fileName());
        }
        versions.insert(fileInfo.fileName(),sonVersions);
    }

    auto allChildrenGroups = m_pAllVersionSetting->childGroups();
    for(QString group:allChildrenGroups){
        QDir groupDir(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+group);
        QFileInfoList fileInfoList = groupDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
        auto bShowMainMenu = CLuaApi::getInstance()->isShowSearchMenuItems(group,fileInfoList);
        if (bShowMainMenu.hasValue())
        {
            if (!bShowMainMenu.value()) continue;
        }
        else
        {
            //一代大小型控制柜不显示V4升级包
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)
                    ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
            {
                if(group.at(0)=='4') continue;
            }
            else //二代柜 E6 CR20A 不显示V3升级包
            {
                if(group.at(0)=='3') continue;
            }
        }

        qDebug()<<"CommonData::getStrPropertiesCabinetType().name ---> "<<CommonData::getStrPropertiesCabinetType().name;
        QMenu* currentBigVersion = m_pMenu->addMenu(group);
        currentBigVersion->setStyleSheet(strQss);
        currentBigVersion->setWindowFlag(Qt::NoDropShadowWindowHint);
        QStringList existVersions = versions.value(group);
        m_pAllVersionSetting->beginGroup(group);
        QStringList keyVersions = m_pAllVersionSetting->allKeys();
        m_pAllVersionSetting->endGroup();
        const QString strCtrlType = CommonData::getControllerType().name;
        for(QString version:keyVersions){
            auto bShowSubMenu = CLuaApi::getInstance()->isShowSearchSubMenuItems(version);
            if (bShowSubMenu.hasValue())
            {
                if (!bShowSubMenu.value()) continue;
            }
            else
            {
                if (version.contains("4.5.3.0") && !CommonData::getCurrentSettingsVersion().servo1.isEmpty())
                {
                    if (CommonData::getCurrentSettingsVersion().servo1.at(0)=='7')
                    {
                        goto labelShow;
                    }
                    else
                    {
                        continue;
                    }
                }
                if (version.contains("4.5.2.0"))
                {
                    if (strCtrlType.contains("CR")&&strCtrlType.contains("A")
                            &&CommonData::getCurrentSettingsVersion().servo1.at(0)=='7')
                    {
                        goto labelShow;
                    }
                    else
                    {
                        continue;
                    }
                }
                if (CommonData::getControllerType().name.contains("Nova",Qt::CaseInsensitive))
                {
                    if (CommonData::getCurrentSettingsVersion().servo1.at(0)=='2' &&
                            version.contains("Nova10V",Qt::CaseInsensitive)&&version.contains("3.5.8.1"))
                    {
                        continue;
                    }
                    else if (CommonData::getCurrentSettingsVersion().servo1.at(0)=='3' &&
                             !version.contains("Nova10V",Qt::CaseInsensitive)&&version.contains("3.5.8.1"))
                    {
                        continue;
                    }
                }
                if (!CommonData::getControllerType().name.contains("Nova",Qt::CaseInsensitive) &&
                        version.contains("Nova10V",Qt::CaseInsensitive))
                {
                    continue;
                }
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)&&!version.contains("CRA"))
                {
                    continue;
                }
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6)&&!version.contains("ED6"))
                {
                    continue;
                }
                if(!CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)
                        &&!CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6)
                        &&(version.contains("CRA")||version.contains("ED6")))
                {
                    continue;
                }
            }
labelShow:
            if(existVersions.contains(version)){
                auto pAct = currentBigVersion->addAction(QIcon(":/image/images/menu_download.png"),version);
                pAct->setProperty("hasPackage",QVariant(true));
            }else{
                auto pAct = currentBigVersion->addAction(QIcon(":/image/images/menu_undownload.png"),version);
                pAct->setProperty("hasPackage",QVariant());
            }
        }
    }
    if(versions.contains("temporary")){
        QMenu* currentBigVersion = m_pMenu->addMenu("temporary");
        currentBigVersion->setStyleSheet(strQss);
        currentBigVersion->setWindowFlag(Qt::NoDropShadowWindowHint);
        QStringList smallVersions = versions.value("temporary");
        for(QString version:smallVersions){
            auto bShowSubMenu = CLuaApi::getInstance()->isShowSearchSubMenuItems(version);
            if (bShowSubMenu.hasValue())
            {
                if (!bShowSubMenu.value()) continue;
            }
            else
            {
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)&&!version.contains("CRA"))
                {
                    continue;
                }
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6)&&!version.contains("ED6"))
                {
                    continue;
                }
                if(!CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)
                        &&!CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6)
                        &&(version.contains("CRA")||version.contains("ED6"))) {
                    continue;
                }
            }
            auto pAct = currentBigVersion->addAction(QIcon(":/image/images/menu_download.png"),version);
            pAct->setProperty("hasPackage",QVariant(true));
        }
    }
}

void MainWidget2::slot_menuClicked(QAction *pAction)
{
    if (!pAction) return;
    QMenu* pMenu = static_cast<QMenu*>(pAction->parent());
    m_upgradeParentVersion = pMenu->title();
    m_upgradeVersion = pAction->text();

    bool bHasPack = pAction->property("hasPackage").toBool();
    changeMenuButtonText(m_upgradeVersion, bHasPack);

    ui->btnUpgrade->show();
    ui->btnUpgrade->setDisabled(false);

    readUpgradeVersion(m_upgradeParentVersion,m_upgradeVersion);

    isCurrentVersionEnableUpgrade();
    if (!ui->btnUpgrade->isEnabled())
    {
        WidgetToast::warn(tr("当前固件版本信息获取不全，无法一键升级"),3000);
    }
    qDebug()<<"点击菜单选择升级包后，准备开始进行伺服参数校验对比.....";
    showServoParamLabel(bHasPack);
}

void MainWidget2::slot_connectClicked()
{
    ui->btnConnect->setDisabled(true);
    ui->stackedWidget->setDisabled(true);
    m_searchDeviceAndIsOnlineTimer->stop();
    if(ui->btnConnect->property("isConnected").toInt()== -1){
        this->style()->unpolish(ui->btnConnect); //清除旧的样式
        ui->btnConnect->setProperty("isConnected",1);
        this->style()->polish(ui->btnConnect);   //更新为新的样式
        ui->btnConnect->setText(tr("连接中"));
        m_strCurrentIP = ui->boxDevice->currentText();
        g_strCurrentIP = m_strCurrentIP;

        //初始化单个升级功能里的所有固件是否可升级状态
        CommonData::setSafeIOFOEStatus(0);
        CommonData::setSafeIOXMLStatus(0);
        CommonData::setFeedbackStatus(0);
        CommonData::setUniIOFOEStatus(0);
        CommonData::setUniIOXMLStatus(0);
        CommonData::setCCBOXFOEStatus(0);
        CommonData::setCCBOXXMLStatus(0);
        CommonData::setMainControlStatus(0);
        CommonData::setServoJ1FOEStatus(0);
        CommonData::setServoJ1XMLStatus(0);
        CommonData::setServoJ2FOEStatus(0);
        CommonData::setServoJ2XMLStatus(0);
        CommonData::setServoJ3FOEStatus(0);
        CommonData::setServoJ3XMLStatus(0);
        CommonData::setServoJ4FOEStatus(0);
        CommonData::setServoJ4XMLStatus(0);
        CommonData::setServoJ5FOEStatus(0);
        CommonData::setServoJ5XMLStatus(0);
        CommonData::setServoJ6FOEStatus(0);
        CommonData::setServoJ6XMLStatus(0);
        CommonData::setTerminalFOEStatus(0);
        CommonData::setTerminalXMLStatus(0);
        CommonData::setSafeBFOEStatus(0);

        m_isUpgradeSuccessStatus = 0;
        ui->labelTopControlType->show();
        ui->stackedWidget->setDisabled(false);
        setStackedWidgetDisable(false,true);
        ui->widgetFuncSelection->setDisabled(false);
        ui->btnAboutMe->setDisabled(false);

        CommonData::setSettingsProductInfoHardwareInfo(m_httpProtocol->getSettingsProductInfoHardwareInfo(g_strCurrentIP));
        CommonData::setStrPropertiesCabinetType(m_httpProtocol->getPropertiesCabinetType(g_strCurrentIP));
        DobotType::ControllerType controllerType = m_httpProtocol->getControllerType(g_strCurrentIP);
        CommonData::setControllerType(controllerType);
        qDebug()<<"=====>cabType="<<CommonData::getStrPropertiesCabinetType().name<<",collerType="<<controllerType.name<<",extName="<<controllerType.nameExt<<'\n';
        DobotType::SettingsVersion settingsVersion = m_httpProtocol->postSettingsVersion(g_strCurrentIP,QString());
        if(settingsVersion.control.isEmpty()){
            settingsVersion = m_httpProtocol->getSettingsVersion(g_strCurrentIP);
        }
        if(settingsVersion.control.isEmpty())
        {
            qDebug()<<"get SettingsVersion failed,  Please check the network cable plugging and unplugging ";
            m_searchDeviceAndIsOnlineTimer->start(2500);
            return;
        }
        CommonData::setCurrentSettingsVersion(settingsVersion);
        //连接后启动lua脚本
        CLuaApi::getInstance()->startLua();

        if(g_isRestartServer){
            g_isRestartServer = false;
            SFtpConnectParam prm;
            prm.iPort = 22;
            prm.strPwd = "dobot";
            prm.strHost = g_strCurrentIP;
            prm.strUserName = "root";
            m_csFtpClient->connectHost(prm);
            widget2StartServer();
        }

        readServoJointTable();
        //判断一代大型控制柜是否需要显示一键升级伺服参数按钮
        showUpgradeServoParamAllWriteUI(false);
        auto bOnekeyServoFuncCanUse = CLuaApi::getInstance()->isOnekeyServoFuncCanUse();
        if (bOnekeyServoFuncCanUse.hasValue())
        {
            m_strServoFuncCanUseTips = bOnekeyServoFuncCanUse.value().second;
            if (bOnekeyServoFuncCanUse.value().first)
            {
                showUpgradeServoParamAllWriteUI(true);
            }
        }
        else
        {
            m_strServoFuncCanUseTips = "";
            QString strControl = CommonData::getCurrentSettingsVersion().control;
            QString servo = CommonData::getCurrentSettingsVersion().servo1;
            strControl.remove(QChar('V'),Qt::CaseInsensitive);
            servo.remove(QChar('V'),Qt::CaseInsensitive);
            int intVersion = stringVersionToIntVersion(strControl);
            int intServoVersion = stringVersionToIntVersion(servo);
            qDebug()<<"btnUpgradeServoParamAllWriteHidden intVersion "<<intVersion<<",intServoVersion "<<intServoVersion;
            if (CommonData::getControllerType().name.contains("CR"))
            {//CR/CRV2/CRA:控制器》=3560 4133《=伺服固件《V7
                if(intVersion >= 3560 && intServoVersion >= 4133 && intServoVersion<7000)
                {
                    showUpgradeServoParamAllWriteUI(true);
                }
            }
            else if (CommonData::getControllerType().name.contains("Nova",Qt::CaseInsensitive))
            {
                if(intVersion >= 3580 && intServoVersion >= 2211)
                {
                    showUpgradeServoParamAllWriteUI(true);
                }
            }
            else if (CommonData::getControllerType().name.contains("E6",Qt::CaseInsensitive))
            {
                if(intVersion >= 4000 && intServoVersion >= 3034)
                {
                    showUpgradeServoParamAllWriteUI(true);
                }
            }
        }

        const QString strPropertiesCabinetType = CommonData::getStrPropertiesCabinetType().name;
        if(strPropertiesCabinetType == "CCBOX"){
            qDebug()<< "小型控制柜(CCBOX) ";
        }else if(strPropertiesCabinetType == "CC162"){
            qDebug()<< "标准控制柜(CC162) ";
            ui->labelCurrentSafeIOTheme->setText(tr("安全IO固件："));
        }else if(strPropertiesCabinetType == "CC262"){
            ui->labelCurrentSafeIOTheme->setText(tr("安全IO板A："));
            qDebug()<< "标准控制柜(CC262) ";
        }else{
            ui->labelCurrentSafeIOTheme->setText(tr("安全IO固件："));
            qDebug()<< "标准控制柜";
        }

        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
        {
            ui->labelTopControlType->setText(CommonData::getStrPropertiesCabinetType().name);
        }
        else
        {
            ui->labelTopControlType->setText(CommonData::getControllerType().name+"-"+CommonData::getStrPropertiesCabinetType().name);
        }

        if(!CommonData::getCurrentSettingsVersion().control.isEmpty())
        {
            ui->labelCurrentMainControlVersion->setAttribute(Qt::WA_AlwaysShowToolTips);
            setLabelToolTip(ui->labelCurrentMainControlVersion,CommonData::getCurrentSettingsVersion().control);
            ui->textSingleMainControl->setAttribute(Qt::WA_AlwaysShowToolTips);
            setLabelToolTip(ui->textSingleMainControl,CommonData::getCurrentSettingsVersion().control);
        }
        if(!settingsVersion.safeio.isEmpty()){
            ui->textSingleSafeIO->setText(settingsVersion.safeio);
            ui->labelCurrentSafeIOVersion->setText(settingsVersion.safeio);
        }
        if(!settingsVersion.feedback.isEmpty()){
            ui->textSingleFeedback->setText(settingsVersion.feedback);
            ui->labelCurrentFeedbackVersion->setText(settingsVersion.feedback);
        }
        if(!settingsVersion.unio.isEmpty()){
            ui->textSingleCCBOX->setText(settingsVersion.unio);
            ui->textSingleUniIO->setText(settingsVersion.unio);
            ui->labelCurrentUnIOVersion->setText(settingsVersion.unio);
            ui->labelCurrentIOFirmwareVersion->setText(settingsVersion.unio);
        }
        if(!settingsVersion.servo1.isEmpty()){
            ui->textSingleServoJAll->setText(settingsVersion.servo1);
            ui->textSingleServoJ1->setText(settingsVersion.servo1);
            ui->labelCurrentJ1Version->setText(settingsVersion.servo1);
        }
        if(!settingsVersion.servo2.isEmpty()){
            ui->textSingleServoJ2->setText(settingsVersion.servo2);
            ui->labelCurrentJ2Version->setText(settingsVersion.servo2);
        }
        if(!settingsVersion.servo3.isEmpty()){
            ui->textSingleServoJ3->setText(settingsVersion.servo3);
            ui->labelCurrentJ3Version->setText(settingsVersion.servo3);
        }
        if(!settingsVersion.servo4.isEmpty()){
            ui->textSingleServoJ4->setText(settingsVersion.servo4);
            ui->labelCurrentJ4Version->setText(settingsVersion.servo4);
        }
        if(!settingsVersion.servo5.isEmpty()){
            ui->textSingleServoJ5->setText(settingsVersion.servo5);
            ui->labelCurrentJ5Version->setText(settingsVersion.servo5);
        }
        if(!settingsVersion.servo6.isEmpty()){
            ui->textSingleServoJ6->setText(settingsVersion.servo6);
            ui->labelCurrentJ6Version->setText(settingsVersion.servo6);
        }
        if(!settingsVersion.terminal.isEmpty()){
            ui->textSingleTerminal->setText(settingsVersion.terminal);
            ui->labelCurrentTerminalVersion->setText(settingsVersion.terminal);
        }
        ui->btnConnect->setStyleSheet("QPushButton { \n  border: none;\n  border-radius: 4px;"
                                        "\n  background-color: rgba(255,255,255,20);\n  color: rgb(255, 255, 255);\n}QPushButton:disabled{ /* all types of tool button */\n  border: none;\n  border-radius: 4px;\n  background-color: #C7C7C7;\n  color: rgb(255, 255, 255);\n}\n");
        sleep(1000);
        m_searchDeviceAndIsOnlineTimer->start(9000);
        qDebug()<<" ---------------******** ";
        QString reply;
        for(int i = 0;i < 3; i++)
        {
            DobotType::StructProtocolVersion structProtocolVersion;
            structProtocolVersion = m_httpProtocol->getProtocolVersion(g_strCurrentIP);
            reply = structProtocolVersion.toolVersion;
            if(!reply.isEmpty())
            {
                CommonData::setStructProtocolVersion(structProtocolVersion);
                m_strProtocolVersionSlaveFiles = structProtocolVersion.pathInfo.slave;
                m_strProtocolVersionControl = structProtocolVersion.pathInfo.control;
                m_strProtocolVersionKernel = structProtocolVersion.pathInfo.kernel;
                break;
            }
        }

        /********************************************************************************/
        bool isSNEmpty = false;
        if(CommonData::getCurrentSettingsVersion().control.at(0)=='3')//如果当前为V3，用V3的接口设置当前控制器的SN码
        {
            DobotType::SettingsProductInfoHardwareInfo settingsProductInfoHardwareInfo;
            settingsProductInfoHardwareInfo.ControllCabinetSNCode = m_httpProtocol->getSettingsProductInfoControllerSn(g_strCurrentIP);
            CommonData::setSettingsProductInfoHardwareInfo(settingsProductInfoHardwareInfo);
        }
        if(!CommonData::getCurrentSettingsVersion().control.isEmpty())
        {
            if(isCurrentControlLagerTargetControl(CommonData::getCurrentSettingsVersion().control,"4.5.0.0")
                    &&isCurrentFWLargerTargetFWVersion(CommonData::getCurrentSettingsVersion().servo1,"6.1.4.0"))
            {
                if(CommonData::getSettingsProductInfoHardwareInfo().ControllCabinetSNCode.isEmpty()
                        &&CommonData::getSettingsProductInfoHardwareInfo().RealArmSNCode.isEmpty()
                        &&CommonData::getSettingsProductInfoHardwareInfo().RobotArmSNCode.isEmpty())
                {
                    isSNEmpty = true;
                }
            }
            else
            {
                isSNEmpty = true;
            }
        }
        DobotType::ProtocolExchangeResult info = m_httpProtocol->getProtocolExchange(g_strCurrentIP);
        if(!info.alarms.isEmpty()&&info.alarms[0].contains(4112))
        {
            isSNEmpty = false;
            slotSNAlarmed(true);
        }
        else
        {
            slotSNAlarmed(false);
        }
        slotFromTitleBarToSN(isSNEmpty);

        if(reply.isEmpty())
        {
            g_isRestartServer = true;
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("连接控制器服务失败，请断开重连"));
            pWndMsg->show();
            setStackedWidgetDisable(true,false);
        }
        else
        {
            searchDeviceDisable(false);
        }

        QString servoParameterTemplateCSV = getServoTemplateFile();
        qDebug()<<" servoParameterTemplateCSV ---- >"<<servoParameterTemplateCSV;
        mServoParamsTableModel->removeRows(0, mServoParamsTableModel->rowCount());
        m_myCheckBoxHeader->setIsChecked(false);
        m_singleReadAWriteDelegate->clear();
        QFileInfo servoParamCSVfile(servoParameterTemplateCSV);
        if(servoParamCSVfile.exists() && servoParamCSVfile.isFile())
        {
            importServoParamsExcel(servoParameterTemplateCSV);
        }
        searchMenuItems();
        ui->btnConnect->setText(tr("断开连接"));
        ui->btnConnect->style()->unpolish(ui->btnConnect); //清除旧的样式
        ui->btnConnect->setProperty("isConnected",2);
        ui->btnConnect->style()->polish(ui->btnConnect);   //更新为新的样式
        ui->btnConnect->repaint();
        //以下是判断当前控制柜型号以及本体型号是否匹配，如果不匹配则禁止使用维护工具
        bool isDisableUse = true;
        QString strReason;
        auto bIsCanUseTool = CLuaApi::getInstance()->isCanUseTool(strReason);
        if (bIsCanUseTool.hasValue())
        {
            if (bIsCanUseTool.value())
            {
                isDisableUse = false;
            }
        }
        else
        {
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
            {
                QString name = CommonData::getControllerType().name;
                if(name.contains("Nova")||"CR3"==name||"CR5"==name||
                        "CR7"==name||"CR10"==name||"CR12"==name||"CR16"==name)
                {
                    isDisableUse = false;
                }
            }
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
            {
                if(CommonData::getControllerType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
                {
                    isDisableUse = false;
                }
            }
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162))
            {
                if(CommonData::getControllerType().name.contains("CR")&&!CommonData::getControllerType().name.contains("A"))
                {
                    isDisableUse = false;
                }
            }
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC262))
            {
                if(CommonData::getControllerType().name.contains("CR")&&CommonData::getControllerType().name.contains("A"))
                {
                    isDisableUse = false;
                }
            }
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC263))
            {
                if(CommonData::getControllerType().name.contains("CR20A"))
                {
                    isDisableUse = false;
                }
            }
        }
        if(isDisableUse)
        {
            if (strReason.isEmpty()){
                strReason = tr("维护工具暂不适配该产品，或产品搭配有误。");
            }
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",strReason);
            pWndMsg->show();
            ui->btnConnect->clicked();
            return;
        }

        disableWidgetWhenConnected();

        qDebug()<<" ---------------******** ";
        DobotType::ProtocolExchangeResult protocolExchange = m_httpProtocol->getProtocolExchange(g_strCurrentIP);
        if (protocolExchange.isAlarm)
        {
            auto ptr = new ControllerWarn(this);
            ptr->setGeometry(0,0,width(),height());
            ptr->show();
            ptr->raise();
        }
        if(protocolExchange.controlMode == g_controlModeEnable)
        {
            m_isDisableControlWidget->show();
            m_isDisableControlWidget->raise();
        }
        showFirmwareAdvanceUI(); //显示一键升级页面、单个升级/高级功能页面的元素
    }else{
        g_isRestartServer = true;//保证每次连接都要重新启动startservertool.exe，防止用户不关闭软件而是直接连接另外一台机器导致启动错误。
        CLuaApi::getInstance()->stopLua(); //断开连接时停止脚本
        ui->stackedWidget->setDisabled(false);
        setStackedWidgetDisable(true,true);
        //        initFirmwareCheckWidgetStatus();
        ui->labelTopControlType->hide();
#ifndef V136_NOVA_UPDATE
        ui->btnDownload->show();
#endif
        ui->btnImport->show();
        ui->btnMenu->show();
        ui->btnUpgrade->show();
        textClear();
        searchDeviceDisable(true);
        ui->btnConnect->setDisabled(false);
        ui->labelDeviceIcon->setDisabled(false);
        QFont font;
        font.setUnderline(true);
        //ui->btnAboutMe->setDisabled(true);
        ui->btnConnect->setText(tr("连接"));
        ui->btnConnect->setProperty("isConnected",-1);
        //断开连接后初始化伺服参数模块中的表格的高亮
        for(int row = 0; row < mServoParamsTableModel->rowCount();row++)
        {
            if(mServoParamsTableModel->item(row,colValue) != NULL)
            {
                mServoParamsTableModel->item(row,colValue)->setForeground(QBrush(QColor("#2D3440")));
            }
        }
        //取消所有勾选框
        m_myCheckBoxHeader->setIsChecked(false);
        m_myCheckBoxHeader->checkBoxClicked(false);
        ui->btnConnect->setStyleSheet("QPushButton { \n  border: none;\n  border-radius: 4px;"
                                        "\n  background-color: #30CF58;\n  color: rgb(255, 255, 255);\n}QPushButton:disabled{ /* all types of tool button */\n  border: none;\n  border-radius: 4px;\n  background-color: #C7C7C7;\n  color: rgb(255, 255, 255);\n}\n");
        m_searchDeviceAndIsOnlineTimer->start(2500);

        //初始化系统工具里的检测功能
        initSystemToolDectect();
        initFileRecoveryWidget();
    }
}

void MainWidget2::slot_searchDevice()
{
    m_searchDeviceAndIsOnlineTimer->stop();
    int connectCount = 0;
    bool currentIPExist = true;

    /*之所以这么做，是因为下面的m_httpProtocol->getConnectionState()内部使用QEventLoop.exec()进入事件循环导致程序在这里暂停，而转入到其他地方执行。
      当搜索设备IP的slot_saveIpsFromManual时间到并被触发，这样修改了m_ipList，而此时的http接口也返回了，继续执行。
      因为m_ipList内部数据结构被修改了，for循环的迭代器失效了，结果导致崩溃。
      所以需要将此变量提前拿出来，这样不至于迭代器失效导致崩溃。
    */
    QStringList arrIpList = m_ipList;
    for(QString ip : arrIpList)
    {
        //如果当前IP存在，那么只给当前IP查看当前状态
        if(!m_strCurrentIP.isEmpty())
        {
            if(m_strCurrentIP != ip)
            {
                continue;
            }
        }
        //        qDebug()<<" ip****** "<<ip;
        int exist = false;
        if(m_httpProtocol->getConnectionState(ip) != ConnectState::unconnected){
#ifndef V136_NOVA_UPDATE
            ui->btnDownload->show();
#endif
            ui->btnImport->show();
            ui->labelDeviceIcon->setDisabled(false);
            ui->btnConnect->show();
            ui->btnConnect->setDisabled(false);
            ui->btnManuallyAdd->hide();
            connectCount++;
            if(ui->boxDevice->itemData(0).toString() == "SearchDevices"){
                ui->boxDevice->clear();
                ui->boxDevice->addItem(ip);
            }
            for(int i=0 ;i<ui->boxDevice->count() ;i++){
                if(ip == ui->boxDevice->itemText(i)){
                    exist = true;
                    break;
                }
            }
            if(!exist){
                ui->boxDevice->addItem(ip);
            }
        }else{
            if(ip == g_strCurrentIP){
                currentIPExist = false;
            }
            for(int i=0 ;i<ui->boxDevice->count() ;i++){
                if(ip == ui->boxDevice->itemText(i)){
                    ui->boxDevice->removeItem(i);
                    break;
                }
            }
        }
    }
    slot_isOnline();
    if(connectCount == 0)
    {
        ui->labelTopControlType->hide();
        m_strCurrentIP = "";
        textClear();
        qDebug()<<"  connectCount == 0  ======";
        DobotType::StructProtocolVersion structProtocolVersion;
        structProtocolVersion = m_httpProtocol->getProtocolVersion(g_strCurrentIP);
        if(!structProtocolVersion.toolVersion.isEmpty())
        {
            setStackedWidgetDisable(true,true);
        }
        else
        {
            setStackedWidgetDisable(true,false);
        }

        ui->btnDownload->hide();
        ui->btnImport->hide();
        ui->btnUpgrade->hide();
        searchDeviceDisable(true);
        QFont font;
        font.setUnderline(true);
        ui->boxDevice->addItem(tr("搜索设备中..."),QVariant("SearchDevices"));
        ui->boxDevice->show();
        ui->btnManuallyAdd->show();
        ui->btnConnect->setProperty("isConnected",-1);
        ui->btnConnect->setText(tr("连接"));
        //ui->btnAboutMe->setDisabled(true);
        g_isRestartServer = true;
        initFileRecoveryWidget();
        //        initFirmwareCheckWidgetStatus();
        ui->btnConnect->hide();
        ui->btnConnect->setStyleSheet("QPushButton { /* all types of tool button */\n  "
                                        "border: none;\n  border-radius: 4px;\n  background-color: #30CF58;\n  "
                                        "color: rgb(255, 255, 255);\n}\nQPushButton:disabled{ "
                                        "/* all types of tool button */\n  border: none;\n  border-radius: 4px;\n  "
                                        "background-color: #C7C7C7;\n  color: rgb(255, 255, 255);\n}\n");
        if(!ui->btnFirmwareUpgrade->isChecked())
        {
            ui->btnFirmwareUpgrade->clicked(true);
        }

        //断开连接后初始化伺服参数模块中的表格的高亮
        for(int row = 0; row < mServoParamsTableModel->rowCount();row++)
        {
            if(mServoParamsTableModel->item(row,colValue) != NULL)
            {
                mServoParamsTableModel->item(row,colValue)->setForeground(QBrush(QColor("#2D3440")));
            }
        }
        //取消所有勾选框
        m_myCheckBoxHeader->setIsChecked(false);
        m_myCheckBoxHeader->checkBoxClicked(false);
        m_searchDeviceAndIsOnlineTimer->start(2500);

        //初始化系统工具里的检测功能
        initSystemToolDectect();
    }
    else
    {
        m_searchDeviceAndIsOnlineTimer->start();
    }
}

void MainWidget2::slot_openManuallyIPWidget()
{
    auto ptr = new ManualIpWidget(this);
    connect(ptr,&ManualIpWidget::signal_saveIpsToMainWindow,this,&MainWidget2::slot_saveIpsFromManual);
    ptr->setGeometry(0,0,width(),height());
    ptr->show();
}

void MainWidget2::slot_saveIpsFromManual(QStringList ipList)
{
    m_searchDeviceAndIsOnlineTimer->stop();
    m_ipList.clear();
    m_ipList.append("192.168.5.1");
    m_ipList.append("192.168.200.1");
    for (auto ip : ipList){
        if (!m_ipList.contains(ip)){
            m_ipList.append(ip);
        }
    }
    m_searchDeviceAndIsOnlineTimer->start(2500);
}

void MainWidget2::slot_download()
{
    if (m_bIsCheckingServoParam)
    {
        WidgetToast::warn(tr("请等待伺服参数对比完成"), 2000);
        return ;
    }
    qDebug()<< " slot_download **** ";
    if(m_upgradeVersion.isEmpty()){
        auto* pWndMsg = new MessageWidget(this);
        pWndMsg->setMessage("warn",tr("请先选择控制器版本"));
        pWndMsg->show();
        return;
    }
    m_diyProgressDialog->setGeometry(0,0,width(),height());
    m_diyProgressDialog->setHeadLabel("downloading...");
    m_diyProgressDialog->setTitle(tr("下载中"));
    m_diyProgressDialog->setInitProgressValue(0);
    if(!m_upgrade2Widget->isVisible()){
        m_diyProgressDialog->raise();
        m_diyProgressDialog->show();
    }
    QString upgradeDir = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion;
#ifndef V136_NOVA_UPDATE
    qDebug()<<"m_download QCoreApplication::applicationDirPath()"<<QCoreApplication::applicationDirPath();
    m_upgradeFileZipDownload->startDownload(g_strHttpDobotPrefix+"/"+m_upgradeParentVersion+"/"+m_upgradeVersion+".zip",QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion);
#endif
}

void MainWidget2::slot_downloadProgress(qint64 bytesRead, qint64 totalBytes, qreal progress)
{
    qDebug()<<" bytesRead "<<bytesRead<<" totalBytes "<<totalBytes<<endl;
    m_diyProgressDialog->setProgressRange(totalBytes);
    m_diyProgressDialog->setProgressValue(bytesRead);
}

void MainWidget2::slot_downloadFinished(bool ok)
{
    qDebug()<<"=============slot_downloadFinished>>>>>>>"<<ok;
    if(!ok){
        if(!m_diyProgressDialog->isHidden()){
            m_diyProgressDialog->setTitle(tr("下载失败"));
            m_diyProgressDialog->setHeadLabel("failed!");
            qDebug()<<"slot_downloadFinished failed";
        }
        return;
    }
    qDebug()<<"  QCoreApplication::applicationDirPath() "<<QCoreApplication::applicationDirPath();
    QLibrary lib(QCoreApplication::applicationDirPath()+"\\ZLibWrap.dll");
    if(lib.load()){
        ZWZipExtract zWZipExtract = (ZWZipExtract)lib.resolve("ZWZipExtract");
        QString sourceZip = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+".zip";
        QString dstZipPath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion;
        //如果存在目标文件夹，需先删除。
        QDir dstDir(dstZipPath+"/"+m_upgradeVersion);
        if(dstDir.exists()){
            dstDir.removeRecursively();
        }
        long result = zWZipExtract(QDir::toNativeSeparators(sourceZip).toStdString().c_str(),QDir::toNativeSeparators(dstZipPath).toStdString().c_str());
        if(m_zipDiyManager->zipUnCompress(sourceZip,dstZipPath+"/",QString())){
            if(!m_diyProgressDialog->isHidden()){
                m_diyProgressDialog->setTitle(tr("下载成功"));
                m_diyProgressDialog->setHeadLabel("success!");
            }
            searchMenuItems();
            ui->btnUpgrade->show();
            readUpgradeVersion(m_upgradeParentVersion,m_upgradeVersion);
            if(m_upgrade2Widget->isVisible()){
                widget2UpgradeFirmwareDowloadFinish(true);
            }
            changeMenuButtonText(m_upgradeVersion, true);
            qDebug()<<"下载完升级包后，准备开始进行伺服参数校验对比....."<<sourceZip;
            showServoParamLabel(true);
        }else{
            if(!m_diyProgressDialog->isHidden()){
                m_diyProgressDialog->setTitle(tr("下载失败"));
                m_diyProgressDialog->setHeadLabel("failed!");
            }
        }
        QFile file(sourceZip);
        qDebug()<<"file.remove()   " <<file.remove();
    }else{
        qWarning()<<"程序目录下缺少压缩库文件";
        if(!m_diyProgressDialog->isHidden()){
            m_diyProgressDialog->setTitle(tr("解压失败，程序目录下缺少解压缩库文件"));
            m_diyProgressDialog->setHeadLabel("failed!");
        }
    }
}

static bool IsValidUpdatePackage(const QString strDir)
{
    QDir dir(strDir);
    const QString strVersion = dir.dirName();
    const QFileInfo info1(dir,strVersion+"detailChinese.txt");
    const QFileInfo info2(dir,strVersion+"detailEnglish.txt");
    const QFileInfo info3(dir,strVersion+"upgrade.ini");
    const QFileInfo info4(dir,"files");
    const QFileInfo info5(dir,"servoCSV");
    QStringList folders = dir.entryList(QStringList()<<QString("*CR_*update_*"),QDir::Dirs|QDir::NoDotAndDotDot);
    if (!info1.exists() || !info1.isFile()) return false;
    if (!info2.exists() || !info2.isFile()) return false;
    if (!info3.exists() || !info3.isFile()) return false;
    if (!info4.exists() || !info4.isDir()) return false;
    if (!info5.exists() || !info5.isDir()) return false;
    if (folders.isEmpty()) return false;

    QString upgradeInfoPath = info3.absoluteFilePath();
    QHash<QString,QHash<QString,QString>> allLuaVal;
    QSettings iniFiles(upgradeInfoPath,QSettings::IniFormat);
    QStringList allChilds = iniFiles.childGroups();
    for (int i=0; i<allChilds.size(); ++i){
        QString strGN = allChilds[i];
        QHash<QString,QString> allKV;
        iniFiles.beginGroup(strGN);
        QStringList allKeys = iniFiles.allKeys();
        for (int j=0; j<allKeys.size(); ++j){
            QString strKey = allKeys[j];
            QString strVal = iniFiles.value(strKey).toString();
            allKV.insert(strKey,strVal);
        }
        iniFiles.endGroup();
        allLuaVal.insert(strGN, allKV);
    }
    auto luaValueIni = CLuaApi::getInstance()->isCanImportPackage(strVersion, allLuaVal);
    if (luaValueIni.hasValue()){
        return luaValueIni.value();
    }
    return true;
}
void MainWidget2::slot_importTempVersion()
{
    if (m_bIsCheckingServoParam)
    {
        WidgetToast::warn(tr("请等待伺服参数对比完成"), 2000);
        return ;
    }
    QString srcDir = QFileDialog::getExistingDirectory(nullptr,QString(),
                                                       "/home",
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
    if(srcDir.isEmpty()){
        return;
    }
    if (!IsValidUpdatePackage(srcDir)){
        auto* pWndMsg = new MessageWidget(this);
        pWndMsg->setMessage("warn",tr("请导入正确的升级包！"));
        pWndMsg->show();
        return;
    }

    m_diyProgressDialog->setGeometry(0,0,width(),height());
    m_diyProgressDialog->raise();
    m_diyProgressDialog->show();
    m_diyProgressDialog->setTitle(tr("导入中..."));
    m_diyProgressDialog->setHeadLabel("importing");
    QDir dir(srcDir);
    m_strImportDirName = dir.dirName();
    QDir dstDir(QCoreApplication::applicationDirPath()+"/upgradeFiles/temporary/"+m_strImportDirName);
    if(dstDir.exists()){
        dstDir.removeRecursively();
    }
    qDebug()<<" dir.dirName() "<< dir.dirName();
    emit m_importTempVersionHandler->signal_copyFolder(srcDir,QCoreApplication::applicationDirPath()+"/upgradeFiles/temporary/",true);
    sleep(500);
}

void MainWidget2::slot_importTempVersionProgress(int progress,int total)
{
    m_diyProgressDialog->setProgressValue(progress);
    m_diyProgressDialog->setProgressRange(total);
}

void MainWidget2::slot_closeWindow()
{
    m_upgradeVersion = "";
    m_upgradeParentVersion = "";

    CLuaApi::getInstance()->stopLua();
    qApp->quit();
}

void MainWidget2::slot_miniWindow()
{
    this->showMinimized();
}
#include <windows.h>
#include <QDesktopWidget>
void MainWidget2::slot_maxWindow(bool ok)
{
    if(ok){
        this->showMaximized();
        ui->btnMax->setStyleSheet("QPushButton{border-image: url(:/image/images2/icon_main_max_after.png);color: gray;}");
    }else{
        this->showNormal();
        ui->btnMax->setStyleSheet("image: url(:/image/images2/icon_main_max.png);border:none;");
    }
}

void MainWidget2::slot_onPanelBoxChanged(bool checked)
{
    QObject* btn = sender();
    ui->widgetBackup->setStyleSheet("border:none;");
    ui->widgetFirmwareUpgrade->setStyleSheet("border:none;");
    ui->widgetSystemTool->setStyleSheet("border:none;");
    ui->widgetFirmwareCheck->setStyleSheet("border:none;");
    ui->widgetServoParams->setStyleSheet("border:none;");

    ui->btnFirmwareUpgrade->setChecked(false);
    ui->btnSystemTool->setChecked(false);
    ui->btnFirmwareCheck->setChecked(false);
    ui->btnBackup->setChecked(false);
    ui->btnServoParams->setChecked(false);

    if(btn == ui->btnServoParams){
        if(m_bIsStackedWidgetDisable)
        {
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("当前22000端口服务断开"));
            pWndMsg->show();
            ui->widgetFirmwareUpgrade->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
            ui->btnFirmwareUpgrade->setChecked(true);
            return;
        }
        if (ui->btnUpgradeWidgetServoParamAllWrite->isHidden()){
            //点击了“伺服参数”功能，但是伺服对比和一键覆盖写入窗口是隐藏的，则不允许操作
            if (m_strServoFuncCanUseTips.isEmpty()){
                m_strServoFuncCanUseTips = tr("固件版本较低，不可使用此功能，请升级固件版本！");
            }
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",m_strServoFuncCanUseTips);
            pWndMsg->show();
            return;
        }

#ifdef QT_DEBUG
        m_isPasswordConfirm = true;
#endif
        if(m_isPasswordConfirm || EnvDebugConfig::isDebugEnv())
        {
            showServoParamWidget();
        }
        else
        {
            ui->btnServoParams->setChecked(true);
            ui->widgetServoParams->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
            ui->widgetFuncSelection->repaint();
            ui->stackedWidget->setCurrentIndex(7);
            m_currentPasswordRandom = CommonData::getRandom();
            ui->labelRandomNum->setText(tr("随机码:  ")+QString("%1").arg(m_currentPasswordRandom));
        }
        return;
    }
    if (btn == ui->btnFirmwareUpgrade){

        ui->widgetFirmwareUpgrade->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
        ui->stackedWidget->setCurrentIndex(0);
        ui->btnFirmwareUpgrade->setChecked(true);
    }
    if(btn == ui->btnFirmwareCheck){
        if(m_bIsStackedWidgetDisable)
        {
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("当前22000端口服务断开"));
            pWndMsg->show();
            ui->widgetFirmwareUpgrade->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
            ui->btnFirmwareUpgrade->setChecked(true);
            return;
        }
        ui->widgetFirmwareCheck->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
        ui->stackedWidget->setCurrentIndex(1);
        ui->btnFirmwareCheck->setChecked(true);
    }
    if(btn == ui->btnSystemTool){
        if(m_bIsStackedWidgetDisable)
        {
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("当前22000端口服务断开"));
            pWndMsg->show();
            ui->widgetFirmwareUpgrade->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
            ui->btnFirmwareUpgrade->setChecked(true);
            return;
        }
        ui->widgetSystemTool->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
        ui->stackedWidget->setCurrentIndex(2);
        ui->btnSystemTool->setChecked(true);
    }
    if(btn == ui->btnBackup){
        if(m_bIsStackedWidgetDisable)
        {
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("当前22000端口服务断开"));
            pWndMsg->show();
            ui->widgetFirmwareUpgrade->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
            ui->btnFirmwareUpgrade->setChecked(true);
            return;
        }
        ui->widgetBackup->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
        ui->stackedWidget->setCurrentIndex(3);
        ui->btnBackup->setChecked(true);
    }
}

void MainWidget2::slot_isOnline()
{
#ifndef V136_NOVA_UPDATE
    if (!m_httpProtocol->getIsOnline()) {
        //网络未连接，发送信号通知
        qDebug() << "当前网络未连接";
        isOnline = false;
        ui->btnUpgrade->setText(tr("升级"));
    }else{
        isOnline = true;
        ui->btnUpgrade->setText(tr("下载并升级"));
    }
#else
    isOnline = true;
    ui->btnUpgrade->setText(tr("下载并升级"));
#endif
}

void MainWidget2::slot_btnLanguageChange(bool isZh)
{
    qDebug()<<"isZh : "<<isZh;
    QSettings setting("config/config.ini",QSettings::IniFormat);
    if(isZh)
    {
        setting.setValue("language/style",0);
    }else{
        setting.setValue("language/style",1);
    }
    LangeuageFontChange(!isZh);
    if(!isZh){
        g_isEnglish = true;
        if(m_Trans->load(QCoreApplication::applicationDirPath()+"/tr_en.qm")){
            qApp->installTranslator(m_Trans);
        }else{
            qDebug()<<"  english tr_en.qm  load fail ....";
        }
    }else{
        g_isEnglish = false;
        if(m_Trans->load(QCoreApplication::applicationDirPath()+"/tr_zh.qm")){
            qApp->installTranslator(m_Trans);
        }else{
            qDebug()<<"  zh  tr_zh.qm load  fail ....";
        }
    }
}

void MainWidget2::slot_upgradeWidgetUpgradeSingle2Count()
{
    m_upgradeProgressTimer->stop();
    QTimer::singleShot(1000, this, [this]{
        qDebug()<<"try again one key update CR fwAndXml......begin......";
        upgradeAllFirmwareAndXMLs();
        qDebug()<<"try again one key update CR fwAndXml......end......";
    });
}

int MainWidget2::setSingleFuncUpgradeStatus(int status)
{
    if(status == 2)
    {
        auto* pWndMsg = new MessageWidget(this);
        pWndMsg->setMessage("warn",tr("该板卡已升级成功，请选择升级失败的板卡。"));
        pWndMsg->show();
    }
    return status;
}

void MainWidget2::slot_upgradeProgress()
{
    /*先停止定时器，不然下面http请求要是耗时很久，就会导致1s定时器多次触发，http继续排队，结果http服务恢复正常时，瞬间大量http请求导致
    http服务出问题。*/
    if (m_bGetUpdateCRStatus){
        return;
    }
    m_bGetUpdateCRStatus = true;
    DobotType::UpdateStatus upgradeStatus = m_httpProtocol->getUpdateCRStatus(g_strCurrentIP);
    m_bGetUpdateCRStatus = false;

    QList<int> intUpgradeStatusList = upgradeStatus.FWUpdateResult;
    qDebug()<<"upgradeStatus.bIsOutage : "<<upgradeStatus.bIsOutage;
    if(upgradeStatus.bIsOutage)
    {
        qDebug()<<"...................bIsOutage "<<upgradeStatus.bIsOutage;
        m_upgrade2Widget->setUpgradeStatus(intUpgradeStatusList, upgradeStatus.bIsOutage);

        if(!CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
        {
            if (!m_exceptionOutage.bStart){
                m_exceptionOutage.bStart = true;
                m_exceptionOutage.timer.restart();
            }
            else
            {
                if (m_exceptionOutage.timer.elapsed()>m_exceptionOutage.deltaTime){
                    m_upgradeProgressTimer->stop();
                    QEventLoop loop;
                    auto pDlg = new DlgMsgBox(this);
                    connect(pDlg, &DlgMsgBox::destroyed, &loop, &QEventLoop::quit);
                    pDlg->setTitle(tr("请重启控制柜"),DlgMsgBox::Type::ok);
                    pDlg->setOk(tr("好的"));
                    pDlg->show();
                    loop.exec();
                    m_exceptionOutage.bStart = false;
                    m_upgrade2Widget->initStatus();
                    m_upgrade2Widget->hide();
                }
            }
        }
        return;
    }
    else
    {
        m_exceptionOutage.bStart = false;
    }
    const int iFwUnio = m_upgrade2Widget->getIdxFWUniIO();
    const int iFwMainCtrl = m_upgrade2Widget->getIdxFWMainCtrl();
    const int iFwFeedback = m_upgrade2Widget->getIdxFWFeedback();
    const int iFwSafeIOB = m_upgrade2Widget->getIdxFWSafeIOB();
    const int iFwSafeIO = m_upgrade2Widget->getIdxFWSafeIO();
    const int iFwJ1 = m_upgrade2Widget->getIdxFWJ1();
    const int iFwJ2 = m_upgrade2Widget->getIdxFWJ2();
    const int iFwJ3 = m_upgrade2Widget->getIdxFWJ3();
    const int iFwJ4 = m_upgrade2Widget->getIdxFWJ4();
    const int iFwJ5 = m_upgrade2Widget->getIdxFWJ5();
    const int iFwJ6 = m_upgrade2Widget->getIdxFWJ6();
    const int iFwTerminal = m_upgrade2Widget->getIdxFWTerminal();

    const int iXmlUniIO = m_upgrade2Widget->getIdxXMLUniIO();
    const int iXmlSafeIO = m_upgrade2Widget->getIdxXMLSafeIO();
    const int iXmlJ1 = m_upgrade2Widget->getIdxXMLJ1();
    const int iXmlJ2 = m_upgrade2Widget->getIdxXMLJ2();
    const int iXmlJ3 = m_upgrade2Widget->getIdxXMLJ3();
    const int iXmlJ4 = m_upgrade2Widget->getIdxXMLJ4();
    const int iXmlJ5 = m_upgrade2Widget->getIdxXMLJ5();
    const int iXmlJ6 = m_upgrade2Widget->getIdxXMLJ6();
    const int iXmlTerminal = m_upgrade2Widget->getIdxXMLTerminal();

    CommonData::setUniIOFOEStatus(upgradeStatus.FWUpdateResult[iFwUnio]);
    CommonData::setUniIOXMLStatus(upgradeStatus.XMLUpdateResult[iXmlUniIO]);

    CommonData::setCCBOXFOEStatus(upgradeStatus.FWUpdateResult[iFwUnio]);
    CommonData::setCCBOXXMLStatus(upgradeStatus.XMLUpdateResult[iXmlUniIO]);

    CommonData::setMainControlStatus(upgradeStatus.FWUpdateResult[iFwMainCtrl]);

    CommonData::setFeedbackStatus(upgradeStatus.FWUpdateResult[iFwFeedback]);

    CommonData::setSafeBFOEStatus(upgradeStatus.FWUpdateResult[iFwSafeIOB]);

    CommonData::setSafeIOFOEStatus(upgradeStatus.FWUpdateResult[iFwSafeIO]);
    CommonData::setSafeIOXMLStatus(upgradeStatus.XMLUpdateResult[iXmlSafeIO]);

    CommonData::setServoJ1FOEStatus(upgradeStatus.FWUpdateResult[iFwJ1]);
    CommonData::setServoJ1XMLStatus(upgradeStatus.XMLUpdateResult[iXmlJ1]);

    CommonData::setServoJ2FOEStatus(upgradeStatus.FWUpdateResult[iFwJ2]);
    CommonData::setServoJ2XMLStatus(upgradeStatus.XMLUpdateResult[iXmlJ2]);

    CommonData::setServoJ3FOEStatus(upgradeStatus.FWUpdateResult[iFwJ3]);
    CommonData::setServoJ3XMLStatus(upgradeStatus.XMLUpdateResult[iXmlJ3]);

    CommonData::setServoJ4FOEStatus(upgradeStatus.FWUpdateResult[iFwJ4]);
    CommonData::setServoJ4XMLStatus(upgradeStatus.XMLUpdateResult[iXmlJ4]);

    CommonData::setServoJ5FOEStatus(upgradeStatus.FWUpdateResult[iFwJ5]);
    CommonData::setServoJ5XMLStatus(upgradeStatus.XMLUpdateResult[iXmlJ5]);

    CommonData::setServoJ6FOEStatus(upgradeStatus.FWUpdateResult[iFwJ6]);
    CommonData::setServoJ6XMLStatus(upgradeStatus.XMLUpdateResult[iXmlJ6]);

    CommonData::setTerminalFOEStatus(upgradeStatus.FWUpdateResult[iFwTerminal]);
    CommonData::setTerminalXMLStatus(upgradeStatus.XMLUpdateResult[iXmlTerminal]);

    intUpgradeStatusList[iFwUnio]=upgradeFwAndXmlStatusCheck(upgradeStatus.FWUpdateResult[iFwUnio],upgradeStatus.XMLUpdateResult[iXmlUniIO]);
    intUpgradeStatusList[iFwSafeIO]=upgradeFwAndXmlStatusCheck(upgradeStatus.FWUpdateResult[iFwSafeIO],upgradeStatus.XMLUpdateResult[iXmlSafeIO]);
    intUpgradeStatusList[iFwJ1]=upgradeFwAndXmlStatusCheck(upgradeStatus.FWUpdateResult[iFwJ1],upgradeStatus.XMLUpdateResult[iXmlJ1]);
    intUpgradeStatusList[iFwJ2]=upgradeFwAndXmlStatusCheck(upgradeStatus.FWUpdateResult[iFwJ2],upgradeStatus.XMLUpdateResult[iXmlJ2]);
    intUpgradeStatusList[iFwJ3]=upgradeFwAndXmlStatusCheck(upgradeStatus.FWUpdateResult[iFwJ3],upgradeStatus.XMLUpdateResult[iXmlJ3]);
    intUpgradeStatusList[iFwJ4]=upgradeFwAndXmlStatusCheck(upgradeStatus.FWUpdateResult[iFwJ4],upgradeStatus.XMLUpdateResult[iXmlJ4]);
    intUpgradeStatusList[iFwJ5]=upgradeFwAndXmlStatusCheck(upgradeStatus.FWUpdateResult[iFwJ5],upgradeStatus.XMLUpdateResult[iXmlJ5]);
    intUpgradeStatusList[iFwJ6]=upgradeFwAndXmlStatusCheck(upgradeStatus.FWUpdateResult[iFwJ6],upgradeStatus.XMLUpdateResult[iXmlJ6]);
    intUpgradeStatusList[iFwTerminal]=upgradeFwAndXmlStatusCheck(upgradeStatus.FWUpdateResult[iFwTerminal],upgradeStatus.XMLUpdateResult[iXmlTerminal]);

    m_upgrade2Widget->setErrorCode(upgradeStatus.errorCode);
    m_upgrade2Widget->setUpgradeStatus(intUpgradeStatusList, upgradeStatus.bIsOutage);
}

void MainWidget2::slot_downloadedSoftwareTime(bool ok)
{
    if(!ok){
        return;
    }
    if(m_currentSoftwareVersion==0){
        QFile file(QCoreApplication::applicationDirPath()+"/config/softWareInfo.ini");
        file.remove();
        return;
    }
    QSettings softwareSetting("config/softWareInfo.ini",QSettings::IniFormat);
    QStringList downloadedSoftwareVersions = softwareSetting.value("software/version").toString().split(".");
    QString downloadedSoftwareVersion;
    for(QString softwareVersion:downloadedSoftwareVersions){
        downloadedSoftwareVersion+=softwareVersion;
    }
    if(m_currentSoftwareVersion<downloadedSoftwareVersion.toInt()){
        DowloadTipWidget* downloadTipWidget = new DowloadTipWidget(this);
        downloadTipWidget->setGeometry(0,0,width(),height());
        QString url = softwareSetting.value("software/url").toString();
        if(!url.isEmpty()){
            downloadTipWidget->setMessage(url);
        }
        downloadTipWidget->raise();
        downloadTipWidget->show();
    }else{
        qDebug()<<"slot_downloadedSoftwareTime no url";
    }
    QString currrentSoftwareVersion;
    currrentSoftwareVersion = QString("%1.%2.%3").arg(m_currentSoftwareVersion/100).arg((m_currentSoftwareVersion/10)%10).arg(m_currentSoftwareVersion%10);
    softwareSetting.setValue("software/version",currrentSoftwareVersion);
}

void MainWidget2::slot_downloadAllVersionIniFinished(bool ok)
{
    qDebug()<<"allVersion.ini download ----------------------------------------------> "<<ok;
    if (!ok)
    {//下载失败，则重新下载
        QTimer::singleShot(2000, this, [this]{
            m_upgradeAllVersionIniDownload->startDownload(g_strHttpDobotPrefix+"/"+g_strAllVersion
                                                          ,QCoreApplication::applicationDirPath()+"/upgradeFiles");
        });
        return;
    }
    const QString strDownFile = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+g_strAllVersion;
    const QString strLocalFile = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+g_strAllVersion+".cfg";

    QFileInfo info(strDownFile);
    if (!info.exists()) return;
    if (info.size()<=0)
    {
        QFile::remove(strDownFile);
        return ;
    }
    QFile::remove(strLocalFile);
    QFile::rename(strDownFile,strLocalFile);

    m_pAllVersionSetting->deleteLater();
    m_pAllVersionSetting = new QSettings(strLocalFile,QSettings::IniFormat);
}

void MainWidget2::slot_downloadUpdateConfigJsonFinished(bool ok)
{
    qDebug()<<"updateconfig.json download ---------------------------------------------->"<<ok;
    if (!ok)
    {//下载失败，则重新下载
        QTimer::singleShot(3000, this, [this]{
            m_updateConfigJsonDownload->startDownloadFile(getUpdateConfigJsonFileUrl(),getUpdateConfigJsonFile()+".down");
        });
        return;
    }
    m_localUpdateConfig.fromJsonFile(getUpdateConfigJsonFile());
    m_remoteUpdateConfig.fromJsonFile(getUpdateConfigJsonFile()+".down");
    if (m_localUpdateConfig.updatefirmware.version < m_remoteUpdateConfig.updatefirmware.version
        && QUrl(m_remoteUpdateConfig.updatefirmware.url).isValid() && !m_remoteUpdateConfig.updatefirmware.filemd5.isEmpty())
    {
        if (!m_downloadUpdatefirmLua)
        {
            m_downloadUpdatefirmLua = new DownloadTool();
            connect(m_downloadUpdatefirmLua,&DownloadTool::sigDownloadFinished,this,&MainWidget2::slot_downloadUpdatefirmLuaFinished);
            m_downloadUpdatefirmLua->startDownloadFile(m_remoteUpdateConfig.updatefirmware.url
                                                          ,getUpdateFirmwareLuaFile()+".down");
        }
    }
    if (m_localUpdateConfig.startServer.version < m_remoteUpdateConfig.startServer.version
        && QUrl(m_remoteUpdateConfig.startServer.url).isValid() && !m_remoteUpdateConfig.startServer.filemd5.isEmpty())
    {
        if (!m_downloadStarserverTool)
        {
            m_downloadStarserverTool = new DownloadTool();
            connect(m_downloadStarserverTool,&DownloadTool::sigDownloadFinished,this,&MainWidget2::slot_downloadStartserverToolFinished);
            m_downloadStarserverTool->startDownloadFile(m_remoteUpdateConfig.startServer.url
                                                          ,getStartServerZipFile()+".down");
        }
    }
}

void MainWidget2::slot_downloadUpdatefirmLuaFinished(bool ok)
{
    const QString strDownFile = getUpdateFirmwareLuaFile()+".down";
    qDebug()<<"updatefirmware.lua download ---------------------------------------------->"<<ok;
    if (!ok)
    {//下载失败，则重新下载
        QTimer::singleShot(2000, this, [this,strDownFile]{
            m_downloadUpdatefirmLua->startDownloadFile(m_remoteUpdateConfig.updatefirmware.url,strDownFile);
        });
        return;
    }
    CMd5 md5;
    const QString strMd5 = md5.GetFileMd5(strDownFile);
    if (strMd5 != m_remoteUpdateConfig.updatefirmware.filemd5)
    {//下载失败了，重新下载
        qDebug()<<"updatefirmware.lua 下载失败，因为md5值校验不对";
        QTimer::singleShot(2000, this, [this,strDownFile]{
            m_downloadUpdatefirmLua->startDownloadFile(m_remoteUpdateConfig.updatefirmware.url,strDownFile);
        });
        return;
    }

    bool bUpdateOk = false;
    const QString strFile = getUpdateFirmwareLuaFile();
    if (QFile::exists(strFile))
    {
        if (QFile::remove(strFile) && QFile::rename(strDownFile,strFile))
        {
             bUpdateOk = true;
        }
    }
    else
    {
        if (QFile::rename(strDownFile,strFile))
        {
            bUpdateOk = true;
        }
    }
    if (bUpdateOk)
    {
        m_localUpdateConfig.updatefirmware = m_remoteUpdateConfig.updatefirmware;
        QString strJson = m_localUpdateConfig.toJson();
        QFile file(getUpdateConfigJsonFile());
        if (file.open(QFile::Truncate|QFile::WriteOnly))
        {
            file.write(strJson.toUtf8());
            file.close();
        }
        if (CLuaApi::getInstance()->isLuaRun())
        {
            WidgetToast::ok(tr("脚本已更新，需要重新连接才生效"), 3000);
        }
        searchMenuItems();
    }
}

void MainWidget2::slot_downloadStartserverToolFinished(bool ok)
{
    const QString strDownFile = getStartServerZipFile()+".down";
    qDebug()<<"startServer.zip download ---------------------------------------------->"<<ok;
    if (!ok)
    {//下载失败，则重新下载
        QTimer::singleShot(2000, this, [this,strDownFile]{
            m_downloadStarserverTool->startDownloadFile(m_remoteUpdateConfig.startServer.url, strDownFile);
        });
        return;
    }
    CMd5 md5;
    const QString strMd5 = md5.GetFileMd5(strDownFile);
    if (strMd5 != m_remoteUpdateConfig.startServer.filemd5)
    {//下载失败了，重新下载
        qDebug()<<"startServer.zip 下载失败，因为md5值校验不对";
        QTimer::singleShot(2000, this, [this,strDownFile]{
            m_downloadStarserverTool->startDownloadFile(m_remoteUpdateConfig.startServer.url,strDownFile);
        });
        return;
    }

    bool bUpdateOk = false;
    const QString strFile = getStartServerZipFile();
    if (QFile::exists(strFile))
    {
        if (QFile::remove(strFile) && QFile::rename(strDownFile,strFile))
        {
             bUpdateOk = true;
        }
    }
    else
    {
        if (QFile::rename(strDownFile,strFile))
        {
            bUpdateOk = true;
        }
    }

    bUpdateOk = uncompressStartserverToolZip();
    m_localUpdateConfig.startServer = m_remoteUpdateConfig.startServer;
    QString strJson = m_localUpdateConfig.toJson();
    QFile file(getUpdateConfigJsonFile());
    if (file.open(QFile::Truncate|QFile::WriteOnly))
    {
        file.write(strJson.toUtf8());
        file.close();
    }

    if (bUpdateOk)
    {
        if (CLuaApi::getInstance()->isLuaRun())
        {
            auto pMsgBox = new MessageWidget(this);
            pMsgBox->setMessage("ok",tr("工具配置下载完毕，需要断开并重新连接设备才生效"));
            pMsgBox->setBtnText(tr("好的"));
            pMsgBox->show();
        }
    }
    else
    {
        auto pMsgBox = new MessageWidget(this);
        pMsgBox->setMessage("ok",tr("工具配置下载完毕，需要手动重启维护工具软件才生效"));
        pMsgBox->setBtnText(tr("好的"));
        pMsgBox->show();
    }
}

void MainWidget2::slot_downloadServoJointTableFinished(bool ok)
{
    qDebug()<<"ServoJointConfig.csv download ---------------------------------------------->"<<ok;
    if (!ok)
    {//下载失败，则重新下载
        QTimer::singleShot(3000, this, [this]{
            m_downloadServoJointTable->startDownloadFile(getServoTemplateJointFileUrl(),getServoTemplateJointFileZIP());
        });
        return;
    }
    const QString strFile = getServoTemplateJointFileZIP();
    ZipDiyManager zip;
    if(zip.zipUnCompress(strFile,getServoParamTemplateDir(),QString())){
        readServoJointTable();
    }else{
        qDebug()<<"ServoTemplateJointConfig.zip uncompress faile!";
    }
}

void MainWidget2::slot_downloadControllerScriptFinished(bool ok)
{
    qDebug()<<"ControllerScriptFile.zip download ---------------------------------------------->"<<ok;
    if (!ok)
    {//下载失败，则重新下载
        static constexpr int itimeout = 10*60*1000; //分钟
        QTimer::singleShot(itimeout, this, [this]{
            m_downloadControllerScript->startDownloadFile(getControllerScriptFileUrl(),getControllerScriptFileZIP());
        });
        return;
    }
    const QString strFile = getControllerScriptFileZIP();
    ZipDiyManager zip;
    if(zip.zipUnCompress(strFile,getControllerScriptDir(),QString())){
    }else{
        qDebug()<<"ControllerScriptFile.zip uncompress faile!";
    }
}

void MainWidget2::readServoJointTable()
{
/*csv文件格式为：
厂家(Vendor),型号(Version),文件名(Prefix of Name)
7,1,zznew-01
3,11,new32-01
*/
//第一行为表头，其他行为数字，共3列
    const QString strFile = getServoJointTableFile();
    QList<QStringList> sjt = parseCSVFile(strFile);
    if (sjt.size()<2) return;
    m_servoJointTable.clear();

    sjt.removeFirst(); //删掉第一行表头
    for (int i=0; i<sjt.size(); ++i){
        auto v = sjt.at(i);
        if (v.size()<2) {
            qDebug()<<strFile<<" row="<<i<<" less then 2 ================================================";
            continue;
        }
        QString strKey = v[0]+","+v[1];
        QString strVal;
        if (v.size()>2) strVal = v[2];
        m_servoJointTable.insert(strKey,strVal);
    }
}

QPair<bool,QString> MainWidget2::getServoJointTable(int iChangJia, int iDianJiXingHao)
{
    QString str = QString::asprintf("%d,%d",iChangJia, iDianJiXingHao);
    auto itr = m_servoJointTable.find(str);
    if (itr != m_servoJointTable.end()){
        return qMakePair<bool,QString>(true, itr.value());
    }
    return qMakePair<bool,QString>(false, "");
}

bool MainWidget2::uncompressStartserverToolZip()
{
    const QString strFile = getStartServerZipFile();
    if (!QFile::exists(strFile)) return false;
    const QString dstZipPath = getStartServerZipFileDir();

    QDir destDir;
    const QString strServerToolDir = dstZipPath+"startServer";
    const QString strServerToolDirBack = strServerToolDir+"bak";
    qDebug()<<"begin uncompress start server tool zip file:"<<strFile;
    if (destDir.exists(strServerToolDir))
    {
        if (!destDir.rename(strServerToolDir,strServerToolDirBack))
        {
            qDebug()<<"begin uncompress start server tool zip file fail,because rename fail";
            return false;
        }
    }
    ZipDiyManager zip;
    if(zip.zipUnCompress(strFile,dstZipPath,QString())){
        QFile::remove(strFile);
        destDir.setPath(strServerToolDirBack);
        destDir.removeRecursively();
        destDir.rmpath(destDir.path());
        return true;
    }else{//解压失败则需要恢复原来的目录
        qDebug()<<"begin uncompress start server tool zip file fail";
        destDir.setPath(strServerToolDir);
        destDir.removeRecursively();
        destDir.rmpath(destDir.path());
        destDir.rename(strServerToolDirBack,strServerToolDir);
        return false;
    }
}

void MainWidget2::slot_startDiagnose()
{
    ui->btnDiagnose->hide();
    ui->listWidgetFirmwareCheck->addItem(m_item0);
    ui->listWidgetFirmwareCheck->setItemWidget(m_item0,m_firmwareCheckItem0);
    ui->listWidgetFirmwareCheck->addItem(m_itemDns);
    ui->listWidgetFirmwareCheck->setItemWidget(m_itemDns,m_firmwareCheckItemDns);
    ui->listWidgetFirmwareCheck->addItem(m_itemMacAddress);
    ui->listWidgetFirmwareCheck->setItemWidget(m_itemMacAddress,m_firmwareCheckItemMacAddress);

    m_firmwareCheckItem0->setMainThemeStatus(1);

    m_firmwareCheckItemDns->setLabelStatus(0);
    m_firmwareCheckItemDns->setThemeStatus(1);

    //    if(m_httpProtocol->postFaultCheck(g_strCurrentIP,"dns")=="success"){
    //        m_firmwareCheckItemDns->setThemeStatus(2);
    //    }else{
    //        m_firmwareCheckItemDns->setThemeStatus(-1);
    //        m_firmwareCheckItemDns->setLabelStatus(10);
    //        ui->btnAllRepair->show();
    //    }

    m_firmwareCheckItemMacAddress->setLabelStatus(0);
    m_firmwareCheckItemMacAddress->setThemeStatus(1);

    //    if(m_httpProtocol->postFaultCheck(g_strCurrentIP,"mac")=="success"){
    //        m_firmwareCheckItemMacAddress->setThemeStatus(2);
    //    }else{
    //        m_firmwareCheckItemMacAddress->setThemeStatus(-1);
    //        m_firmwareCheckItemMacAddress->setLabelStatus(10);
    //        ui->btnAllRepair->show();
    //    }

    ui->btnRestartDiagnose->show();
    m_firmwareCheckItem0->setMainThemeStatus(2);
}

void MainWidget2::slot_cSFtpClientFinishedJob(qint64 id, const QList<QSsh::SftpFileInfo> &fileInfoList, bool bOk, QString strErrMsg)
{
    qDebug()<<"=========================================================slot_cSFtpClientFinishedJob:"<<id;
    if(!bOk){
        qDebug()<<" slot_cSFtpClientFinishedJob "<<strErrMsg;
    }
    if(id == 101){
        SFtpOptFile opt;
        opt.id = 102;
        opt.strRemotePath = "/dobot/userdata/project/update/files";
        opt.strLocalPath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/files";
        opt.opt = SFtpOption::OPT_LISTDIR;
        m_csFtpClient->ssh(opt);
    }
    if(id == 103){
        if(CommonData::getCurrentSettingsVersion().control.contains("V3.5.1")){
            qDebug()<<"3524以下 V3.5.1  "<<CommonData::getCurrentSettingsVersion().control;
            wigdet2ControllCompatibleUpdateDiskKernel();
        }else if(CommonData::getCurrentSettingsVersion().control.contains("V3.5.2")){
            if(CommonData::getCurrentSettingsVersion().control.mid(7,1).toInt() <= 4){
                qDebug()<<"3524以下  "<<CommonData::getCurrentSettingsVersion().control;
                wigdet2ControllCompatibleUpdateDiskKernel();
            }else{
                qDebug()<<"3524以上  3530以下 不包含3530 直接升级";
                widget2UpgradeFileCopyToControlFinish(true);
            }
        }else{
            qDebug()<<" widget2UpgradeFileCopyToControlFinish not 3541 351  ";
            widget2UpgradeFileCopyToControlFinish(true);
        }
    }
    else if(id == SystemLogExport_OPT_LISTDIR){
        QStringList projectList;
        for(QSsh::SftpFileInfo sftpFileInfo : fileInfoList){
            if(sftpFileInfo.name=="."||sftpFileInfo.name==".."){
                continue;
            }
            if(sftpFileInfo.type == QSsh::SftpFileType::FileTypeDirectory){
                projectList.append(sftpFileInfo.name);
            }
        }
        auto ptr = new WidgetSystemLogExport(this);
        ptr->setGeometry(0,0,width(),height());
        connect(ptr,&WidgetSystemLogExport::signal_systemLogExport,this,&MainWidget2::slot_SystemLogExport);
        ptr->setProjectList(projectList);
        ptr->show();
    }
}

void MainWidget2::slot_singleFunc()
{
    if (m_bIsCheckingServoParam)
    {
        WidgetToast::warn(tr("请等待伺服参数对比完成"), 2000);
        return ;
    }
    qDebug()<<"m_isUpgradeSuccessStatus  "<<m_isUpgradeSuccessStatus;
    if(m_isUpgradeSuccessStatus == 2)
    {
        auto* pWndMsg = new MessageWidget(this);
        pWndMsg->setMessage("warn",tr("如需使用高级功能，请重启后操作。"));
        pWndMsg->show();
        return;
    }

    m_widgetSingleUpgrade->searchMenuItems();
    ui->stackedWidget->setCurrentIndex(5);
}

void MainWidget2::slot_backToUpgrade()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWidget2::slot_upgradeSingleFwOrXML(QStringList allUpdateFilePath, QStringList allUpdateFile)
{
    auto fileValid = CLuaApi::getInstance()->isSingleUpdrageFilesValid(m_strLabelSingleFwAndXML,allUpdateFile);
    if (fileValid.hasValue()){
        bool b = fileValid.value().first;
        QString str = fileValid.value().second;
        if (!b){
            if (str.isEmpty()){
                str = tr("请检查要升级的固件");
            }
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",str);
            pWndMsg->show();
            return;
        }
    }else{
        for (int i=0; i<allUpdateFile.size(); ++i){
            QString strUpdateFile = allUpdateFile.at(i);
            if(!m_strLabelSingleFwAndXML.contains("XML")&&!strUpdateFile.contains("FOE"))
            {
                auto* pWndMsg = new MessageWidget(this);
                pWndMsg->setMessage("warn",tr("请检查要升级的固件"));
                pWndMsg->show();
                return;
            }
            if(m_strLabelSingleFwAndXML.contains("XML")&&!strUpdateFile.contains("XML"))
            {
                auto* pWndMsg = new MessageWidget(this);
                pWndMsg->setMessage("warn",tr("请检查要升级的XML"));
                pWndMsg->show();
                return;
            }
        }
    }

    m_widgetSingleUpgrading->setGeometry(0,0,width(),height());
    m_widgetSingleUpgrading->setSingleUpgradeStatus(1);
    m_widgetSingleUpgrading->show();
    QStringList projectList = allUpdateFilePath;
    m_allUpdateFile = allUpdateFile;
    {//先删除目录内容
        QEventLoop procEvt;
        QProcess proc;
        proc.setProgram(QCoreApplication::applicationDirPath()+"/tool/plink/plink.exe");
        proc.setArguments(QStringList()<<"-ssh"<<"-l"<<"root"<<"-pw"<<"dobot"<<g_strCurrentIP<<QString("rm -rf %1;mkdir %1").arg(m_strProtocolVersionSlaveFiles));
        qDebug()<<"slot_upgradeSingleFwOrXML rm -rf;mkdir:"<<proc.arguments();
        connect(&proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,[&procEvt](int exitCode, QProcess::ExitStatus exitStatus){
            qDebug()<<"[slot_upgradeSingleFwOrXML::execCmd-finished]exitCode="<<exitCode<<",exitStatus="<<exitStatus;
            procEvt.quit();
        });
        proc.start();
        procEvt.exec();
    }

    m_pFileSingleTransfer->setProgram(QCoreApplication::applicationDirPath()+"/tool/plink/pscp.exe");
    QStringList argsUpload;
    argsUpload<<"-pw"<<"dobot"<<"-r"<<projectList<<QString("root@%1:%2").arg(g_strCurrentIP,m_strProtocolVersionSlaveFiles);
    m_pFileSingleTransfer->setArguments(argsUpload);
    m_pFileSingleTransfer->start();
}

void MainWidget2::slot_csFtpClientOnChannelFileInfoAvailableFinish(qint64 job, const QList<QSsh::SftpFileInfo> &fileInfoList)
{
    qDebug()<<"=========================================================slot_csFtpClientOnChannelFileInfoAvailableFinish:"<<job;
    if(job == 102){
        QString currentServoVersion;
        if(ui->labelCurrentJ1Version->text().at(0)=='4'){
            currentServoVersion = "V4";
        }else if(ui->labelCurrentJ1Version->text().at(0)=='3'){
            currentServoVersion = "V3";
        }else{
            return;
        }

        for(QSsh::SftpFileInfo sftpFileInfo : fileInfoList){
            if(sftpFileInfo.name.contains("Servo")){
                m_controllServoList.append(sftpFileInfo.name);
                if(!sftpFileInfo.name.contains(currentServoVersion)){
                    if(!sftpFileInfo.name.contains("XML")){
                        qDebug()<<"  sssffffff "<<sftpFileInfo.name;
                        SFtpOptFile opt;
                        opt.id = 103;
                        opt.strRemotePath = "/dobot/userdata/project/update/files/"+sftpFileInfo.name;
                        opt.strLocalPath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/files";
                        opt.opt = SFtpOption::OPT_REMOVEFILE;
                        m_csFtpClient->ssh(opt);
                    }
                }
            }
        }
    }
}

void MainWidget2::showServoParamWidget()
{
    ui->stackedWidget->setCurrentIndex(6);
    ui->stackedWidget->repaint();
    ui->widgetBackup->setStyleSheet("border:none;");
    ui->widgetFirmwareUpgrade->setStyleSheet("border:none;");
    ui->widgetSystemTool->setStyleSheet("border:none;");
    ui->widgetFirmwareCheck->setStyleSheet("border:none;");
    ui->widgetServoParams->setStyleSheet("border:none;");

    ui->btnFirmwareUpgrade->setChecked(false);
    ui->btnSystemTool->setChecked(false);
    ui->btnFirmwareCheck->setChecked(false);
    ui->btnBackup->setChecked(false);
    ui->btnServoParams->setChecked(false);

    ui->widgetServoParams->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");

    ui->btnServoParams->setChecked(true);
    ui->widgetFuncSelection->repaint();
}

QString MainWidget2::getCurrentUseAddress()
{
    auto all = m_macAddressSetting.childGroups();
    if (all.isEmpty()) return "";
    QString group = all[0];
    m_macAddressSetting.beginGroup(group);
    QString startAddress = m_macAddressSetting.value("start").toString();
    QString currentUse = m_macAddressSetting.value("current").toString();
    QString endAddress = m_macAddressSetting.value("end").toString();
    int intCurrentUse = currentUse.toInt(nullptr,16);
    int intEndAddress = endAddress.toInt(nullptr,16);
    if(intCurrentUse>intEndAddress){
        m_macAddressSetting.setValue("current",startAddress);
    }else{
        m_macAddressSetting.setValue("current",QString::number(intCurrentUse+1,16).toUpper());
    }
    qDebug()<<" intCurrentUse "<<intCurrentUse;
    qDebug()<<" intEndAddress "<<intEndAddress;
    qDebug()<<"currentUse  "<<currentUse;
    m_macAddressSetting.endGroup();
    return currentUse;
}

QString MainWidget2::paramIndex2Address(QString strParaIndStr)
{
    strParaIndStr.remove(QChar('H'), Qt::CaseInsensitive);
    QStringList addressList = strParaIndStr.split(".");
    int address0 = addressList.at(0).toInt(nullptr, 16)+0x2000;
    int address1 = addressList.at(1).toInt()+1;
    //由于get 的时候 返回的是 小写字母  需转换成 大写字母
    QString str = QString::asprintf("%X%02d",address0,address1);
    return str;
}

QString MainWidget2::address2ParamIndex(QString address)
{
    //address=addr200d02固定格式和长度
    constexpr static int iLen = 10;
    if (!address.startsWith("addr20") || address.length()!=iLen){
        return address;
    }
    QString add1 = address.mid(6,2).toUpper();
    int iaddr2 = address.mid(8,2).toInt()-1;
    QString str = QString::asprintf("%02d",iaddr2);
    return QString("H%1.%2").arg(add1).arg(str);
}

void MainWidget2::LangeuageFontChange(bool isEnglish)
{
    QFont f(qApp->font());

    if(isEnglish)
    {
        f.setFamily("Arial");
    }
    else
    {
        f.setFamily("Microsoft YaHei UI");
    }
    qApp->setFont(f);
    //modifyStyle(this,arg1 == Qt::CheckState::Checked);
    foreach (QWidget *widget, QApplication::allWidgets())
    {
        QFont f(widget->font());
        if(isEnglish)
        {
            f.setFamily("Arial");
        }
        else
        {
            f.setFamily("Microsoft YaHei UI");
        }
        widget->setFont(f);
    }
}

double MainWidget2::dotDividedBitsValue(bool isReadMode, int dotDividedBits, double value)
{
    qDebug()<<" dotDividedBits "<<dotDividedBits;
    if(dotDividedBits == 0)
    {
        return value;
    }
    int multipleNum = 1;
    if(dotDividedBits == 1)
    {
        multipleNum = 10;
    }
    if(dotDividedBits == 2)
    {
        multipleNum = 100;
    }
    if(dotDividedBits == 3)
    {
        multipleNum = 1000;
    }
    if(isReadMode)
    {
        value = value/multipleNum;
    }
    else
    {
        value = value*multipleNum;
    }
    return value;
}

void MainWidget2::delegateServoParamsTable()
{
    m_singleIsCheckDelegate = new IsCheckDelegate();
    m_myCheckBoxHeader = new MyCheckBoxHeader(Qt::Horizontal, ui->tableServoParamView);
    connect(m_myCheckBoxHeader,&MyCheckBoxHeader::checkBoxClicked,this,[&](bool isTrue){
        for(int row = 0;row < mServoParamsTableModel->rowCount();row++)
        {
            QStandardItem* item = mServoParamsTableModel->item(row,colIsCheck);
            if(item == nullptr)
            {
                continue;
            }
            if(isTrue)
            {
                mServoParamsTableModel->setData(item->index(), true, Qt::UserRole);
            }
            else
            {
                mServoParamsTableModel->setData(item->index(), false, Qt::UserRole);
            }
        }
    });

    //如果某个选项取消勾选 此时需要取消头部勾选
    connect(m_singleIsCheckDelegate,&IsCheckDelegate::signalIsCheckHeader,this,[&](bool data){
        if(!data)
        {
            m_myCheckBoxHeader->setIsChecked(data);
        }
    });

    QString sheet = ui->tableServoParamView->horizontalHeader()->styleSheet();
    qDebug()<<" sheet--> "<<sheet;
    ui->tableServoParamView->setHorizontalHeader(m_myCheckBoxHeader);
    //    m_myCheckBoxHeader->setStyleSheet(" QHeaderView::section {border: none;  }");

    //单独写入
    m_singleReadAWriteDelegate = new ReadAWriteBtnDelegate();
    connect(m_singleReadAWriteDelegate,&ReadAWriteBtnDelegate::startSingleWrite,this,[&](int row)
    {
        QStandardItem* item = mServoParamsTableModel->item(row,colIsReadOnly);
        if(item == nullptr || item->text() == g_strServoParamsColValueTrue){
            qDebug()<<"m_singleWriteDelegate only read  ";
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("该列只允许读，不允许写"));
            pWndMsg->show();
            return;
        }

        QString paramIndex = item->text();

        QStandardItem* addressItem = mServoParamsTableModel->item(row,colParaIndex);
        QString address = paramIndex2Address(addressItem->text());
        DobotType::StructSettingsServoParams settingsServoParams;
        DobotType::StructServoParam servoParam;
        servoParam.servoNum = ui->comboServo->currentText();
        servoParam.key = "addr"+address;

        QModelIndex valueIndex = mServoParamsTableModel->index(row, colValue, QModelIndex());
        double value = mServoParamsTableModel->data(valueIndex).toDouble();
        QModelIndex dotDividedBitsIndex = mServoParamsTableModel->index(row, colDotDividedBits, QModelIndex());
        int dotDividedBits = mServoParamsTableModel->data(dotDividedBitsIndex).toInt();
        double writeValue = dotDividedBitsValue(false,dotDividedBits,value);
        servoParam.value = writeValue;

        settingsServoParams.src = "httpClient";
        settingsServoParams.servoParams.append(servoParam);
        bool result = m_httpProtocol->postSettingsModifyServoParams(g_strCurrentIP,settingsServoParams);
        if(result == true)
        {
            sleep(300);
            DobotType::StructSettingsServoParamsResult servoParamsResult = m_httpProtocol->getSettingsModifyServoParams(g_strCurrentIP);
            if(servoParamsResult.servoParams.isEmpty())
            {
                qDebug()<<"m_singleWriteDelegate  getSettingsModifyServoParams servoParams.isEmpty() failed ";
                auto pTipsWnd = new ServoParamBubbleTipWidget();
                pTipsWnd->move(this->pos().x()+width()/2-pTipsWnd->width()/2,this->pos().y()+height()/2-pTipsWnd->height()/2-150);
                pTipsWnd->setContent(tr("写入失败"));
                pTipsWnd->setDisabled(true);
                pTipsWnd->show();
            }
            else if(servoParamsResult.servoParams[0].status != true)
            {
                qDebug()<<"m_singleWriteDelegate  getSettingsModifyServoParams failed ";
                auto pTipsWnd = new ServoParamBubbleTipWidget();
                pTipsWnd->move(this->pos().x()+width()/2-pTipsWnd->width()/2,this->pos().y()+height()/2-pTipsWnd->height()/2-150);
                pTipsWnd->setContent(tr("写入失败"));
                pTipsWnd->setDisabled(true);
                pTipsWnd->show();
            }
            else
            {
                qDebug()<<"m_singleWriteDelegate  getSettingsModifyServoParams success ";
                auto pTipsWnd = new ServoParamBubbleTipWidget();
                pTipsWnd->setContent(tr("写入成功"));
                pTipsWnd->move(this->pos().x()+width()/2-pTipsWnd->width()/2,this->pos().y()+height()/2-pTipsWnd->height()/2-150);
                pTipsWnd->show();
            }
        }
        else
        {
            qDebug()<<"m_singleWriteDelegate  postSettingsModifyServoParams success ";
            auto pTipsWnd = new ServoParamBubbleTipWidget();
            pTipsWnd->move(this->pos().x()+width()/2-pTipsWnd->width()/2,this->pos().y()+height()/2-pTipsWnd->height()/2-150);
            pTipsWnd->setContent(tr("写入失败"));
            pTipsWnd->setDisabled(true);
            pTipsWnd->show();
        }
    },Qt::QueuedConnection);

    connect(m_singleReadAWriteDelegate,&ReadAWriteBtnDelegate::startSingleRead, this, [&](int row){
        ui->pageServoParams->setDisabled(true);
        QStandardItem* item = mServoParamsTableModel->item(row,colParaIndex);
        if(item == nullptr || item->text().isEmpty())
        {
            qDebug()<<"m_singleReadDelegate item->text().isEmpty() ";
            return;
        }
        QString paramIndex = item->text();
        qDebug()<<"m_singleReadDelegate paramIndex  "<<paramIndex;
        QString address = paramIndex2Address(paramIndex);
        DobotType::StructSettingsServoParams settingsServoParams;
        DobotType::StructServoParam servoParam;
        servoParam.servoNum = ui->comboServo->currentText();
        servoParam.key = "addr"+address;
        settingsServoParams.servoParams.append(servoParam);

        bool isSuccess = m_httpProtocol->postSettingsReadServoParams(g_strCurrentIP,settingsServoParams);
        if(!isSuccess)
        {
            auto pTipsWnd = new ServoParamBubbleTipWidget();
            pTipsWnd->setContent(tr("读取失败"));
            pTipsWnd->setDisabled(true);
            pTipsWnd->move(this->pos().x()+width()/2-pTipsWnd->width()/2,this->pos().y()+height()/2-pTipsWnd->height()/2-150);
            pTipsWnd->show();
        }else
        {
            sleep(300);
            DobotType::StructSettingsServoParamsResult servoParamsResult = m_httpProtocol->getSettingsReadServoParams(g_strCurrentIP);
            if(servoParamsResult.status == true)
            {
                if(servoParamsResult.servoParams.isEmpty())
                {
                    qDebug()<<" m_singleReadDelegate servoParams.isEmpty() ";
                    auto pTipsWnd = new ServoParamBubbleTipWidget();
                    pTipsWnd->setContent(tr("读取失败"));
                    pTipsWnd->setDisabled(true);
                    pTipsWnd->move(this->pos().x()+width()/2-pTipsWnd->width()/2,this->pos().y()+height()/2-pTipsWnd->height()/2-150);
                    pTipsWnd->show();
                }
                else if(servoParamsResult.servoParams[0].status == true)
                {
                    double value = servoParamsResult.servoParams[0].value;
                    qDebug()<<"m_singleReadDelegate value  "<<value;
                    QModelIndex dotDividedBitsIndex = mServoParamsTableModel->index(row, colDotDividedBits, QModelIndex());
                    int dotDividedBits = mServoParamsTableModel->data(dotDividedBitsIndex).toInt();
                    double viewValue = dotDividedBitsValue(true,dotDividedBits,value);

                    QModelIndex index = mServoParamsTableModel->index(row, colValue, QModelIndex());
                    mServoParamsTableModel->setData(index,viewValue, Qt::EditRole);
                    auto pTipsWnd = new ServoParamBubbleTipWidget();
                    pTipsWnd->setContent(tr("读取成功"));
                    pTipsWnd->move(this->pos().x()+width()/2-pTipsWnd->width()/2,this->pos().y()+height()/2-pTipsWnd->height()/2-150);
                    pTipsWnd->show();
                }
                else
                {
                    qDebug()<<" m_singleReadDelegate servoParamsResult.servoParams[0].status == false ";
                    auto pTipsWnd = new ServoParamBubbleTipWidget();
                    pTipsWnd->setContent(tr("读取失败"));
                    pTipsWnd->setDisabled(true);
                    pTipsWnd->move(this->pos().x()+width()/2-pTipsWnd->width()/2,this->pos().y()+height()/2-pTipsWnd->height()/2-150);
                    pTipsWnd->show();
                }
            }
            else
            {
                qDebug()<<" getSettingsReadServoParams Failed ";
                auto pTipsWnd = new ServoParamBubbleTipWidget();
                pTipsWnd->setContent(tr("读取失败"));
                pTipsWnd->setDisabled(true);
                pTipsWnd->move(this->pos().x()+width()/2-pTipsWnd->width()/2,this->pos().y()+height()/2-pTipsWnd->height()/2-150);
                pTipsWnd->show();
            }
        }
        ui->pageServoParams->setDisabled(false);
    },Qt::QueuedConnection);
}

void MainWidget2::importServoParamsExcel(QString filePath)
{
    //每次导入新模板需初始化一下表格数量
    mServoParamsTableModel->setRowCount(modelRowMax);
    if(filePath.isEmpty()){
        return;
    }
    QStringList servoReadOnlyList;
    QFile servoAddressfile(QCoreApplication::applicationDirPath()+"/config/servoReadOnlyAddress.txt");
    if(servoAddressfile.exists())
    {
        if (servoAddressfile.open(QIODevice::ReadOnly))
        {
            QTextStream servoParamstream_text(&servoAddressfile);
            while (!servoParamstream_text.atEnd())
            {
                servoReadOnlyList.push_back(servoParamstream_text.readLine());
            }
        }
    }
    QFile inFile(filePath);
    QStringList lines;/*行数据*/

    int rowCount = 0;
    m_servoParamsCount = 0;
    if (inFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream_text(&inFile);
        stream_text.setCodec(checkFileTextCode(&inFile));
        while (!stream_text.atEnd())
        {
            lines.push_back(stream_text.readLine());
        }
        rowCount = lines.size();
        m_servoParamsCount = rowCount;
        QStringList headDataList = lines.at(1).split(",");

        //导入某个关节的.csv
        if(headDataList.size() <= 5)
        {
            if(headDataList[3] == "ParaValue")
            {
                for(int row = 2;row < lines.size();row++)
                {
                    QStringList list = lines[row].split(",");
                    QStringList valueList = list[3].split(":");
                    qDebug()<<" row list[3]  valueList[0]"<< valueList[0];
                    QModelIndex index = mServoParamsTableModel->index(row-2, colValue, QModelIndex());
                    mServoParamsTableModel->setData(index, valueList[0], Qt::EditRole);
                }
                //导入关节轴数据后重新设置表格数量
                mServoParamsTableModel->setRowCount(lines.size()-2);
                ui->tableServoParamView->repaint();
            }
            else
            {
                auto* pWndMsg = new MessageWidget(this);
                pWndMsg->setMessage("warn",tr("请导入正确的表格"));
                pWndMsg->show();
            }
            return;
        }

        for (int col = 1; col < headDataList.size(); col++)
        {
            mServoParamsTableModel->setHeaderData(col+1, Qt::Horizontal, headDataList.at(col), Qt::EditRole);
        }

        for (int row = 2, modelRow = 0; row < lines.size(); row++,modelRow++)
        {
            QString line = lines.at(row);
            QStringList split = line.split(",");/*列数据*/
            if(split[0].contains("TRUE")||split[0].contains("FALSE"))
            {
            }
            else{
                modelRow--;
                continue;
            }
            //只读的选项设置为true，否则为false
            if(split[colIsReadOnly-1]=="TRUE")
            {
                QModelIndex indexWirte = mServoParamsTableModel->index(modelRow, colReadAWirte, QModelIndex());
                mServoParamsTableModel->setData(indexWirte, QVariant(true), Qt::EditRole);
            }
            else
            {
                QModelIndex indexWirte = mServoParamsTableModel->index(modelRow, colReadAWirte, QModelIndex());
                mServoParamsTableModel->setData(indexWirte, QVariant(false), Qt::EditRole);
            }
            if(servoReadOnlyList.contains(split[colParaIndex-1]))
            {
                QModelIndex indexWirte = mServoParamsTableModel->index(modelRow, colReadAWirte, QModelIndex());
                mServoParamsTableModel->setData(indexWirte, QVariant(true), Qt::EditRole);
            }
            for (int col = 0; col < split.size(); col++)
            {

                QString value = split.at(col);
                //由于第一列为按钮，上位机表格显示数据需要往右移动一列
                int modelCol = col+1;
                QModelIndex index = mServoParamsTableModel->index(modelRow, modelCol, QModelIndex());

                if(modelCol == colValue)
                {
                    value = value.split(":")[0];
                    mServoParamsTableModel->setData(index, value, Qt::EditRole);
                }
                else
                {
                    if(modelCol == colIsCheck)
                    {
                        value = "";
                    }
                    mServoParamsTableModel->setData(index, value, Qt::DisplayRole);
                }
            }
            m_servoParamsCount = modelRow+1;
        }
        inFile.close();
    }
    else
    {
        return;
    }
    qDebug()<<" m_servoParamsCount "<<m_servoParamsCount;
    mServoParamsTableModel->setRowCount(m_servoParamsCount);
    mServoParamsTableModel->setHeaderData(0, Qt::Horizontal,"操作", Qt::EditRole);
    ui->tableServoParamView->setItemDelegateForColumn(colIsCheck, m_singleIsCheckDelegate);
    ui->tableServoParamView->setItemDelegateForColumn(colReadAWirte, m_singleReadAWriteDelegate);
    ui->tableServoParamView->setColumnWidth(0, 120);
    ui->tableServoParamView->setColumnWidth(1, 60);
    ui->tableServoParamView->setColumnWidth(2, 120);
    ui->tableServoParamView->setColumnWidth(4, 80);

    //清空上次所选的PID参数
    for(int i : m_singleReadAWriteDelegate->m_pidWidgetList)
    {
        for(int j = 0; j < mServoParamsTableModel->columnCount();j++)
        {
            QStandardItem* itemJ = mServoParamsTableModel->item(i,j);
            if (!itemJ) continue;
            itemJ->setBackground(QBrush(QColor(255,255,255)));
        }
    }
    m_singleReadAWriteDelegate->m_pidWidgetList.clear();
    //获取 0800~0805功能码 设置这几行的背景色
    for(int i = 0; i < mServoParamsTableModel->rowCount();i++)
    {
        QStandardItem* item = mServoParamsTableModel->item(i,colParaIndex);
        if(item == nullptr || item->text().isEmpty())
        {
            qDebug()<<"exportServoParamsFile item->text().isEmpty() ";
            continue;
        }
        QString paramIndex = item->text();
        int intParamIndex = paramIndex2Address(paramIndex).toInt();
        qDebug()<<" intParamIndex "<<intParamIndex;
        if(intParamIndex>200800&&intParamIndex<=200806)
        {
            m_singleReadAWriteDelegate->signal_renderPIDWidget(i);
            for(int j = 0; j < mServoParamsTableModel->columnCount();j++)
            {
                QStandardItem* itemJ = mServoParamsTableModel->item(i,j);
                if (!itemJ) continue;
                itemJ->setBackground(QBrush(QColor(0, 71, 187,12)));
            }
        }
    }
    ui->tableServoParamView->repaint();
}

void MainWidget2::widgetSingleFunc()
{
    ui->labelSingleSafeIO->hide();
    ui->textSingleSafeIO->hide();
    ui->textSingleSafeIOXML->hide();
    ui->btnSingleSafeIO->hide();
    ui->btnSingleSafeIOXML->hide();
    ui->lineSingleSafeIO->hide();

    ui->labelSingleFeedback->hide();
    ui->textSingleFeedback->hide();
    ui->btnSingleFeedback->hide();
    ui->lineSingleFeedback->hide();

    ui->labelSingleUniIO->hide();
    ui->textSingleUniIO->hide();
    ui->textSingleUniIOXML->hide();
    ui->btnSingleUniIO->hide();
    ui->btnSingleUniIOXML->hide();
    ui->lineSingleUniIO->hide();

    ui->labelSingleIOFW->hide();
    ui->textSingleCCBOX->hide();
    ui->textSingleCCBOXXML->hide();
    ui->btnSingleIOFW->hide();
    ui->btnSingleIOFWXML->hide();
    ui->lineSingleIOFW->hide();

    ui->labelSingleServoJ2->hide();
    ui->textSingleServoJ2->hide();
    ui->textSingleServoJ2XML->hide();
    ui->btnSingleServoJ2->hide();
    ui->btnSingleServoJ2XML->hide();

    ui->labelSingleServoJ3->hide();
    ui->textSingleServoJ3->hide();
    ui->textSingleServoJ3XML->hide();
    ui->btnSingleServoJ3->hide();
    ui->btnSingleServoJ3XML->hide();

    ui->labelSingleServoJ4->hide();
    ui->textSingleServoJ4->hide();
    ui->textSingleServoJ4XML->hide();
    ui->btnSingleServoJ4->hide();
    ui->btnSingleServoJ4XML->hide();

    ui->labelSingleServoJ5->hide();
    ui->textSingleServoJ5->hide();
    ui->textSingleServoJ5XML->hide();
    ui->btnSingleServoJ5->hide();
    ui->btnSingleServoJ5XML->hide();

    ui->labelSingleServoJ6->hide();
    ui->textSingleServoJ6->hide();
    ui->textSingleServoJ6XML->hide();
    ui->btnSingleServoJ6->hide();
    ui->btnSingleServoJ6XML->hide();

    ui->labelSingleTerminal->hide();
    ui->textSingleTerminal->hide();
    ui->textSingleTerminalXML->hide();
    ui->btnSingleTerminal->hide();
    ui->btnSingleTerminalXML->hide();
    ui->lineSingleTerminal->hide();

    ui->btnServoAll6J->setChecked(false);

    if(CommonData::getStrPropertiesCabinetType().name.contains("CCBOX")){
        ui->labelSingleIOFW->show();
        ui->textSingleCCBOX->show();
        ui->textSingleCCBOXXML->show();
        ui->btnSingleIOFW->show();
        ui->btnSingleIOFWXML->show();
        ui->lineSingleIOFW->show();

        ui->labelSingleTerminal->show();
        ui->textSingleTerminal->show();
        ui->textSingleTerminalXML->show();
        ui->btnSingleTerminal->show();
        ui->btnSingleTerminalXML->show();
        ui->lineSingleTerminal->show();
    }else if(CommonData::getStrPropertiesCabinetType().name.contains("CC")){
        ui->labelSingleSafeIO->show();
        ui->textSingleSafeIO->show();
        ui->textSingleSafeIOXML->show();
        ui->btnSingleSafeIO->show();
        ui->btnSingleSafeIOXML->show();
        ui->lineSingleSafeIO->show();

        ui->labelSingleFeedback->show();
        ui->textSingleFeedback->show();
        ui->btnSingleFeedback->show();
        ui->lineSingleFeedback->show();

        ui->labelSingleUniIO->show();
        ui->textSingleUniIO->show();
        ui->textSingleUniIOXML->show();
        ui->btnSingleUniIO->show();
        ui->btnSingleUniIOXML->show();
        ui->lineSingleUniIO->show();

        ui->labelSingleTerminal->show();
        ui->textSingleTerminal->show();
        ui->textSingleTerminalXML->show();
        ui->btnSingleTerminal->show();
        ui->btnSingleTerminalXML->show();
        ui->lineSingleTerminal->show();

    }else if(CommonData::getStrPropertiesCabinetType().name.contains("MG400")){
    }else if(CommonData::getStrPropertiesCabinetType().name.contains("M1Pro")){
        ui->labelSingleServoJ2->show();
        ui->textSingleServoJ2->show();
        ui->textSingleServoJ2XML->show();
        ui->btnSingleServoJ2->show();
        ui->btnSingleServoJ2XML->show();

        ui->labelSingleServoJ3->show();
        ui->textSingleServoJ3->show();
        ui->textSingleServoJ3XML->show();
        ui->btnSingleServoJ3->show();
        ui->btnSingleServoJ3XML->show();

        ui->labelSingleServoJ4->show();
        ui->textSingleServoJ4->show();
        ui->textSingleServoJ4XML->show();
        ui->btnSingleServoJ4->show();
        ui->btnSingleServoJ4XML->show();

        //TODO 如果版本为1开头 没末端  版本为2开头 有末端
    }else if(CommonData::getStrPropertiesCabinetType().name.contains("E6")){
        ui->labelSingleUniIO->show();
        ui->textSingleUniIO->show();
        ui->textSingleUniIOXML->show();
        ui->btnSingleUniIO->show();
        ui->btnSingleUniIOXML->show();
        ui->lineSingleUniIO->show();
        ui->labelSingleTerminal->show();
        ui->textSingleTerminal->show();
        ui->textSingleTerminalXML->show();
        ui->btnSingleTerminal->show();
        ui->btnSingleTerminalXML->show();
        ui->lineSingleTerminal->show();
    }
    setHideServo6JWidget(true);
}

void MainWidget2::showFirmwareAdvanceUI()
{
    COptional<QHash<QString,bool>> ret = CLuaApi::getInstance()->getShowFirmAdvanceUI();
    if (!ret.hasValue()){
        cabinetTypeInfoShow();
        widgetSingleFunc();
        return ;
    }
    auto result = ret.value();
    if (result.empty()){
        cabinetTypeInfoShow();
        widgetSingleFunc();
        return ;
    }
    QHash<QString,QList<QWidget*>> allWidget;
    QList<QWidget*> widgets;
    //一键升级页面：主控板固件
    widgets.append(ui->labelCurrentMainControlTheme);
    widgets.append(ui->labelCurrentMainControlVersion);
    widgets.append(ui->labelUpgradeMainControlVersion);
    //高级功能/单个升级页面：主控板固件
    widgets.append(ui->labelSingleMain);
    widgets.append(ui->textSingleMainControl);
    widgets.append(ui->btnSingleMainControl);
    widgets.append(ui->lineSingleMainControl);
    allWidget.insert(QString("control"),widgets);
    widgets.clear();

    //一键升级页面：IO板固件
    widgets.append(ui->labelCurrentIOFirmwareTheme);
    widgets.append(ui->labelCurrentIOFirmwareVersion);
    widgets.append(ui->labelUpgradeIOFirmwareVersion);
    //高级功能/单个升级页面：IO板固件
    widgets.append(ui->labelSingleIOFW);
    widgets.append(ui->textSingleCCBOX);
    widgets.append(ui->btnSingleIOFW);
    widgets.append(ui->textSingleCCBOXXML);
    widgets.append(ui->btnSingleIOFWXML);
    widgets.append(ui->lineSingleIOFW);
    allWidget.insert(QString("ccboxio"),widgets);
    widgets.clear();

    //一键升级页面：安全IO固件
    widgets.append(ui->labelCurrentSafeIOTheme);
    widgets.append(ui->labelCurrentSafeIOVersion);
    widgets.append(ui->labelUpgradeSafeIOVersion);
    //高级功能/单个升级页面：安全IO固件
    widgets.append(ui->labelSingleSafeIO);
    widgets.append(ui->textSingleSafeIO);
    widgets.append(ui->btnSingleSafeIO);
    widgets.append(ui->textSingleSafeIOXML);
    widgets.append(ui->btnSingleSafeIOXML);
    widgets.append(ui->lineSingleSafeIO);
    allWidget.insert(QString("safeio"),widgets);
    widgets.clear();

    //一键升级页面：馈能板固件
    widgets.append(ui->labelCurrentFeedbackTheme);
    widgets.append(ui->labelCurrentFeedbackVersion);
    widgets.append(ui->labelUpgradeFeedbackVersion);
    //高级功能/单个升级页面：馈能板固件
    widgets.append(ui->labelSingleFeedback);
    widgets.append(ui->textSingleFeedback);
    widgets.append(ui->btnSingleFeedback);
    widgets.append(ui->lineSingleFeedback);
    allWidget.insert(QString("feedback"),widgets);
    widgets.clear();

    //一键升级页面：通用IO固件
    widgets.append(ui->labelCurrentUnIOTheme);
    widgets.append(ui->labelCurrentUnIOVersion);
    widgets.append(ui->labelUpgradeUnIOVersion);
    //高级功能/单个升级页面：通用IO固件
    widgets.append(ui->labelSingleUniIO);
    widgets.append(ui->textSingleUniIO);
    widgets.append(ui->btnSingleUniIO);
    widgets.append(ui->textSingleUniIOXML);
    widgets.append(ui->btnSingleUniIOXML);
    widgets.append(ui->lineSingleUniIO);
    allWidget.insert(QString("unio"),widgets);
    widgets.clear();

    //一键升级页面：末端IO固件
    widgets.append(ui->labelTerminalTheme);
    widgets.append(ui->labelCurrentTerminalVersion);
    widgets.append(ui->labelUpgradeTerminalVersion);
    //高级功能/单个升级页面：末端IO固件
    widgets.append(ui->labelSingleTerminal);
    widgets.append(ui->textSingleTerminal);
    widgets.append(ui->btnSingleTerminal);
    widgets.append(ui->textSingleTerminalXML);
    widgets.append(ui->btnSingleTerminalXML);
    widgets.append(ui->lineSingleTerminal);
    allWidget.insert(QString("terminal"),widgets);
    widgets.clear();

    //一键升级页面：伺服固件
    widgets.append(ui->labelServoTheme);
    widgets.append(ui->labelJ1Theme);
    widgets.append(ui->labelCurrentJ1Version);
    widgets.append(ui->labelUpgradeJ1Version);
    widgets.append(ui->labelJ2Theme);
    widgets.append(ui->labelCurrentJ2Version);
    widgets.append(ui->labelUpgradeJ2Version);
    widgets.append(ui->labelJ3Theme);
    widgets.append(ui->labelCurrentJ3Version);
    widgets.append(ui->labelUpgradeJ3Version);
    widgets.append(ui->labelJ4Theme);
    widgets.append(ui->labelCurrentJ4Version);
    widgets.append(ui->labelUpgradeJ4Version);
    widgets.append(ui->labelJ5Theme);
    widgets.append(ui->labelCurrentJ5Version);
    widgets.append(ui->labelUpgradeJ5Version);
    widgets.append(ui->labelJ6Theme);
    widgets.append(ui->labelCurrentJ6Version);
    widgets.append(ui->labelUpgradeJ6Version);
    //高级功能/单个升级页面：伺服固件
    ui->btnServoAll6J->setChecked(false);
    widgets.append(ui->btnServoAll6J);
    widgets.append(ui->textSingleServoJAll);
    widgets.append(ui->btnSingleServoUpgradeAllFW);
    widgets.append(ui->textSingleAllXML);
    widgets.append(ui->btnSingleServoUpgradeAllXML);
    widgets.append(ui->lineSingleServoJ1);
    /* //高级功能的几个伺服参数默认不展开不显示，由btnServoAll6J去控制
    widgets.append(ui->labelSingleServoJ1);//1
    widgets.append(ui->textSingleServoJ1);
    widgets.append(ui->btnSingleServoJ1);
    widgets.append(ui->textSingleServoJ1XML);
    widgets.append(ui->btnSingleServoJ1XML);
    widgets.append(ui->labelSingleServoJ2);//2
    widgets.append(ui->textSingleServoJ2);
    widgets.append(ui->btnSingleServoJ2);
    widgets.append(ui->textSingleServoJ2XML);
    widgets.append(ui->btnSingleServoJ2XML);
    widgets.append(ui->labelSingleServoJ3);//3
    widgets.append(ui->textSingleServoJ3);
    widgets.append(ui->btnSingleServoJ3);
    widgets.append(ui->textSingleServoJ3XML);
    widgets.append(ui->btnSingleServoJ3XML);
    widgets.append(ui->labelSingleServoJ4);//4
    widgets.append(ui->textSingleServoJ4);
    widgets.append(ui->btnSingleServoJ4);
    widgets.append(ui->textSingleServoJ4XML);
    widgets.append(ui->btnSingleServoJ4XML);
    widgets.append(ui->labelSingleServoJ5);//5
    widgets.append(ui->textSingleServoJ5);
    widgets.append(ui->btnSingleServoJ5);
    widgets.append(ui->textSingleServoJ5XML);
    widgets.append(ui->btnSingleServoJ5XML);
    widgets.append(ui->labelSingleServoJ6);//6
    widgets.append(ui->textSingleServoJ6);
    widgets.append(ui->btnSingleServoJ6);
    widgets.append(ui->textSingleServoJ6XML);
    widgets.append(ui->btnSingleServoJ6XML);*/
    allWidget.insert(QString("servo"),widgets);
    widgets.clear();

    for(auto itr=allWidget.begin(); itr!=allWidget.end(); ++itr){
        QString key = itr.key();
        bool bShow = false;
        if (result.contains(key)){
            bShow = result.value(key);
        }
        const auto& widgets = itr.value();
        for (auto w=widgets.begin(); w!=widgets.end(); ++w){
            (*w)->setVisible(bShow);
        }
    }

}

void MainWidget2::widget2UpgradeDevice()
{
    m_upgrade2Widget->initStatus();
    m_isInterruptUpgrade = false;
    DobotType::ProtocolExchangeResult exchange = m_httpProtocol->getProtocolExchange(g_strCurrentIP);
    if(exchange.prjState != "stopped"){
        auto* pWndMsg = new MessageWidget(this);
        pWndMsg->setMessage("warn",tr("当前设备处于工程运行状态或报警\n不适合升级"));
        pWndMsg->show();
        return;
    }

    m_upgrade2Widget->setGeometry(0,0,this->width(),this->height());
    m_upgrade2Widget->raise();
    m_upgrade2Widget->show();

    QString strUpgradeDir = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion;
    QDir upgradeDir(strUpgradeDir);
    qDebug()<<"strUpgradeDir  "<<strUpgradeDir;
    if(!upgradeDir.exists()){
        if(isOnline==false){
            auto ptr = new TipWidget(this);
            ptr->setGeometry(0,0,width(),height());
            ptr->raise();
            ptr->show();
            return;
        }else{
            m_upgrade2Widget->setDownloadFirmwareStatus(1);
            slot_download();
            return;
        }
    }else{
        widget2UpgradeFirmwareDowloadFinish(true);
    }
}

void MainWidget2::slot_importFinishedTempVersion(bool ok)
{
    if(!ok){
        m_diyProgressDialog->setTitle(tr("导入失败"));
        m_diyProgressDialog->setHeadLabel("failed!");
        isCurrentVersionEnableUpgrade();
    }else{
        m_diyProgressDialog->setTitle(tr("导入成功"));
        qDebug()<<"m_diyProgressDialog->progressRange()  "<<m_diyProgressDialog->progressRange();;
        if(m_diyProgressDialog->progressRange()==0)
        {
            m_diyProgressDialog->setProgressValue(100);
        }
        m_diyProgressDialog->setHeadLabel("success!");
        searchMenuItems();
        m_upgradeParentVersion = "temporary";
        m_upgradeVersion = m_strImportDirName;
        ui->btnUpgrade->show();

        readUpgradeVersion(m_upgradeParentVersion,m_upgradeVersion);
        isCurrentVersionEnableUpgrade();

        while (m_diyProgressDialog->isVisible()){//窗口关闭隐藏了才开始校验对比伺服参数，不然窗口遮挡住看不到效果，又会唧唧歪歪说个不停
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
        if (!ui->btnUpgrade->isEnabled())
        {
            WidgetToast::warn(tr("当前固件版本信息获取不全，无法一键升级"),3000);
        }
        changeMenuButtonText(m_strImportDirName, true);
        qDebug()<<"导入升级包后，准备开始进行伺服参数校验对比.....";
        showServoParamLabel(true);
    }
}

void MainWidget2::slot_upgradeDevice()
{
    if (m_bIsCheckingServoParam)
    {
        WidgetToast::warn(tr("请等待伺服参数对比完成"), 2000);
        return ;
    }
    QString strReason;
    auto bIsCanUpgrade = CLuaApi::getInstance()->isCanUpgrade(strReason);
    if (bIsCanUpgrade.hasValue())
    {
        if (!bIsCanUpgrade.value())
        {
            if (strReason.isEmpty()) strReason = tr("V3.5.2.4以下不允许升级");
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",strReason);
            pWndMsg->show();
            return;
        }
    }
    else
    {
        if (CommonData::getControllerType().name.contains("Nova",Qt::CaseInsensitive)){
            //nova所有的版本都允许升级
        }else{
            QString strV3Control = CommonData::getCurrentSettingsVersion().control.split("-")[0];
            if(strV3Control.at(0) == 'V' || strV3Control.at(0) == 'v')
            {
                strV3Control = strV3Control.mid(1);
            }
            if(strV3Control.at(0) == '3')
            {
                QStringList strV3List = strV3Control.split(".");
                bool isSmallerMainControl = false;
                int V1 = strV3List[0].toInt();
                int V2 = strV3List[1].toInt();
                int V3 = strV3List[2].toInt();
                int V4 = strV3List[3].toInt();
                if(V1<3) isSmallerMainControl = true;
                if(V1==3&&V2<5) isSmallerMainControl = true;
                if(V1==3&&V2==5&&V3<2) isSmallerMainControl = true;
                if(V1==3&&V2==5&&V3==2&&V4<4) isSmallerMainControl = true;
                qDebug()<<"slot_upgradeDevice intVersion V3.5.2.4以下不允许升级 "<<isSmallerMainControl;
                if(isSmallerMainControl)
                {
                    auto* pWndMsg = new MessageWidget(this);
                    pWndMsg->setMessage("warn",tr("V3.5.2.4以下不允许升级"));
                    pWndMsg->show();
                    return;
                }
            }
            QString strUpgradeControl;
            if(m_upgradeParentVersion == "temporary")
            {
                if(CommonData::getUpgradeSettingsVersion().control.isEmpty()||CommonData::getUpgradeSettingsVersion().control.size()<7)
                {
                    auto* pWndMsg = new MessageWidget(this);
                    pWndMsg->setMessage("warn",tr("当前升级版本有问题"));
                    pWndMsg->show();
                    return;
                }
                if(CommonData::getUpgradeSettingsVersion().control.at(0) == "V")
                {
                    strUpgradeControl = CommonData::getUpgradeSettingsVersion().control.mid(1,7);
                }
                else
                {
                    strUpgradeControl = CommonData::getUpgradeSettingsVersion().control.mid(0,7);
                }
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
                {
                    if(strUpgradeControl.at(0) == '3')
                    {
                        QStringList strV3List = strUpgradeControl.split(".");
                        if(strV3List.size()<4)
                        {
                            auto* pWndMsg = new MessageWidget(this);
                            pWndMsg->setMessage("warn",tr("当前升级版本有问题"));
                            pWndMsg->show();
                            return;
                        }

                        bool isSmallerMainControl = false;
                        int V1 = strV3List[0].toInt();
                        int V2 = strV3List[1].toInt();
                        int V3 = strV3List[2].toInt();
                        int V4 = strV3List[3].toInt();
                        if(V1<3) isSmallerMainControl = true;
                        if(V1==3&&V2<5) isSmallerMainControl = true;
                        if(V1==3&&V2==5&&V3<4) isSmallerMainControl = true;
                        if(V1==3&&V2==5&&V3==4&&V4<0) isSmallerMainControl = true;
                        qDebug()<<"slot_upgradeDevice intVersion 升级版本只能是3.5.4.0及以上 "<<isSmallerMainControl;
                        if(isSmallerMainControl)
                        {
                            auto* pWndMsg = new MessageWidget(this);
                            pWndMsg->setMessage("warn",tr("机器人维护工具仅支持升级/回退至控制器版本3.5.4.0及以上"));
                            pWndMsg->show();
                            return;
                        }
                    }
                }
                else
                {
                    if(strUpgradeControl.at(0) == '4')
                    {
                        QStringList strV4List = strUpgradeControl.split(".");
                        if(strV4List.size()<4)
                        {
                            auto* pWndMsg = new MessageWidget(this);
                            pWndMsg->setMessage("warn",tr("当前升级版本有问题"));
                            pWndMsg->show();
                            return;
                        }
                        bool isSmallerMainControl = false;
                        int V1 = strV4List[0].toInt();
                        int V2 = strV4List[1].toInt();
                        int V3 = strV4List[2].toInt();
                        int V4 = strV4List[3].toInt();
                        if(V1<4) isSmallerMainControl = true;
                        if(V1==4&&V2<4) isSmallerMainControl = true;
                        if(V1==4&&V2==4&&V3<0) isSmallerMainControl = true;
                        if(V1==4&&V2==4&&V3==0&&V4<0) isSmallerMainControl = true;
                        qDebug()<<"slot_upgradeDevice intVersion 升级版本只能是4.4.0.0及以上 "<<isSmallerMainControl;
                        if(isSmallerMainControl)
                        {
                            auto* pWndMsg = new MessageWidget(this);
                            pWndMsg->setMessage("warn",tr("机器人维护工具仅支持升级/回退至控制器版本4.4.0.0及以上"));
                            pWndMsg->show();
                            return;
                        }
                    }
                }
            }

            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)&&
                    (CommonData::getUpgradeSettingsVersion().control.contains("V3")||CommonData::getUpgradeSettingsVersion().control.at(0) == '3'))
            {
                auto* pWndMsg = new MessageWidget(this);
                pWndMsg->setMessage("warn",tr("二代控制柜不允许回退V3"));
                pWndMsg->show();
                return;
            }

            if(CommonData::getCurrentSettingsVersion().servo1.startsWith("3.1"))
            {
                auto* pWndMsg = new MessageWidget(this);
                pWndMsg->setMessage("warn",tr("该控制柜版本不支持一键升级请联系技术支持"));
                pWndMsg->show();
                qDebug()<<"伺服为3.1版本，不允许升级";
                return;
            }

            if(strV3Control.at(0) == '3')
            {
                QStringList strV3List = strV3Control.split(".");
                bool isSmallerOldElectronicSkinMainControl = false;
                int V1 = strV3List[0].toInt();
                int V2 = strV3List[1].toInt();
                int V3 = strV3List[2].toInt();
                int V4 = strV3List[3].toInt();
                if(V1<3) isSmallerOldElectronicSkinMainControl = true;
                if(V1==3&&V2<5) isSmallerOldElectronicSkinMainControl = true;
                if(V1==3&&V2==5&&V3<3) isSmallerOldElectronicSkinMainControl = true;
                if(V1==3&&V2==5&&V3==3&&V4<0) isSmallerOldElectronicSkinMainControl = true;

                qDebug()<<"slot_upgradeDevice intVersion V3.5.3.0  如果当前机器带电子皮肤，若升级至3.5.3及以上版本，电子皮肤会失效 "<<isSmallerOldElectronicSkinMainControl;
                if(isSmallerOldElectronicSkinMainControl)
                {
                    auto ptr = new UpgradeIsOldElectronicSkinWidget(this);
                    ptr->setGeometry(0,0,width(),height());
                    connect(ptr,&UpgradeIsOldElectronicSkinWidget::signal_toUpgradeDevice,this,[&]{
                        widget2UpgradeDevice();
                    });
                    ptr->raise();
                    ptr->show();
                    return;
                }
            }

            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX)||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162))
            {
                if(CommonData::getUpgradeSettingsVersion().control.at(0) =='4')
                {
                    qDebug()<<"一代柜不允许升级V4";
                    auto* pWndMsg = new MessageWidget(this);
                    pWndMsg->setMessage("warn",tr("一代柜不允许升级V4"));
                    pWndMsg->show();
                    return;
                }
            }

            if(CommonData::getCurrentSettingsVersion().control.contains("V4")||CommonData::getCurrentSettingsVersion().control.at(0) == '4' )
            {
                if(CommonData::getUpgradeSettingsVersion().control.contains("V3")||CommonData::getUpgradeSettingsVersion().control.at(0) =='3')
                {
                    auto* pWndMsg = new MessageWidget(this);
                    pWndMsg->setMessage("warn",tr("V4禁止回退至V3"));
                    pWndMsg->show();
                    return;
                }
            }

            if(CommonData::getCurrentSettingsVersion().control.contains("V3")||CommonData::getCurrentSettingsVersion().control.at(0) == '3' )
            {
                if(CommonData::getUpgradeSettingsVersion().control.contains("V4")||CommonData::getUpgradeSettingsVersion().control.at(0) =='4')
                {
                    auto* pWndMsg = new MessageWidget(this);
                    pWndMsg->setMessage("warn",tr("禁止V3升级至V4"));
                    pWndMsg->show();
                    return;
                }
            }

            //对ED6单圈做处理
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6)
                    &&CommonData::getCurrentSettingsVersion().servo1.at(0) == '2'
                    &&ui->labelUpgradeJ1Version->text().contains("-"))
            {
                auto* pWndMsg = new MessageWidget(this);
                pWndMsg->setMessage("warn",tr("该伺服版本禁止升级，请联系FAE"));
                pWndMsg->show();
                return;
            }
        }
    }

    auto pLoading = new LoadingUI(this);
    pLoading->setGeometry(0,0,width(),height());
    pLoading->show();
    QString postUpdateCheckPartitionOccupancyResult = m_httpProtocol->getUpdateCheckPartitionOccupancy(g_strCurrentIP);
    pLoading->deleteLater();
    if(postUpdateCheckPartitionOccupancyResult.isEmpty()||postUpdateCheckPartitionOccupancyResult != "idle")
    {
        m_isControlLogBusyWidget->setGeometry(0,0,width(),height());
        m_isControlLogBusyWidget->show();
        return;
    }

    m_exceptionOutage.bStart = false;
    widget2UpgradeDevice();
}

void MainWidget2::textClear()
{
    ui->btnUpgrade->hide();
    ui->btnSingleFunc->setEnabled(false);

    emit ui->btnFirmwareUpgrade->clicked(true);

    m_upgradeVersion = "";
    m_upgradeParentVersion = "";
    changeMenuButtonText("");
    showUpgradeServoParamAllWriteUI(false);
    showServoParamLabel(false);

    ui->labelCurrentMainControlVersion->setText("-");
    ui->labelCurrentSafeIOVersion->setText("-");
    ui->labelCurrentIOFirmwareVersion->setText("-");
    ui->labelUpgradeIOFirmwareVersion->setText("-");
    ui->labelCurrentFeedbackVersion->setText("-");
    ui->labelCurrentUnIOVersion->setText("-");
    ui->labelCurrentJ1Version->setText("-");
    ui->labelCurrentJ2Version->setText("-");
    ui->labelCurrentJ3Version->setText("-");
    ui->labelCurrentJ4Version->setText("-");
    ui->labelCurrentJ5Version->setText("-");
    ui->labelCurrentJ6Version->setText("-");
    ui->labelCurrentTerminalVersion->setText("-");

    ui->labelUpgradeMainControlVersion->setText("-");
    ui->labelUpgradeSafeIOVersion->setText("-");
    ui->labelUpgradeFeedbackVersion->setText("-");
    ui->labelUpgradeUnIOVersion->setText("-");
    ui->labelUpgradeJ1Version->setText("-");
    ui->labelUpgradeJ2Version->setText("-");
    ui->labelUpgradeJ3Version->setText("-");
    ui->labelUpgradeJ4Version->setText("-");
    ui->labelUpgradeJ5Version->setText("-");
    ui->labelUpgradeJ6Version->setText("-");
    ui->labelUpgradeTerminalVersion->setText("-");
}

void MainWidget2::changeMenuButtonText(QString strText, bool bHasPackage)
{
    if (strText.isEmpty())
    {
        ui->btnDownload->setEnabled(false);
        ui->btnVersionText->hide();
    }
    else
    {
        ui->btnDownload->setEnabled(true);
        ui->btnVersionText->setVisible(bHasPackage);
    }
    ui->btnMenu->changeMenuButtonText(strText, bHasPackage);
}

void MainWidget2::showServoParamLabel(bool bHasPack)
{
    ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant());
    if (ui->btnUpgradeWidgetServoParamAllWrite->isHidden()) return;
    //选中某个升级包后，判断是否需要升级单个伺服参数
    if (!bHasPack)
    {
        return;
    }
    ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(true, QVariant());
    bool bNeedUpdate = false;
    QList<int> bJointNotEqual; //检测到不相等的伺服关节
    QString arrJointCSVFile[6];
    qDebug()<<"准备开始伺服参数校验对比.checkSveroParameters:"<<m_upgradeParentVersion<<",m_upgradeVersion="<<m_upgradeVersion;
    if (checkSveroParameters(bNeedUpdate,bJointNotEqual,arrJointCSVFile))
    {
        QString str;
        for(auto j : bJointNotEqual){
            str += QString::asprintf("J%d,",j);
        }
        if (!str.isEmpty()){//不为空说明存在关节伺服参数不一致的情况
            str = QString(tr("机器本体的%1伺服参数与升级包的不一致")).arg(str);
        }
        ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant(bNeedUpdate),false,str);
    }
    else
    {
        if (bNeedUpdate){//这种情况，就是其中某几个关节检测到需要更新，另外几个关节检测失败了
            QString str;
            for(auto j : bJointNotEqual){
                str += QString::asprintf("J%d,",j);
            }
            str = QString(tr("机器本体的%1伺服参数与升级包的不一致")).arg(str);
            ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant(),true,str);
        }else{
            ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant(),true);
        }
    }
}

void MainWidget2::showUpgradeServoParamAllWriteUI(bool show)
{
    if (show){
        ui->btnUpgradeWidgetServoParamAllWrite->show();
        ui->scrollAreaUpgrade->setProperty("showServer",QVariant(true));
    }
    else{
        ui->btnUpgradeWidgetServoParamAllWrite->hide();
        ui->scrollAreaUpgrade->setProperty("showServer",QVariant());
    }
}

void MainWidget2::cabinetTypeInfoShow()
{
    if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX)){
        ui->labelCurrentIOFirmwareTheme->show();
        ui->labelCurrentIOFirmwareVersion->show();
        ui->labelUpgradeIOFirmwareVersion->show();

        ui->labelCurrentSafeIOTheme->hide();
        ui->labelCurrentSafeIOVersion->hide();
        ui->labelUpgradeSafeIOVersion->hide();

        ui->labelCurrentFeedbackTheme->hide();
        ui->labelCurrentFeedbackVersion->hide();
        ui->labelUpgradeFeedbackVersion->hide();

        ui->labelCurrentUnIOTheme->hide();
        ui->labelCurrentUnIOVersion->hide();
        ui->labelUpgradeUnIOVersion->hide();
    }else if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6)){
        ui->labelCurrentUnIOTheme->show();
        ui->labelCurrentUnIOVersion->show();
        ui->labelUpgradeUnIOVersion->show();

        ui->labelCurrentSafeIOTheme->hide();
        ui->labelCurrentSafeIOVersion->hide();
        ui->labelUpgradeSafeIOVersion->hide();

        ui->labelCurrentFeedbackTheme->hide();
        ui->labelCurrentFeedbackVersion->hide();
        ui->labelUpgradeFeedbackVersion->hide();

        ui->labelCurrentIOFirmwareTheme->hide();
        ui->labelCurrentIOFirmwareVersion->hide();
        ui->labelUpgradeIOFirmwareVersion->hide();

    }
    else{
        ui->labelCurrentSafeIOTheme->show();
        ui->labelCurrentSafeIOVersion->show();
        ui->labelUpgradeSafeIOVersion->show();

        ui->labelCurrentFeedbackTheme->show();
        ui->labelCurrentFeedbackVersion->show();
        ui->labelUpgradeFeedbackVersion->show();

        ui->labelCurrentUnIOTheme->show();
        ui->labelCurrentUnIOVersion->show();
        ui->labelUpgradeUnIOVersion->show();

        ui->labelCurrentIOFirmwareTheme->hide();
        ui->labelCurrentIOFirmwareVersion->hide();
        ui->labelUpgradeIOFirmwareVersion->hide();
    }

}

void MainWidget2::showCurrentAndUpgradeFirmware(QLabel *labelTheme, QLabel *labelCurrent, QLabel *labelUpgrade, bool isShow)
{
    if(isShow){
        labelTheme->show();
        labelCurrent->show();
        labelUpgrade->show();
    }else{
        labelTheme->hide();
        labelCurrent->hide();
        labelUpgrade->hide();
    }
}

void MainWidget2::setStackedWidgetDisable(bool isDisabled,bool is02ServerStarted)
{
#ifdef QT_DEBUG
    isDisabled = false;
    is02ServerStarted = true;
#endif
    ui->pageUpgrade->setDisabled(false);

    if(isDisabled)
    {
        ui->btnMenu->setDisabled(true);
        ui->pageFileBackup->setDisabled(true);
        ui->pageFileRecovery->setDisabled(true);
        ui->pageSystemTool->setDisabled(true);
        ui->pageServoParams->setDisabled(true);
        m_bIsStackedWidgetDisable = true;
    }
    else
    {
        ui->btnMenu->setDisabled(false);
        ui->pageFileBackup->setDisabled(false);
        ui->pageFileRecovery->setDisabled(false);
        ui->pageSystemTool->setDisabled(false);
        ui->pageServoParams->setDisabled(false);
        m_bIsStackedWidgetDisable = false;

    }
    if(is02ServerStarted)
    {
        ui->btnSingleFunc->setDisabled(false);
        ui->pageUpgradeSingleCRCC16->setDisabled(false);
    }
    else
    {
        ui->btnSingleFunc->setDisabled(true);
        ui->pageUpgradeSingleCRCC16->setDisabled(true);
    }
}

void MainWidget2::setLabelToolTip(QLabel *label, QString text)
{
    label->setText(text);
    label->setToolTipDuration(1000);
    label->setToolTip(label->text());
    QFontMetrics metric(label->font());
    QString str = metric.elidedText(label->text(), Qt::ElideRight, label->width());
    label->setText(str);
}

void MainWidget2::systemToolSyncTime()
{
    QDate qDate = QDate::currentDate();
    QString date = qDate.toString("yyyy-MM-dd");
    qDebug()<<"btnSystemSynTime date "<<date;
    QTime qTime = QTime::currentTime();
    QString time = qTime.toString("hh:mm:ss");
    qDebug()<<"btnSystemSynTime time"<<time;
    m_widgetSystemToolTip->setRepairStatus(1);
    m_widgetSystemToolTip->show();
    if(m_httpProtocol->postFaultRepair(g_strCurrentIP,"synTime","", date,time)!="success")
    {
        m_widgetSystemToolTip->setRepairStatus(-1);
    }
    else
    {
        m_widgetSystemToolTip->setRepairStatus(2);
    }
}

void MainWidget2::widget2UpgradeFirmwareDowloadFinish(bool ok)
{
    if(ok){
        m_upgrade2Widget->setDownloadFirmwareStatus(2);
        widget2StartServerFinish(true);
    }else{
        m_upgrade2Widget->setDownloadFirmwareStatus(-1);
    }
}

void MainWidget2::widget2StartServer()
{
    QString strStartToolServer = getStartServerZipFileDir()+"startServer/startToolServer.exe";
    if (!QFileInfo::exists(strStartToolServer))
    {
        QString strTips = QString(tr("缺失文件%1: ")).arg(strStartToolServer);
        auto* messageWidget = new MessageWidget(this);
        messageWidget->setMessage("warn",strTips);
        messageWidget->show();
        return;
    }
    QProcess* p = new QProcess();
    QString* allResult = new QString();
    QEventLoop eventLoop;
    QObject::connect(p,&QProcess::errorOccurred,[&](QProcess::ProcessError error){
        qWarning()<<" widget2StartServer slot_ errorOccurred!" <<error;
        p->terminate();
        p->kill();

    });

    QObject::connect(p,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[&](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<" widget2StartServer finished! " <<exitStatus;
        QString cmdResult = p->readAll();
        if(cmdResult.contains("idel-show:The disk is already full!"))
        {
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setAttribute(Qt::WA_DeleteOnClose,true);
            pWndMsg->setGeometry(0,0,width(),height());
            pWndMsg->setMessage("warn",tr("请联系FAE清理磁盘"));
            pWndMsg->show();
        }
        qDebug()<< "cmdResult "<<cmdResult;
        delete allResult;
        eventLoop.quit();
        p->terminate();
        p->kill();
        p->deleteLater();
    });
    QString strArchitecture = CLuaApi::getInstance()->getRobotArchitecture();
    p->start(strStartToolServer,QStringList()<<g_strCurrentIP<<strArchitecture);
    eventLoop.exec();
}

void MainWidget2::widget2StartServerFinish(bool ok)
{
    //判断当前是否是Nova设备
    DobotType::ControllerType controllerType = m_httpProtocol->getControllerType(g_strCurrentIP);
    QString strNova = "NC";
    QString strV4RX = "RX";

    QString firmwareFilePath = "upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/files/";
    QStringList projectList;
    QSet<QString> projectFiles;
    QDir dir(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/files");
    if(!dir.exists())
    {
        qDebug()<<"固件升级文件夹不存在 ";
    }
    bool isHasCR20 = false;
    for(QFileInfo fileInfo:dir.entryInfoList())
    {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..") continue;
        QString strFileName{fileInfo.fileName()};
        auto bNeedCopy = CLuaApi::getInstance()->isFileNeedCopyToController(strFileName);
        if (bNeedCopy.hasValue())
        {
            if (bNeedCopy.value())
            {
                projectFiles.insert(firmwareFilePath+fileInfo.fileName());
            }
            continue;
        }
        if(fileInfo.fileName().contains("Servo")){
            if(fileInfo.fileName().contains("XML"))
            {
                qDebug()<<" fileInfo.fileName() Servo  XML"<<fileInfo.fileName();
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)
                        &&fileInfo.fileName().contains("V2")
                        &&(CommonData::getCurrentSettingsVersion().servo1.at(0)=='6'
                           ||CommonData::getCurrentSettingsVersion().servo1.at(0)=='2'
                           ||CommonData::getCurrentSettingsVersion().servo1.at(0)=='7'))
                {
                    projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                }

                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)
                        &&fileInfo.fileName().contains("3.0.0")
                        &&CommonData::getCurrentSettingsVersion().servo1.at(0)!='6'
                        &&CommonData::getCurrentSettingsVersion().servo1.at(0)!='2')
                {
                    projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                }

                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX)
                        &&fileInfo.fileName().contains("V2")
                        &&fileInfo.fileName().contains(strNova))
                {
                    projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                }
                //E6伺服XML
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
                {
                    if(fileInfo.fileName().contains("V2")
                            &&fileInfo.fileName().contains("MG6")
                            &&CommonData::getCurrentSettingsVersion().servo1.at(0)=='2')
                    {
                        projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                    }
                    if(fileInfo.fileName().contains("V3")
                            &&fileInfo.fileName().contains("MG6")
                            &&CommonData::getCurrentSettingsVersion().servo1.at(0)=='3')
                    {
                        projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                    }
                }
            }
            else
            {
                if(ui->labelCurrentJ1Version->text().at(0)=='4'){
                    if(fileInfo.fileName().contains("V4")){
                        projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                    }
                }
                if(ui->labelCurrentJ1Version->text().at(0)=='3'){
                    if(fileInfo.fileName().contains("V3")){
                        projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                    }
                }

                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X))
                {
                    if(fileInfo.fileName().contains("CR")
                            &&fileInfo.fileName().contains(strV4RX)
                            &&(CommonData::getCurrentSettingsVersion().servo1.at(0)=='6'
                            ||CommonData::getCurrentSettingsVersion().servo1.at(0)=='7'))
                    {
                        projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                    }

                }
                //CCBOX 伺服固件传输
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
                {
                    if(fileInfo.fileName().contains(strNova,Qt::CaseInsensitive))
                    {
                        projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                    }
                }
                //E6伺服固件
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
                {
                    if(fileInfo.fileName().contains("V2")
                            &&CommonData::getCurrentSettingsVersion().servo1.at(0)=='2')
                    {
                        projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                    }
                    if(fileInfo.fileName().contains("V3")
                            &&CommonData::getCurrentSettingsVersion().servo1.at(0)=='3')
                    {
                        projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                    }
                }
            }
        }
        else if(fileInfo.fileName().contains("TerminalIO")){
            //E6末端固件 XML传输
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
            {
                if(fileInfo.fileName().contains("RX")&&fileInfo.fileName().contains("MG6"))
                {
                    projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                }
            }
            //CCBOX 末端固件 XML传输
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
            {
                if(fileInfo.fileName().contains(strNova)&&CommonData::getCurrentSettingsVersion().terminal.at(0)=='6')
                {
                    projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                }
            }
            //二代柜 末端固件 XML传输
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X))
            {
                //不是CR20的固件 XML传输
                if(!CommonData::getControllerType().name.contains("CR20A"))
                {
                    if(fileInfo.fileName().contains(strV4RX)
                            &&!fileInfo.fileName().contains("CR20")
                            &&CommonData::getCurrentSettingsVersion().terminal.at(0)=='6')
                    {
                        projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                    }
                }
                //是CR20的固件 XML传输
                if(CommonData::getControllerType().name.contains("CR20A"))
                {
                    if(fileInfo.fileName().contains(strV4RX)
                            &&fileInfo.fileName().contains("CR20")
                            &&CommonData::getCurrentSettingsVersion().terminal.at(0)=='6')
                    {
                        isHasCR20 = true;
                        projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                    }
                }
            }
            //一代大型控制柜 末端固件XML传输
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162))
            {
                if(fileInfo.fileName().contains("ST")
                        &&fileInfo.fileName().contains("CR"))
                {
                    projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                }
            }
        }
        else {
            projectFiles.insert(firmwareFilePath+fileInfo.fileName());
        }
    }
    if(CommonData::getControllerType().name.contains("CR20A")&&!isHasCR20)
    {
        for(QFileInfo fileInfo:dir.entryInfoList())
        {
            if(fileInfo.fileName().contains("TerminalIO")){
                if(fileInfo.fileName().contains(strV4RX)
                        &&!fileInfo.fileName().contains("CR20")
                        &&CommonData::getCurrentSettingsVersion().terminal.at(0)=='6')
                {
                    projectFiles.insert(firmwareFilePath+fileInfo.fileName());
                }
            }
        }
    }
    projectList = projectFiles.toList();
    QStringList mainControlFiles;
    QDir upgradeMainControlDir(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion);
    for(QFileInfo fileInfo : upgradeMainControlDir.entryInfoList())
    {
        QString fileAbsolutePath = fileInfo.absoluteFilePath();
        QString fileName = fileInfo.fileName();
        auto info = getMatchCRUpdateInfo(fileName);
        if(info.valid)
        {
            auto allMainFiles = CLuaApi::getInstance()->getControllerUpdateFiles();
            if (allMainFiles.hasValue())
            {
                const QString strPath = "upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/"+fileInfo.fileName()+"/src/";
                QStringList allFiles = allMainFiles.value();
                for (QString str : allFiles)
                {
                    mainControlFiles.append(strPath+str);
                }
                break;
            }
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
            {
                mainControlFiles.append("upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/"+fileInfo.fileName()+"/src/bin.tar.gz");
                mainControlFiles.append("upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/"+fileInfo.fileName()+"/src/afterUpdate.sh");
            }
            else
            {
                mainControlFiles.append("upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/"+fileInfo.fileName()+"/src/update.tar.gz");
            }
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
            {
                mainControlFiles.append("upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/"+fileInfo.fileName()+"/src/python_env.tar.gz");
            }
            break;
        }
        else
        {
            qDebug()<<" mainControl File not exist ............... ";
        }
    }

    QStringList kernelFiles;
    QString cabVersion;
    auto cabVersionLua = CLuaApi::getInstance()->getCabVersion();
    if (cabVersionLua.hasValue())
    {
        cabVersion = cabVersionLua.value();
    }
    else{
        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)
                ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
        {
            cabVersion = "V1";
        }
        else
        {
            cabVersion = "V2";
        }
    }
    if(CommonData::getCurrentSettingsVersion().control.at(0)!='4')
    {
        QString result = m_httpProtocol->postUpdateCheckKernelVersion(g_strCurrentIP,cabVersion);
        if(result.isEmpty()||result!="same")
        {
            kernelFiles.append(getStartServerZipFileDir()+"startServer/kernel0804.tar.gz");
        }
    }

    qDebug()<<"m_strProtocolVersionSlaveFiles -->  "<<m_strProtocolVersionSlaveFiles;
    qDebug()<<"m_strProtocolVersionControl -->  "<<m_strProtocolVersionControl;
    qDebug()<<"m_strProtocolVersionKernel -->  "<<m_strProtocolVersionKernel;

    QString lstCmd;
    lstCmd.append(QString("rm -rf %1 && rm -rf %2 && mkdir -p %1 && mkdir -p %2")
                  .arg(m_strProtocolVersionSlaveFiles)
                  .arg(m_strProtocolVersionKernel));
    m_pFileTransfer->setUser("root");
    m_pFileTransfer->setPwd("dobot");
    m_pFileTransfer->setIp(g_strCurrentIP);
    m_pFileTransfer->setPlink(QCoreApplication::applicationDirPath()+"/tool/plink/plink.exe");
    m_pFileTransfer->setPscp(QCoreApplication::applicationDirPath()+"/tool/plink/pscp.exe");
    //要上传的文件
    QString strPath = QCoreApplication::applicationDirPath()+"/";
    for (auto itr=projectList.begin(); itr!=projectList.end(); ++itr)
    {
        itr->prepend(strPath);
    }
    for (auto itr=mainControlFiles.begin(); itr!=mainControlFiles.end(); ++itr)
    {
        itr->prepend(strPath);
    }
    m_pFileTransfer->setProperty("projectList",QVariant(projectList));
    m_pFileTransfer->setProperty("projectListDest",QVariant(m_strProtocolVersionSlaveFiles));
    m_pFileTransfer->setProperty("mainControlFiles",QVariant(mainControlFiles));
    m_pFileTransfer->setProperty("mainControlFilesDest",QVariant(m_strProtocolVersionControl));
    m_pFileTransfer->setProperty("kernelFiles",QVariant(kernelFiles));
    m_pFileTransfer->setProperty("kernelFilesDest",QVariant(m_strProtocolVersionKernel));

    m_pFileTransfer->execCmd(lstCmd);
}

void MainWidget2::widget2UpdateDiskKernel()
{
    m_upgrade2Widget->setDiskUpdateStatus(1,tr("升级内核中"));
    QString cabVersion = "";
    auto cabVersionLua = CLuaApi::getInstance()->getCabVersionKernel();
    if (cabVersionLua.hasValue())
    {
        cabVersion = cabVersionLua.value();
    }
    else{
        if(!CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X))
        {
            cabVersion = "V1";
        }
        else
        {
            cabVersion = "V2";
        }
    }
    QString result = m_httpProtocol->postUpdateKernel(g_strCurrentIP,cabVersion);
    if(result.isEmpty()||result != "success")
    {
        m_upgrade2Widget->setDiskUpdateStatus(-1,tr("升级内核失败"));
    }
    else
    {
        m_upgrade2Widget->setDiskUpdateStatus(2,tr("升级内核成功"));
        Widget2MainControlUpgrade();
    }

}

void MainWidget2::wigdet2ControllCompatibleUpdateDiskKernel()
{
    QProcess* p = new QProcess();
    QObject::connect(p,&QProcess::errorOccurred,[&](QProcess::ProcessError error){
        qWarning()<<"slot_ errorOccurred!" <<error;
    });

    QObject::connect(p,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<"wigdet2ControllCompatibleUpdateDiskKernel finished! " <<exitStatus;
        QString cmdResult = p->readAll();
        qDebug()<< "wigdet2ControllCompatibleUpdateDiskKernel 升级内核 cmdResult "<<cmdResult;
        if(cmdResult.contains("update kernel success!")){
            m_upgrade2Widget->hideLabelUpdateDisk();
            widget2UpgradeFileCopyToControlFinish(true);
        }else{
            m_upgrade2Widget->setDiskUpdateStatus(-1,tr("升级内核失败"));
            return;
        }
        p->deleteLater();
    });
    qDebug()<< "wigdet2ControllCompatibleUpdateDiskKernel 升级内核开始 ";
    p->start(QCoreApplication::applicationDirPath()+"/tool/System_update_0804/system_update_5.1.bat",QStringList()<<g_strCurrentIP);
    m_upgrade2Widget->setDiskUpdateStatus(1,tr("现在升级内核修复该问题"));
}

void MainWidget2::wigdet2UpdateCRSingleFwAndXML(QStringList allUpdateFile)
{
    QString strCRSingleFwAndXML = m_strLabelSingleFwAndXML;
    qDebug()<<" m_strLabelSingleFwAndXML "<<m_strLabelSingleFwAndXML;
    DobotType::StructCRSingleFwAndXml singleCRFwAndXml;
    auto cabTypeLua = CLuaApi::getInstance()->getCabType();
    if (cabTypeLua.hasValue())
    {
        singleCRFwAndXml.cabType = cabTypeLua.value();
    }
    else{
        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)
                ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X))
        {
            singleCRFwAndXml.cabType = "big";
        }
        else
        {
            singleCRFwAndXml.cabType = "small";
        }
    }
    auto cabVersionLua = CLuaApi::getInstance()->getCabVersion();
    if (cabVersionLua.hasValue())
    {
        singleCRFwAndXml.cabVersion = cabVersionLua.value();
    }
    else{
        if(CommonData::getStrPropertiesCabinetType().name==g_strPropertiesCabinetTypeCC162
                ||CommonData::getStrPropertiesCabinetType().name==g_strPropertiesCabinetTypeCCBOX)
        {
            singleCRFwAndXml.cabVersion = "V1";
        }
        else
        {
            singleCRFwAndXml.cabVersion = "V2";
        }
    }

    m_jallServoId.clear();
    auto slaveIdLua = CLuaApi::getInstance()->getHardwareSlaveId();
    if (slaveIdLua.hasValue())
    {
        const auto& kv = slaveIdLua.value();
        //int control = kv.value("control");
        int feedback = kv.value("feedback");
        int safeio = kv.value("safeio");
        int ccboxio = kv.value("ccboxio");
        int unio = kv.value("unio");
        int j1 = kv.value("j1");
        int j2 = kv.value("j2");
        int j3 = kv.value("j3");
        int j4 = kv.value("j4");
        int j5 = kv.value("j5");
        int j6 = kv.value("j6");
        int terminal = kv.value("terminal");
        if(strCRSingleFwAndXML.contains("UniIO")) singleCRFwAndXml.slaveId = unio;
        if(strCRSingleFwAndXML.contains("FeedBack")) singleCRFwAndXml.slaveId = feedback;
        if(strCRSingleFwAndXML.contains("CCBOX")) singleCRFwAndXml.slaveId = ccboxio;
        if(strCRSingleFwAndXML.contains("SafeIO")) singleCRFwAndXml.slaveId = safeio;
        if(strCRSingleFwAndXML.contains("ServoJ1"))  singleCRFwAndXml.slaveId = j1;
        if(strCRSingleFwAndXML.contains("ServoJ2")) singleCRFwAndXml.slaveId = j2;
        if(strCRSingleFwAndXML.contains("ServoJ3")) singleCRFwAndXml.slaveId = j3;
        if(strCRSingleFwAndXML.contains("ServoJ4")) singleCRFwAndXml.slaveId = j4;
        if(strCRSingleFwAndXML.contains("ServoJ5")) singleCRFwAndXml.slaveId = j5;
        if(strCRSingleFwAndXML.contains("ServoJ6")) singleCRFwAndXml.slaveId = j6;
        if(strCRSingleFwAndXML.contains("Terminal")) singleCRFwAndXml.slaveId = terminal;
        if(strCRSingleFwAndXML.contains("Servo") && strCRSingleFwAndXML.contains("JALL")){
            m_jallServoId<<j6<<j5<<j4<<j3<<j2<<j1;
            singleCRFwAndXml.slaveId = m_jallServoId.takeFirst();
        }
    }
    else{
        if(strCRSingleFwAndXML.contains("SafeIO")||strCRSingleFwAndXML.contains("FeedBack")) singleCRFwAndXml.slaveId = 1;
        if(strCRSingleFwAndXML.contains("CCBOX")) singleCRFwAndXml.slaveId = 1;
        if(strCRSingleFwAndXML.contains("UniIO"))
        {
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6)) singleCRFwAndXml.slaveId = 1;
            else singleCRFwAndXml.slaveId = 2;
        }
        if(strCRSingleFwAndXML.contains("Terminal"))
        {
            singleCRFwAndXml.slaveId = 8;
            const QString strCabinet = CommonData::getStrPropertiesCabinetType().name;
            if(strCabinet.contains(g_strPropertiesCabinetTypeCC162)||strCabinet.contains(g_strPropertiesCabinetTypeCC26X))
            {
                singleCRFwAndXml.slaveId++;
            }
        }
        if(strCRSingleFwAndXML.contains("Servo"))
        {
            if(strCRSingleFwAndXML.contains("J1")) singleCRFwAndXml.slaveId = 2;
            if(strCRSingleFwAndXML.contains("J2")) singleCRFwAndXml.slaveId = 3;
            if(strCRSingleFwAndXML.contains("J3")) singleCRFwAndXml.slaveId = 4;
            if(strCRSingleFwAndXML.contains("J4")) singleCRFwAndXml.slaveId = 5;
            if(strCRSingleFwAndXML.contains("J5")) singleCRFwAndXml.slaveId = 6;
            if(strCRSingleFwAndXML.contains("J6")) singleCRFwAndXml.slaveId = 7;

            if(strCRSingleFwAndXML.contains("JALL")) singleCRFwAndXml.slaveId = 7;
            const QString strCabinet = CommonData::getStrPropertiesCabinetType().name;
            if(strCabinet.contains(g_strPropertiesCabinetTypeCC162)||strCabinet.contains(g_strPropertiesCabinetTypeCC26X))
            {
                singleCRFwAndXml.slaveId++;
            }
            qDebug()<<"singleCRFwAndXml.slaveId  "<<singleCRFwAndXml.slaveId;
            if(strCRSingleFwAndXML.contains("JALL")){
                int j6 = singleCRFwAndXml.slaveId;
                int j5 = j6-1;
                int j4 = j5-1;
                int j3 = j4-1;
                int j2 = j3-1;
                int j1 = j2-1;
                m_jallServoId<<j6<<j5<<j4<<j3<<j2<<j1;
                singleCRFwAndXml.slaveId = m_jallServoId.takeFirst();
            }
        }
    }
    singleCRFwAndXml.operationType = "start";
    if(strCRSingleFwAndXML.contains("XML")) singleCRFwAndXml.updateType = "xml";
    else singleCRFwAndXml.updateType = "firmware";

    singleCRFwAndXml.allUpdateFile = allUpdateFile;

    qDebug()<<"singleCRFwAndXml ";
    singleCRFwAndXml.toString();
    m_singleCRFwAndXml = singleCRFwAndXml;
    const QString strResult = m_httpProtocol->postUpdateCRSingleFwAndXml(g_strCurrentIP,singleCRFwAndXml);
    if(strResult == g_strSuccess){
        sleep(10000);
        m_singleUpgradeTimer->start(1000);
    }else{
        qDebug()<<"  single upgrade postUpdateCRSingleFwAndXml Failed ";
        m_widgetSingleUpgrading->setSingleUpgradeStatus(-1, strResult);
    }
}

#if 0
void MainWidget2::wigdet2UpdateM1ProSingleFwAndXML(QString strLabelSingleFwAndXML,QString currentLabelFwOrXMLLocation)
{
    QString strSingleFwAndXML = strLabelSingleFwAndXML;
    DobotType::StructM1ProSingleFwAndXml singleM1ProFwAndXml;
    if(strSingleFwAndXML.contains("UniIO")) singleM1ProFwAndXml.slaveId = 1;

    //J1-J4 ui获取当前选项label
    QString currentServoLocation = currentLabelFwOrXMLLocation;
    if(strSingleFwAndXML.contains("Servo"))
    {
        if(currentServoLocation.contains("J1")) singleM1ProFwAndXml.slaveId = 2;
        if(currentServoLocation.contains("J2")) singleM1ProFwAndXml.slaveId = 3;
        if(currentServoLocation.contains("J3")) singleM1ProFwAndXml.slaveId = 4;
        if(currentServoLocation.contains("J4")) singleM1ProFwAndXml.slaveId = 5;
    }

    singleM1ProFwAndXml.operationType = "start";
    if(strSingleFwAndXML.contains("XML"))singleM1ProFwAndXml.updateType = "xml";
    else singleM1ProFwAndXml.updateType = "firmware";

    singleM1ProFwAndXml.updateFile = strSingleFwAndXML;
    if(m_httpProtocol->postUpdateM1ProSingleFwAndXml(g_strCurrentIP,singleM1ProFwAndXml) == "success"){
        //set True TODO
    }else{
        //set False TODO
    }
}

void MainWidget2::wigdet2UpdateMG400SingleFwAndXML(QString strLabelSingleFwAndXML,QString currentLabelFwOrXMLLocation)
{
    //ui->labelSingleFwAndXML
    QString strSingleFwAndXML = strLabelSingleFwAndXML;
    DobotType::StructMG400SingleFwAndXml singleMG400FwAndXml;
    singleMG400FwAndXml.slaveId = 1;

    singleMG400FwAndXml.operationType = "start";
    if(strSingleFwAndXML.contains("XML")) singleMG400FwAndXml.updateType = "xml";
    else singleMG400FwAndXml.updateType = "firmware";
    singleMG400FwAndXml.updateFile = strSingleFwAndXML;

    if(m_httpProtocol->postUpdateMG400SingleFwAndXml(g_strCurrentIP,singleMG400FwAndXml) == "success"){
        //set True TODO
    }else{
        //set False TODO
    }
}
#endif

void MainWidget2::setDiyPos()
{
    int x = (QApplication::desktop()->geometry().width()-this->width())/2;
    int y = QApplication::desktop()->geometry().height()/2-this->height()/2;
    this->setGeometry(x,y,this->width(),this->height());
}

void MainWidget2::disableWidgetWhenConnected()
{
    const QString strName = CommonData::getControllerType().name;
    QRegExp expCRV("CR\\d+V");

    style()->unpolish(ui->btnSystemDNSRepair);
    style()->unpolish(ui->btnDNSDetect);
    if (expCRV.exactMatch(strName) || strName.contains("E6") || strName.contains("Nova 2")){
        ui->btnDNSDetect->setEnabled(false);
        ui->btnSystemDNSRepair->setEnabled(false);
    }else{
        ui->btnDNSDetect->setEnabled(true);
        ui->btnSystemDNSRepair->setEnabled(true);
    }
    style()->polish(ui->btnDNSDetect);
    style()->polish(ui->btnSystemDNSRepair);
}

int MainWidget2::stringVersionToIntVersion(QString version)
{
    QStringList strVersionList = version.split(".");
    int value = 0;
    if (strVersionList.size()>0){
        value = strVersionList[0].toInt();
    }
    if (strVersionList.size()>1){
        value = value*10 + strVersionList[1].toInt();
    }
    if (strVersionList.size()>2){
        value = value*10 + strVersionList[2].toInt();
    }
    if (strVersionList.size()>3){
        value = value*10 + strVersionList[3].toInt();
    }
    return value;
}

void MainWidget2::readToolParamsToErrorCodes()
{
    //打开文件
    QFile file(getStartServerZipFileDir()+"startServer/tool_alarms.json");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }
    QJsonParseError error;
    QJsonDocument jdc=QJsonDocument::fromJson(file.readAll(),&error);
    //判断文件是否完整
    if(error.error!=QJsonParseError::NoError)
    {
        qDebug()<<"parseJson:"<<error.errorString();
        return;
    }
    QJsonArray obj = jdc.array(); //获取数组
    qDebug() <<"object size:"<<obj.size();
    QHash<int,DobotType::StructErrorCode> errorCodeHash;
    for(int i = 0; i < obj.size(); i++)
    {
        QJsonObject jsonErrorCode = obj[i].toObject();
        DobotType::StructErrorCode errorCode;
        errorCode.errorCode = jsonErrorCode.value("id").toInt();
        errorCode.enDescription = jsonErrorCode.value("en").toObject()
                .value("description").toString();
        errorCode.enSolution = jsonErrorCode.value("en").toObject()
                .value("solution").toString();
        errorCode.zhDescription = jsonErrorCode.value("zh_CN").toObject()
                .value("description").toString();
        errorCode.zhSolution = jsonErrorCode.value("zh_CN").toObject()
                .value("solution").toString();
        qDebug()<<"errorCode.enDescription  "<<errorCode.enDescription;
        qDebug()<<"errorCode.zhDescription  "<<errorCode.zhDescription;
        qDebug()<<"errorCode.zhSolution  "<<errorCode.zhSolution;
        qDebug()<<"errorCode.enSolution  "<<errorCode.enSolution;
        errorCodeHash.insert(errorCode.errorCode,errorCode);
    }
    CommonData::setErroCodes(errorCodeHash);
}

void MainWidget2::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton
            && e->x() < this->width()
            && e->y() < this->height())
    {
        this->setCursor(Qt::ClosedHandCursor);
        mouse_press = true;
    }
    move_point=e->globalPos()-this->pos();
}

void MainWidget2::mouseMoveEvent(QMouseEvent *e)
{
    if(mouse_press)
    {
        QPoint move_pos=e->globalPos();
        int absX = 0;
        if(move_point.x()>move_pos.x())
        {
            absX = move_point.x()-move_pos.x();
        }
        else
        {
            absX = move_pos.x()-move_point.x();
        }
        int absY = 0;
        if(move_point.y()>move_pos.y())
        {
            absY = move_point.y()-move_pos.y();
        }
        else
        {
            absY = move_pos.y()-move_point.y();
        }
        if(absY<8||absX<8)
        {
            return;
        }
        /*ui->btnMax->setChecked(false);
        ui->btnMax->clicked(false);*/
        this->move(move_pos-move_point);
    }
}

static int g_iClickCount = 0;
static QTime g_timeElapse;
static LuaDebugForm* g_LuaDebugForm = nullptr;
void MainWidget2::mouseReleaseEvent(QMouseEvent *e)
{
    mouse_press = false;
    this->setCursor(Qt::ArrowCursor);

    if (g_LuaDebugForm) return;
    QRect rc(0,this->height()-20,20,20);
    if (!rc.contains(e->pos())) return;
    if (0==g_iClickCount)
    {
        g_timeElapse.restart();
    }
    g_iClickCount++;
    if (g_iClickCount<10) return;
    int iElapse = g_timeElapse.elapsed();
    if (iElapse<5000)
    {//显示lua调试窗口
        auto pFrm = new LuaDebugForm();
        QPoint pt = mapToGlobal(rc.bottomLeft());
        pFrm->move(pt.x(), pt.y()-pFrm->height());
        pFrm->show();
        g_LuaDebugForm = pFrm;
        connect(pFrm, &LuaDebugForm::destroyed, this, []{
            g_LuaDebugForm = nullptr;
        });
    }
    g_iClickCount = 0;
}

void MainWidget2::closeEvent(QCloseEvent *e)
{
    slot_closeWindow();
    e->accept();
}

void MainWidget2::sleep(int milliseconds)
{
    QTime dieTime = QTime::currentTime().addMSecs(milliseconds);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWidget2::initUpgradeWidget()
{
    m_isControlLogBusyWidget = new IsControlLogBusyWidget(this);
    m_isControlLogBusyWidget->setGeometry(0,0,width(),height());
    m_isControlLogBusyWidget->hide();
    connect(m_isControlLogBusyWidget,&IsControlLogBusyWidget::signal_isContinueUpgrade,this,[&](bool isContinueUpgrade){
        if(isContinueUpgrade)
        {
            auto pLoading = new LoadingUI(this);
            pLoading->setGeometry(0,0,width(),height());
            pLoading->show();
            QString result = m_httpProtocol->getUpdateCheckPartitionOccupancy(g_strCurrentIP);
            pLoading->deleteLater();

            qDebug()<<" isContinueUpgrade --> "<<isContinueUpgrade<<" postUpdateCheckPartitionOccupancy result -->  "<<result;
            if(!result.isEmpty()&&result=="idle")
            {
                m_isControlLogBusyWidget->hide();
                widget2UpgradeDevice();
            }
        }
    });

    m_isDisableControlWidget = new IsDisableControlWidget(this);
    m_isDisableControlWidget->setGeometry(0,0,width(),height());
    m_isDisableControlWidget->hide();
    connect(m_isDisableControlWidget,&IsDisableControlWidget::signalDiableControlMode,this,[&]{
        bool isDisabled = m_httpProtocol->postSettingsControlMode(g_strCurrentIP,g_controlModeDisable);
        if(isDisabled)
        {
            m_isDisableControlWidget->hide();
        }
    });
    connect(m_isDisableControlWidget,&IsDisableControlWidget::signalDisconnect,this,[&]{
        ui->btnConnect->clicked();
    });

    showUpgradeServoParamAllWriteUI(false);
    ui->btnDownload->hide();
    ui->btnImport->hide();
    ui->btnConnect->hide();
    m_upgradeProgressTimer = new QTimer(this);
    connect(m_upgradeProgressTimer,&QTimer::timeout,this,&MainWidget2::slot_upgradeProgress);
    ui->labelTopControlType->hide();
    ui->btnUpgrade->hide();

    searchDeviceDisable(true);

    m_diyProgressDialog = new ProgressDialog(this);
    m_diyProgressDialog->setGeometry(0,0,width(),height());
    m_diyProgressDialog->hide();

    setHideServo6JWidget(true);
    connect(ui->btnServoAll6J,&QPushButton::clicked,this,[&](bool checked){
        if(checked)
        {
            setHideServo6JWidget(false);
        }
        else
        {
            setHideServo6JWidget(true);
        }
    });
    connect(ui->btnUpgradeWidgetServoParamAllWrite, &FormServoParamButton::signalCheckServoParam, this, &MainWidget2::slotCheckServoParam);
    connect(ui->btnUpgradeWidgetServoParamAllWrite,&FormServoParamButton::signalUpdateServoParam,this,[&]{
        if (m_bIsCheckingServoParam)
        {
            WidgetToast::warn(tr("请等待伺服参数对比完成"), 2000);
            return ;
        }
        QString strType=CommonData::getControllerType().name;
        QString strCtrlV = CommonData::getUpgradeSettingsVersion().control.split("-")[0];
        QString strServo = CommonData::getUpgradeSettingsVersion().servo1;
        auto pComfirm = new FormServoUpdateComfirm(this);
        pComfirm->setGeometry(0,0,width(),height());
        pComfirm->setText(tr("是否向%1写入，控制器%2版本相对应的%3伺服固件版本的伺服参数？").arg(strType,strCtrlV,strServo));
        pComfirm->raise();
        pComfirm->show();
        connect(pComfirm, &FormServoUpdateComfirm::signalOk, this, [this,pComfirm]{
            pComfirm->close();

            bool bNeedUpdate = false;
            QList<int> bJointNotEqual; //检测到不相等的伺服关节
            QString arrJointCSVFile[6]={"J1.csv","J1.csv","J1.csv","J1.csv","J1.csv","J1.csv"};
            auto pLoading = new LoadingUI(this);
            pLoading->setGeometry(0,0,width(),height());
            pLoading->setText("<font color='red'>"+tr("对比过程中请勿断开机器或者关闭窗口")+"</font>");
            pLoading->setBigText(tr("伺服参数对比"));
            pLoading->show();
            ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant());
            qDebug()<<"手动点击伺服参数覆盖写入前的伺服参数校验对比............checkSveroParameters";
            if (checkSveroParameters(bNeedUpdate, bJointNotEqual, arrJointCSVFile, true))
            {
                pLoading->close();
                if (!bNeedUpdate)
                {
                    ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant(bNeedUpdate));
                    auto ptr = new FormServoTips(this);
                    ptr->setText(tr("伺服参数为最新版本，无需更新！"));
                    ptr->show();
                }
                else
                {
                    qDebug()<<"手动点击了伺服参数覆盖写入............";
                    if (servoParamsAllWrite(arrJointCSVFile)){
                        ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant(false));
                    }else{
                        ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant(true));
                    }
                }
            }
            else
            {
                if (bNeedUpdate){//这种情况，就是其中某几个关节检测到需要更新，另外几个关节检测失败了
                    QString str;
                    for(auto j : bJointNotEqual){
                        str += QString::asprintf("J%d,",j);
                    }
                    str = QString(tr("机器本体的%1伺服参数与升级包的不一致")).arg(str);
                    ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant(),true,str);
                }else{
                    ui->btnUpgradeWidgetServoParamAllWrite->showServoParamLabel(false, QVariant(),true);
                }
            }
            pLoading->deleteLater();
        });
    });

    m_upgrade2Widget = new Upgrade2Widget(this);
    m_upgrade2Widget->setGeometry(0,0,this->width(),this->height());
    connect(m_upgrade2Widget, &Upgrade2Widget::signal_ExportLogWhenError, this, &MainWidget2::slot_ExportLogWhenError);
    connect(m_upgrade2Widget,&Upgrade2Widget::singal_upgradeSingle2Count,this,&MainWidget2::slot_upgradeWidgetUpgradeSingle2Count);
    connect(m_upgrade2Widget,&Upgrade2Widget::signal_interruptUpgradeUpgrade2Widget,this,[&]{
        m_isInterruptUpgrade = true;
        m_upgradeProgressTimer->stop();
        m_pFileTransfer->kill();
        DobotType::UpdateFirmware updateFirmware;
        updateFirmware.operation = "stop";
        m_httpProtocol->postUpdateCRFwAndXml(g_strCurrentIP,updateFirmware);
        m_upgradeProgressTimer->stop();
    });
    connect(m_upgrade2Widget,&Upgrade2Widget::signal_upgrade2WidgetFinishUpgrade,this,[&](int status){
        m_upgradeProgressTimer->stop();
        if(status == 2){ //升级成功
            m_isUpgradeSuccessStatus = 2;
        }
        else if(status == -1){//升级失败
            m_isUpgradeSuccessStatus = -1;
        }
        else if(status == 1){//重新升级
            upgradeAllFirmwareAndXMLs();
        }
    });
    m_upgrade2Widget->hide();

#ifndef V136_NOVA_UPDATE
    //初始化安装包下载功能
    m_upgradeFileZipDownload = new DownloadTool();
    connect(m_upgradeFileZipDownload,&DownloadTool::sigProgress,this,&MainWidget2::slot_downloadProgress);
    connect(m_upgradeFileZipDownload,&DownloadTool::sigDownloadFinished,this,&MainWidget2::slot_downloadFinished);
    ui->btnDownload->setCursor(QCursor(Qt::PointingHandCursor));
    connect(ui->btnDownload,&QPushButton::clicked,this,&MainWidget2::slot_download);

    //初始化全版本配置文件下载功能
    m_upgradeAllVersionIniDownload = new DownloadTool();
    connect(m_upgradeAllVersionIniDownload,&DownloadTool::sigDownloadFinished,this,&MainWidget2::slot_downloadAllVersionIniFinished);
    m_upgradeAllVersionIniDownload->startDownload(g_strHttpDobotPrefix+"/"+g_strAllVersion
                                                  ,QCoreApplication::applicationDirPath()+"/upgradeFiles");

    //updateconfig.json文件下载功能
    m_updateConfigJsonDownload = new DownloadTool();
    connect(m_updateConfigJsonDownload,&DownloadTool::sigDownloadFinished,this,&MainWidget2::slot_downloadUpdateConfigJsonFinished);
    m_updateConfigJsonDownload->startDownloadFile(getUpdateConfigJsonFileUrl(),getUpdateConfigJsonFile()+".down");

    //伺服模板、伺服关节电机类型对照表
    m_downloadServoJointTable = new DownloadTool();
    connect(m_downloadServoJointTable,&DownloadTool::sigDownloadFinished,this,&MainWidget2::slot_downloadServoJointTableFinished);
    m_downloadServoJointTable->startDownloadFile(getServoTemplateJointFileUrl(),getServoTemplateJointFileZIP());

    //伺获取控制器删除脚本
    m_downloadControllerScript = new DownloadTool();
    connect(m_downloadControllerScript,&DownloadTool::sigDownloadFinished,this,&MainWidget2::slot_downloadControllerScriptFinished);
    m_downloadControllerScript->startDownloadFile(getControllerScriptFileUrl(),getControllerScriptFileZIP());
#endif

    //初始化文件导入功能
    m_importTempVersionHandler = new CopyFolderAFiles();
    connect(m_importTempVersionHandler,&CopyFolderAFiles::signal_copyFolderFinished,this,&MainWidget2::slot_importFinishedTempVersion);
    connect(m_importTempVersionHandler,&CopyFolderAFiles::signal_copyFolderProgress,this,&MainWidget2::slot_importTempVersionProgress);
    ui->btnImport->setCursor(QCursor(Qt::PointingHandCursor));
    connect(ui->btnImport,&QPushButton::clicked,this,&MainWidget2::slot_importTempVersion);

    //初始化固件升级功能
    connect(ui->btnUpgrade,&QPushButton::clicked,this,&MainWidget2::slot_upgradeDevice);

    //初始化手动添加设备功能
    connect(ui->btnManuallyAdd,&QPushButton::clicked,this,&MainWidget2::slot_openManuallyIPWidget);
    m_ipList.append("192.168.5.1");
    m_ipList.append("192.168.200.1");
    ui->boxDevice->setDisabled(true);
    ui->boxDevice->addItem(tr("搜索设备中..."),QVariant("SearchDevices"));

    ui->btnConnect->setProperty("isConnected",-1);
    connect(ui->btnConnect,&QPushButton::clicked,this,&MainWidget2::slot_connectClicked);

    m_searchDeviceAndIsOnlineTimer = new QTimer(this);
    connect(m_searchDeviceAndIsOnlineTimer,&QTimer::timeout,this,&MainWidget2::slot_searchDevice);
    m_searchDeviceAndIsOnlineTimer->start(2500);
}

void MainWidget2::initFirmwareCheckWidget()
{
    m_item0 = new QListWidgetItem();
    m_firmwareCheckItem0 = new WidgetFirmwareCheckItem();
    m_firmwareCheckItem0->setBigFont();
    m_item0->setSizeHint(QSize(100,80));

    m_itemDns = new QListWidgetItem();
    m_firmwareCheckItemDns = new WidgetFirmwareCheckItem();
    connect(m_firmwareCheckItemDns,&WidgetFirmwareCheckItem::signal_firmwareCheckRepair,this,[&]{
        qDebug()<<"CommonData::getCurrentSettingsVersion().control ------- "<<CommonData::getCurrentSettingsVersion().control;
        if(m_httpProtocol->postFaultRepair(g_strCurrentIP,"dns","","","",CommonData::getCurrentSettingsVersion().control) == "success"){
            m_firmwareCheckItemDns->setLabelStatus(2);
        }else{
            m_firmwareCheckItemDns->setLabelStatus(-1);
        }
    });
    m_itemDns->setSizeHint(QSize(100,50));

    m_itemMacAddress = new QListWidgetItem();
    m_firmwareCheckItemMacAddress = new WidgetFirmwareCheckItem();
    connect(m_firmwareCheckItemMacAddress,&WidgetFirmwareCheckItem::signal_firmwareCheckRepair,this,[&]{
        if(m_httpProtocol->postFaultRepair(g_strCurrentIP,"mac",getCurrentUseAddress()) == g_strSuccess){
            m_firmwareCheckItemMacAddress->setLabelStatus(2);
        }else{
            m_firmwareCheckItemMacAddress->setLabelStatus(-1);
        }
    });
    m_itemMacAddress->setSizeHint(QSize(100,50));

    m_firmwareCheckItem0->setTheme("诊断");
    m_firmwareCheckItemDns->setTheme("DNS");
    m_firmwareCheckItemMacAddress->setTheme("Mac地址");

    ui->btnRestartDiagnose->hide();
    ui->btnAllRepair->hide();
}

void MainWidget2::initFirmwareCheckWidgetStatus()
{
    ui->btnDiagnose->show();
    ui->listWidgetFirmwareCheck->clear();
    initFirmwareCheckWidget();
}

void MainWidget2::initFileRecoveryWidget()
{
    ui->lineFileRecoveryDir->setText(QString());
}

void MainWidget2::initSystemToolWidget()
{
    m_widgetSystemToolTip = new WidgetSystemToolTip(this);
    m_widgetSystemToolTip->hide();
    ui->labelMACStatus->setStyleSheet(
                "QLabel[state='ok'] {border:none;color: #3EB72A;text-align:right;font-size: 20px;border:none;}"
                        "QLabel[state='fail'] {border:none;color: #CE4949;text-align:right;font-size: 20px;border:none;}"
                    );

    ui->labelMACStatus->hide();
    ui->labelDNSStatus->setStyleSheet(
                "QLabel[state='ok'] {border:none;color: #3EB72A;text-align:right;font-size: 20px;border:none;}"
                        "QLabel[state='fail'] {border:none;color: #CE4949;text-align:right;font-size: 20px;border:none;}"
                    );
    ui->labelDNSStatus->hide();

    connect(ui->btnDNSDetect,&QPushButton::clicked,this,[&]{
        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
        {
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("该机型暂未开放此功能"));
            pWndMsg->show();
            return;
        }
        if(m_httpProtocol->postFaultCheck(g_strCurrentIP,"dns").result=="success"){
            ui->labelDNSStatus->setText(tr("正常"));
            this->style()->unpolish(ui->labelDNSStatus); //清除旧的样式
            ui->labelDNSStatus->setProperty("state","ok");
            this->style()->polish(ui->labelDNSStatus);   //更新为新的样式
        }else{
            ui->labelDNSStatus->setText(tr("异常"));
            this->style()->unpolish(ui->labelDNSStatus); //清除旧的样式
            ui->labelDNSStatus->setProperty("state","fail");
            this->style()->polish(ui->labelDNSStatus);   //更新为新的样式
        }
        ui->labelDNSStatus->show();
        ui->btnDNSDetect->hide();
    });

    connect(ui->btnMACDetect,&QPushButton::clicked,this,[&]{
        m_allNetworkCardInfo.clear();
        ui->labelMACName->setText("");
        if(m_httpProtocol->postFaultCheck(g_strCurrentIP,"mac").result=="success"){
            ui->labelMACStatus->setText(tr("正常"));
            this->style()->unpolish(ui->labelMACStatus); //清除旧的样式
            ui->labelMACStatus->setProperty("state","ok");
            this->style()->polish(ui->labelMACStatus);   //更新为新的样式

            m_allNetworkCardInfo = m_httpProtocol->getNetworkCardMac(g_strCurrentIP);
            if (m_allNetworkCardInfo.size()>0)
            {
                for(int i=0; i<m_allNetworkCardInfo.size();++i)
                {
                    if (m_allNetworkCardInfo.at(i).first == "eth2")
                    {
                        ui->labelMACName->setText(m_allNetworkCardInfo.at(i).second);
                        break;
                    }
                }
                if (ui->labelMACName->text().isEmpty())
                {
                    ui->labelMACName->setText(m_allNetworkCardInfo.at(0).second);
                }
            }
        }else{
            ui->labelMACStatus->setText(tr("异常"));
            this->style()->unpolish(ui->labelMACStatus); //清除旧的样式
            ui->labelMACStatus->setProperty("state","fail");
            this->style()->polish(ui->labelMACStatus);   //更新为新的样式
        }
        ui->labelMACStatus->show();
        ui->btnMACDetect->hide();
        if (m_allNetworkCardInfo.isEmpty())
        {
            ui->labelMACName->hide();
            ui->btnSystemMacView->hide();
        }
        else
        {
            ui->labelMACName->show();
            ui->btnSystemMacView->show();
        }
    });

    connect(ui->btnSystemMacView, &QPushButton::clicked, this, &MainWidget2::slot_btnSystemMacView);

    connect(ui->btnSystemSynTime,&QPushButton::clicked,this,[&]{
        if(true) //不用输入验证码可以直接查看。m_isPasswordConfirm
        {
            systemToolSyncTime();
        }
        else
        {
            auto ptr = new WidgetServoParamsPassword(this);
            ptr->setGeometry(0,0,width(),height());
            connect(ptr,&WidgetServoParamsPassword::signal_servoParamsPassword,this,[this,ptr]{
                m_isPasswordConfirm = true;
                systemToolSyncTime();
                ptr->deleteLater();
            });
            ptr->show();
        }
    });

    connect(ui->btnSystemFileCheck,&QPushButton::clicked,this,[&]{
        auto ptrWidget = new WidgetSystemFileCheckTip(this);
        ptrWidget->setGeometry(0,0,width(),height());
        ptrWidget->setMessage(SystemFileCheckTipStatus::FileCheck_DOING);
        ptrWidget->show();
        DobotType::StructFaultCheck structFaultCheck = m_httpProtocol->postFaultCheck(g_strCurrentIP,"files");
        if(structFaultCheck.result != "success")
        {
            ptrWidget->setMessage(SystemFileCheckTipStatus::FileCheck_FAIL,structFaultCheck.missFiles);
        }
        else
        {
            ptrWidget->setMessage(SystemFileCheckTipStatus::FileCheck_SUCCESS);
            qDebug()<<"error_file.txt not exist";
        }
    });

    connect(ui->btnSystemLaserCab,&QPushButton::clicked,this,[&]{
        const QString strDir = getControllerScriptDir();
        const QString strBat = "delete.bat";
        QFileInfo f(strDir+strBat);
        if (!f.exists() || !f.isFile())
        {
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("清空脚本及日志失败，没有找到批处理文件"));
            pWndMsg->show();
            return;
        }

        auto pLoading = new LoadingUI(this);
        pLoading->setGeometry(0,0,width(),height());
        pLoading->show();
        QProcess* proc = new QProcess();
        QObject::connect(proc,&QProcess::errorOccurred,[proc,pLoading](QProcess::ProcessError error){
            qWarning()<<proc->arguments()<<", execute fail:" <<error;
            proc->deleteLater();
            pLoading->deleteLater();
            auto* pWndMsg = new MessageWidget(PublicgGetMainWindow());
            pWndMsg->setMessage("warn",tr("清空脚本及日志失败"));
            pWndMsg->show();
        });
        QObject::connect(proc,&QProcess::readyRead,[proc]{
            qDebug()<<"delete script read:"<<QString::fromLocal8Bit(proc->readAll());
        });
        QObject::connect(proc,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[pLoading](int exitCode,QProcess::ExitStatus exitStatus){
            qDebug()<<"delete script finished!  " <<exitCode <<" exitStatus "<<exitStatus;
            pLoading->deleteLater();
            auto* pWndMsg = new MessageWidget(PublicgGetMainWindow());
            pWndMsg->setMessage("warn",tr("清空脚本及日志成功"));
            pWndMsg->show();
        });
        QObject::connect(proc, SIGNAL(finished), proc, SLOT(deleteLater));
        proc->setProgram("cmd.exe");
        proc->setArguments(QStringList()<<"/c"<<strBat<<g_strCurrentIP);
        proc->setWorkingDirectory(strDir);
        proc->start();
    });

    connect(m_widgetSystemToolTip,&WidgetSystemToolTip::signal_widgetSystemToolWifiIpRevovery,this,[&]{
        sleep(500);
        if(m_httpProtocol->postFaultRepair(g_strCurrentIP,"defaultIp","","","",CommonData::getCurrentSettingsVersion().control) == "success"){
            m_widgetSystemToolTip->setRepairStatus(4);
        }else{
            m_widgetSystemToolTip->setRepairStatus(-1);
        }
    });
    connect(ui->btnSystemDNSRepair,&QPushButton::clicked,this,[&]{
        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
        {
            auto* pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("该机型暂未开放此功能"));
            pWndMsg->show();
            return;
        }
        m_widgetSystemToolTip->show();
        m_widgetSystemToolTip->setRepairStatus(1);
        if(m_httpProtocol->postFaultRepair(g_strCurrentIP,"dns","","","",CommonData::getCurrentSettingsVersion().control)=="success"){
            m_widgetSystemToolTip->setRepairStatus(21);
            ui->labelDNSStatus->setText(tr("正常"));
            this->style()->unpolish(ui->labelDNSStatus); //清除旧的样式
            ui->labelDNSStatus->setProperty("state","ok");
            this->style()->polish(ui->labelDNSStatus);   //更新为新的样式
        }else{
            m_widgetSystemToolTip->setRepairStatus(-1);
        }
    });

    connect(ui->btnSystemMacRepair,&QPushButton::clicked,this,[&]{
        m_widgetSystemToolTip->show();
        m_widgetSystemToolTip->setRepairStatus(1);
        QString currentUseAddress = getCurrentUseAddress();
        if(m_httpProtocol->postFaultRepair(g_strCurrentIP,"mac",currentUseAddress)=="success"){
            QString strMac;
            if (m_allNetworkCardInfo.size()>0)
            {
                for(int i=0; i<m_allNetworkCardInfo.size();++i)
                {
                    if (m_allNetworkCardInfo.at(i).first == "eth2")
                    {
                        strMac = m_allNetworkCardInfo.at(i).second;
                        break;
                    }
                }
                if (strMac.isEmpty())
                {
                    strMac = m_allNetworkCardInfo.at(0).second;
                }
            }
            if (strMac.isEmpty())
            {
                strMac = "000EC69"+currentUseAddress;
            }
            m_widgetSystemToolTip->setMacAddress(strMac);
            m_widgetSystemToolTip->setRepairStatus(22);
            ui->labelMACStatus->setText(tr("正常"));
            this->style()->unpolish(ui->labelMACStatus); //清除旧的样式
            ui->labelMACStatus->setProperty("state","ok");
            this->style()->polish(ui->labelMACStatus);   //更新为新的样式
        }else{
            m_widgetSystemToolTip->setRepairStatus(-1);
        }
    });

    connect(ui->btnSystemOverTurnRepair,&QPushButton::clicked,this,[&]{
        m_widgetSystemToolTip->show();
        m_widgetSystemToolTip->setRepairStatus(1);
        if(m_httpProtocol->postFaultRepair(g_strCurrentIP,"overTurn","","","",CommonData::getCurrentSettingsVersion().control)=="success"){
            m_widgetSystemToolTip->setRepairStatus(21);
        }else{
            m_widgetSystemToolTip->setRepairStatus(-1);
        }
    });

    connect(ui->btnSystemDefaultIpRepair,&QPushButton::clicked,this,[&]{
        m_widgetSystemToolTip->show();
        m_widgetSystemToolTip->setRepairStatus(3);
    });

    connect(ui->btnSystemLogExport,&QPushButton::clicked,this,[&]{
        SFtpOptFile opt;
        opt.id = SystemLogExport_OPT_LISTDIR;
        opt.strRemotePath = "/dobot/userdata/project/project";
        opt.strLocalPath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/files";
        opt.opt = SFtpOption::OPT_LISTDIR;
        m_csFtpClient->ssh(opt);
    });
    connect(ui->btnDownloadRecovery,&QPushButton::clicked,this,&MainWidget2::slot_DownloadRecovery);
    connect(ui->btnRecoveryFactory,&QPushButton::clicked,this,&MainWidget2::slot_RecoveryFactory);
}

void MainWidget2::slot_SystemLogExport(QString projectName,QString dstExportDir)
{
    QPointer<Widget2ProgressDialog> ptProgressDlg =new Widget2ProgressDialog(this);
    ptProgressDlg->setGeometry(0,0,width(),height());
    ptProgressDlg->setWarnMessage("");
    ptProgressDlg->setTitle(tr("日志导出"));
    ptProgressDlg->setHeadLabel("export");
    ptProgressDlg->setGeometry(0,0,width(),height());
    ptProgressDlg->setProgressRange(300);
    ptProgressDlg->show();
    ptProgressDlg->setProgressValue(1);

    QFile fileBat(QCoreApplication::applicationDirPath()+"/tool/logExport/dobotLogExport.bat");
    if (fileBat.open(QFile::Truncate|QFile::WriteOnly)){
        QString strTargz = CommonData::getControllerType().name.remove(QRegExp("\\s"))+"_";
        strTargz += CommonData::getStrPropertiesCabinetType().name.remove(QRegExp("\\s"))+"_";
        strTargz += CommonData::getCurrentSettingsVersion().control+"_project";
        QTextStream fos(&fileBat);
        fos.setCodec("UTF-8");
        fos<<"@echo off\r\n"
             "chcp 65001\r\n"
             "echo \"start UpdateTool\"\r\n"
             "cd ./tool/logExport\r\n"
             "cd /d %~dp0\r\n"
             "rd /s /q cache\r\n"
             "md cache\r\n";
        fos<<"(\r\n"
             "echo cd /dobot/userdata/project\r\n"
             "echo rm -rf dobotlogexportname.tar.gz\r\n";
        fos<<QString("echo tar czvf dobotlogexportname.tar.gz logs/ \"project/%1/\"\r\n").arg(projectName);
        fos<<"echo exit\r\n";
        fos<<QString(") | plink.exe -ssh -l root -pw dobot %1\r\n").arg(g_strCurrentIP);
        fos<<QString("pscp.exe -pw dobot -r root@%1:/dobot/userdata/project/dobotlogexportname.tar.gz ./cache\r\n").arg(g_strCurrentIP);
        fos<<"(\r\n"
             "echo cd /dobot/userdata/project\r\n"
             "echo rm -rf dobotlogexportname.tar.gz\r\n"
             "echo exit\r\n";
        fos<<QString(") | plink.exe -ssh -l root -pw dobot %1\r\n").arg(g_strCurrentIP);
        fos<<"cd ./cache\r\n"
             "echo \"begin rename file...\"\r\n";
        fos<<QString("ren \"dobotlogexportname.tar.gz\" \"%1.tar.gz\"\r\n").arg(strTargz);
        fos<<"echo \"logExport successfully.\"\r\n";
        fileBat.close();
    }
    else
    {
        qDebug()<<fileBat.fileName();
        qDebug()<<"open/create dobotLogExport.bat file error:"<<fileBat.errorString();
    }

    QProcess* pLogProg = new QProcess();
    static int progressValue = 1;
    QObject::connect(pLogProg,&QProcess::errorOccurred,[&,ptProgressDlg](QProcess::ProcessError error){
        qWarning()<<"LogProg slot_ errorOccurred!" <<error;
        progressValue = 0;
        ptProgressDlg->setHeadLabel("fail");
        pLogProg->deleteLater();
    });
    QObject::connect(pLogProg,&QProcess::readyRead,[&,ptProgressDlg]{
        progressValue +=1;
        if (ptProgressDlg) ptProgressDlg->setProgressValue(progressValue);
    });
    QObject::connect(pLogProg,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<"m_widgetSystemLogExport finished!  " <<exitCode <<" exitStatus "<<exitStatus;
        QString cmdResult = pLogProg->readAll();
        qDebug()<< "m_widgetSystemLogExport cmdResult "<<cmdResult;
        bool bSuccess = false;
        if(cmdResult.contains("logExport successfully")){
            //机器型号_控制柜型号_控制器版本_导出日期时间_IL
            DobotType::ControllerType controllerType = m_httpProtocol->getControllerType(g_strCurrentIP);
            //获取当前时间
            QDateTime current_time = QDateTime::currentDateTime();
            QString currentTime = current_time.toString("yyyyMMdd");
            QFile file(QCoreApplication::applicationDirPath()+"/tool/logExport/cache" + "/"+controllerType.name+"_"+currentTime+"_VN.txt");
            if(file.open(QIODevice::ReadWrite|QIODevice::Text)){
                QString temp; // 写入内容
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMG400)
                   ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
                {
                    temp+= "E6控制柜:\n";
                    temp+= "控制器版本: "+CommonData::getCurrentSettingsVersion().control+"\n";
                    temp+= "系统版本:"+CommonData::getCurrentSettingsVersion().system+"\n";
                    temp+= "伺服:"+CommonData::getCurrentSettingsVersion().servo1+"\n";
                    temp+= "末端IO:"+CommonData::getCurrentSettingsVersion().terminal+"\n";
                    temp+= "通用IO:"+CommonData::getCurrentSettingsVersion().unio+"\n";
                }
                else if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
                {
                    temp+= "小型控制柜:\n";
                    temp+= "控制器版本: "+CommonData::getCurrentSettingsVersion().control+"\n";
                    temp+= "系统版本:"+CommonData::getCurrentSettingsVersion().system+"\n";
                    temp+= "伺服:"+CommonData::getCurrentSettingsVersion().servo1+"\n";
                    temp+= "末端IO:"+CommonData::getCurrentSettingsVersion().terminal+"\n";
                    temp+= "CCBOX:"+CommonData::getCurrentSettingsVersion().unio+"\n";
                }
                else
                {
                    temp+= "大型控制柜:\n";
                    temp+= "控制器版本: "+CommonData::getCurrentSettingsVersion().control+"\n";
                    temp+= "系统版本:"+CommonData::getCurrentSettingsVersion().system+"\n";
                    temp+= "馈能板:"+CommonData::getCurrentSettingsVersion().feedback+"\n";
                    temp+= "安全IO:"+CommonData::getCurrentSettingsVersion().safeio+"\n";
                    temp+= "通用IO:"+CommonData::getCurrentSettingsVersion().unio+"\n";
                    temp+= "伺服:"+CommonData::getCurrentSettingsVersion().servo1+"\n";
                    temp+= "末端IO:"+CommonData::getCurrentSettingsVersion().terminal+"\n";
                }
                // 将内容写入文件
                QTextStream out(&file);
                out.setCodec("UTF-8");
                out << temp;
                file.close();
                qDebug()<<"completed.";
            }else{
                qDebug()<<"failed to create a new file!";
            }
            const QString strSrc=QCoreApplication::applicationDirPath()+"/tool/logExport/cache";
            const QString strDstZip=dstExportDir+"/"+controllerType.name.remove(QRegExp("\\s"))+"_"
                                    +CommonData::getStrPropertiesCabinetType().name.remove(QRegExp("\\s"))+"_"
                                    +CommonData::getCurrentSettingsVersion().control+"_"+currentTime+"_IL.rar";
            qDebug()<<"srcDir "<<strSrc;
            qDebug()<<"dstZip "<<strDstZip;
            bSuccess = m_zipDiyManager->zipDiyCompress(strSrc,strDstZip,m_zipLogExportPassword);
        }else{
            qDebug()<<"m_widgetSystemLogExport 日志导出 失败";
        }
        progressValue = 0;
        if (ptProgressDlg){
            ptProgressDlg->setProgressValue(ptProgressDlg->progressRange());
            ptProgressDlg->setHeadLabel(bSuccess?"success":"fail");
        }
        pLogProg->deleteLater();//new code
    });
    pLogProg->setProgram(fileBat.fileName());
    pLogProg->start();
}

void MainWidget2::initSystemToolDectect()
{
    ui->labelMACStatus->hide();
    ui->labelDNSStatus->hide();
    ui->btnDNSDetect->show();
    ui->btnMACDetect->show();
    ui->labelMACName->hide();
    ui->btnSystemMacView->hide();
}

void MainWidget2::showSingleUpgradeWidget(QString fwOrXML)
{
    auto ptr = m_widgetSingleUpgrade;
    ptr->searchMenuItems();
    ptr->setSingleUpgradeFwOrXML(fwOrXML);
    m_strLabelSingleFwAndXML = fwOrXML;
    ptr->setGeometry(0,0,width(),height());
    ptr->raise();
    ptr->show();
}

void MainWidget2::initSingleUpgradeWidget()
{
    m_widgetSingleUpgrading = new WidgetSingleUpgrading(this);
    m_widgetSingleUpgrading->setGeometry(0,0,width(),height());
    m_widgetSingleUpgrading->hide();

    m_widgetSingleUpgradeMainControl = new WidgetSingleUpgradeMainControl(this);
    m_widgetSingleUpgradeMainControl->hide();
    m_widgetSingleUpgradeMainControl->setGeometry(0,0,width(),height());
    connect(m_widgetSingleUpgradeMainControl,&WidgetSingleUpgradeMainControl::signalUpgradeSingleMainControl,this,[&](QString upgradeMainControlFilePath){
        if(upgradeMainControlFilePath.isEmpty())
        {
            auto pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("未选择升级的主控板"));
            pWndMsg->show();
            return;
        }

        QStringList projectList;
        auto allMainFiles = CLuaApi::getInstance()->getControllerUpdateFiles();
        if (allMainFiles.hasValue())
        {
            QStringList allFiles = allMainFiles.value();
            for (QString str : allFiles)
            {
                const QString strPath = upgradeMainControlFilePath+"/src/"+str;
                QFileInfo info(strPath);
                if(!info.exists() || !info.isFile())
                {
                    qDebug()<<"lua MainControlDir don't have a "<<strPath;
                    auto* pWndMsg = new MessageWidget(this);
                    pWndMsg->setMessage("warn",QString(tr("缺失文件%1: ")).arg(strPath));
                    pWndMsg->show();
                    return;
                }
                projectList.append(strPath);
            }
        }
        else
        {
            QString upgradeMainControlFile = upgradeMainControlFilePath+"/src/update.tar.gz";
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)
                    ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
            {
                upgradeMainControlFile = upgradeMainControlFilePath+"/src/bin.tar.gz";
            }
            if(!QFile::exists(upgradeMainControlFile))
            {
                qDebug()<<"MainControlDir don't have a "<<upgradeMainControlFile;
                auto* pWndMsg = new MessageWidget(this);
                pWndMsg->setMessage("warn",QString(tr("缺失文件%1: ")).arg(upgradeMainControlFile));
                pWndMsg->show();
                return;
            }
            projectList.append(upgradeMainControlFile);

            QString mainControlSHPath = upgradeMainControlFilePath+"/src/afterUpdate.sh";
            if(QFile::exists(mainControlSHPath)){ projectList.append(mainControlSHPath); }
            mainControlSHPath = upgradeMainControlFilePath+"/src/dobot.sh";
            if(QFile::exists(mainControlSHPath)){ projectList.append(mainControlSHPath); }

            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
            {
                QString upgradeE6PythonFile = upgradeMainControlFilePath+"/src/python_env.tar.gz";
                if(!QFile::exists(upgradeE6PythonFile))
                {
                    qDebug()<<"MainControlDir don't have a file MagicianE6---> "<<upgradeE6PythonFile;
                }
                projectList.append(upgradeE6PythonFile);
            }
        }
        {
            QString str=QDir::toNativeSeparators(upgradeMainControlFilePath);
            QStringList lst = str.split(QDir::separator());
            str = lst.back();
            auto info = getMatchCRUpdateInfo(str);
            str = str.mid(info.index+info.length);
            m_singleUpgradeMainControl = str.left(str.indexOf('-'));
        }
        qDebug()<<"m_singleUpgradeMainControl  ======= "<<m_singleUpgradeMainControl;

        m_widgetSingleUpgradeMainControl->hide();
        m_widgetSingleUpgrading->setSingleUpgradeStatus(1);
        m_widgetSingleUpgrading->show();
        //单个主控板升级压缩包的传输
        QEventLoop eventloop;
        QProcess* pMainControlSingleProcess = new QProcess();
        connect(pMainControlSingleProcess, &QProcess::readyRead, this, [&]{
            qDebug()<<"SingleUpgradeMainControl-->"<<pMainControlSingleProcess->readAll();
        });
        connect(pMainControlSingleProcess,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[&](int exitCode,QProcess::ExitStatus exitStatus){
            qDebug()<<"pMainControlProcess,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished)  ";
            //主控板压缩包传输完成后，请求主控板单个升级
            DobotType::StructCRSingleFwAndXml singleCRFwAndXml;
            auto cabTypeLua = CLuaApi::getInstance()->getCabType();
            if (cabTypeLua.hasValue())
            {
                singleCRFwAndXml.cabType = cabTypeLua.value();
            }
            else{
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)
                        ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162))
                {
                    singleCRFwAndXml.cabType = "big";
                }
                else
                {
                    singleCRFwAndXml.cabType = "small";
                }
            }

            auto cabVersionLua = CLuaApi::getInstance()->getCabVersion();
            if (cabVersionLua.hasValue())
            {
                singleCRFwAndXml.cabVersion = cabVersionLua.value();
            }
            else{
                if(CommonData::getStrPropertiesCabinetType().name==g_strPropertiesCabinetTypeCC162
                        ||CommonData::getStrPropertiesCabinetType().name==g_strPropertiesCabinetTypeCCBOX)
                {
                    singleCRFwAndXml.cabVersion = "V1";
                }
                else
                {
                    singleCRFwAndXml.cabVersion = "V2";
                }
            }
            auto slaveIdLua = CLuaApi::getInstance()->getHardwareSlaveId();
            if (slaveIdLua.hasValue())
            {
                const auto& kv = slaveIdLua.value();
                singleCRFwAndXml.slaveId = kv.value("control", 0);
            }
            else{
                singleCRFwAndXml.slaveId = 0;
            }
            singleCRFwAndXml.operationType = "start";
            singleCRFwAndXml.updateType = "firmware";
            singleCRFwAndXml.updateControlVersion = m_singleUpgradeMainControl;
            QString result = m_httpProtocol->postUpdateCRSingleFwAndXml(g_strCurrentIP,singleCRFwAndXml);
            if(result.isEmpty() || result != g_strSuccess){
                m_widgetSingleUpgrading->setSingleUpgradeStatus(-1);
            }
            pMainControlSingleProcess->terminate();
            pMainControlSingleProcess->kill();
            pMainControlSingleProcess->deleteLater();
            eventloop.quit();
        });
        pMainControlSingleProcess->setProgram(QCoreApplication::applicationDirPath()+"/tool/plink/pscp.exe");
        QStringList argsUpload;
        argsUpload<<"-pw"<<"dobot"<<"-r"<<projectList<<QString("root@%1:%2").arg(g_strCurrentIP,m_strProtocolVersionControl);
        pMainControlSingleProcess->setArguments(argsUpload);
        pMainControlSingleProcess->start();
        eventloop.exec();
        pMainControlSingleProcess->deleteLater();

        //实时获取主控板升级状态
        qDebug()<<"开始实时获取主控板升级状态 signalUpgradeSingleMainControl......";
        QEventLoop getSingleMainProcessStatusEventloop;
        QTimer* getCRSingleStatusTimer = new QTimer();
        getCRSingleStatusTimer->setSingleShot(true);
        connect(getCRSingleStatusTimer,&QTimer::timeout,this,[&]{
            DobotType::StructGetCRSingleStatus getCRSingleStatus = m_httpProtocol->getUpdateSingleStatus(g_strCurrentIP);
            getCRSingleStatusTimer->start(1000);
            if(getCRSingleStatus.result != 0 && getCRSingleStatus.result != 1)
            {
                QString errorMsg;
                DobotType::StructErrorCode structErrorCode;
                if(getCRSingleStatus.result == -1 && getCRSingleStatus.errorCode != 0)
                {
                    structErrorCode = CommonData::getErrorCodes().value(getCRSingleStatus.errorCode);
                    QString description;
                    QString solution;
                    if(g_isEnglish)
                    {
                        description = structErrorCode.enDescription;
                        solution = "solution: "+structErrorCode.enSolution;
                    }
                    else
                    {
                        description = structErrorCode.zhDescription;
                        solution = "解决方案: "+structErrorCode.zhSolution;
                    }
                    m_widgetSingleUpgrading->setSingleUpgradeStatus(getCRSingleStatus.result
                                                                    ,QString("%1").arg(structErrorCode.errorCode)+" "+description+"\n"+solution);
                }
                else
                {
                    if(getCRSingleStatus.result == 2)
                    {
                        CommonData::setMainControlStatus(2);
                    }
                    m_widgetSingleUpgrading->setSingleUpgradeStatus(getCRSingleStatus.result);
                }
                qDebug()<<"getCRSingleStatus.result --> "<<getCRSingleStatus.result;
                getSingleMainProcessStatusEventloop.quit();
            }
        });
        getCRSingleStatusTimer->start(1000);
        getSingleMainProcessStatusEventloop.exec();
        getCRSingleStatusTimer->deleteLater();
    });

    connect(ui->btnSingleMainControl,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getMainControlStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgradeMainControl->setGeometry(0,0,width(),height());
        m_widgetSingleUpgradeMainControl->show();
        m_widgetSingleUpgradeMainControl->searchMenuItems();
    });

    connect(ui->btnSingleServoUpgradeAllFW,&QPushButton::clicked,this,[&]{
        if(CommonData::getServoJ1FOEStatus() == 2
                &&CommonData::getServoJ2FOEStatus() == 2
                &&CommonData::getServoJ3FOEStatus() == 2
                &&CommonData::getServoJ4FOEStatus() == 2
                &&CommonData::getServoJ5FOEStatus() == 2
                &&CommonData::getServoJ6FOEStatus() == 2)
        {
            setSingleFuncUpgradeStatus(2);
            return;
        }
        showSingleUpgradeWidget("ServoJALL");
    });

    //btnSingleServoUpgradeAllXML
    connect(ui->btnSingleServoUpgradeAllXML,&QPushButton::clicked,this,[&]{
        if(CommonData::getServoJ1XMLStatus() == 2
                &&CommonData::getServoJ2XMLStatus() == 2
                &&CommonData::getServoJ3XMLStatus() == 2
                &&CommonData::getServoJ4XMLStatus() == 2
                &&CommonData::getServoJ5XMLStatus() == 2
                &&CommonData::getServoJ6XMLStatus() == 2)
        {
            setSingleFuncUpgradeStatus(2);
            return;
        }
        showSingleUpgradeWidget("ServoJALLXML");
    });

    m_singleUpgradeTimer = new QTimer();
    connect(m_singleUpgradeTimer,&QTimer::timeout,this,[&]{
        DobotType::StructGetCRSingleStatus getCRSingleStatus;
        getCRSingleStatus = m_httpProtocol->getUpdateSingleStatus(g_strCurrentIP);
        qDebug()<<"m_singleUpgradeTimer  status "<<getCRSingleStatus.result;
        static int servoCount = 1;
        if(getCRSingleStatus.result == -1)
        {
            if(m_strLabelSingleFwAndXML.contains("ALL"))
            {
                servoCount = 1;
            }
            DobotType::StructErrorCode structErrorCode;
            structErrorCode = CommonData::getErrorCodes().value(getCRSingleStatus.errorCode);
            QString description;
            QString solution;
            if(g_isEnglish)
            {
                description = structErrorCode.enDescription;
                solution = "solution: "+structErrorCode.enSolution;
            }
            else
            {
                description = structErrorCode.zhDescription;
                solution = "解决方案: "+structErrorCode.zhSolution;
            }
            qDebug()<<"structErrorCode.errorCode +description ---- " <<QString("%1").arg(structErrorCode.errorCode)+" "+description+"\n"+solution;
            m_widgetSingleUpgrading->setSingleUpgradeStatus(getCRSingleStatus.result
                                                            ,QString("%1").arg(structErrorCode.errorCode)+" "+description+"\n"+solution);
            m_singleUpgradeTimer->stop();
            return;
        }
        //m_servoSlaveId 升级6个伺服，不是最后一个时，一旦成功继续升级下一个
        if(getCRSingleStatus.result == 2&&m_strLabelSingleFwAndXML.contains("ALL")&&servoCount<6)
        {
            qDebug()<<"servoCount conitue ************  "<<servoCount;
            m_singleCRFwAndXml.slaveId = m_jallServoId.takeFirst();
            QString result = m_httpProtocol->postUpdateCRSingleFwAndXml(g_strCurrentIP,m_singleCRFwAndXml);
            sleep(1000);
            getCRSingleStatus = m_httpProtocol->getUpdateSingleStatus(g_strCurrentIP);
            if(result != g_strSuccess){
                m_widgetSingleUpgrading->setSingleUpgradeStatus(-1);
                m_singleUpgradeTimer->stop();
                return ;
            }
            else
            {
                if(servoCount != 6)
                {
                    servoCount++;
                    qDebug()<<"servoCount++  "<<servoCount;
                }
            }
        }
        //升级6个伺服，获取最后一个升级的状态
        if(servoCount == 6 && (getCRSingleStatus.result == -1||getCRSingleStatus.result == 2))
        {
            //针对六个轴的升级记录，防止下次升级
            if(getCRSingleStatus.result == 2)
            {
                if(!m_strLabelSingleFwAndXML.contains("XML"))
                {
                    CommonData::setServoJ1FOEStatus(2);
                    CommonData::setServoJ2FOEStatus(2);
                    CommonData::setServoJ3FOEStatus(2);
                    CommonData::setServoJ4FOEStatus(2);
                    CommonData::setServoJ5FOEStatus(2);
                    CommonData::setServoJ6FOEStatus(2);
                }
                else
                {
                    CommonData::setServoJ1XMLStatus(2);
                    CommonData::setServoJ2XMLStatus(2);
                    CommonData::setServoJ3XMLStatus(2);
                    CommonData::setServoJ4XMLStatus(2);
                    CommonData::setServoJ5XMLStatus(2);
                    CommonData::setServoJ6XMLStatus(2);
                }
            }
            servoCount = 1;
            qDebug()<<"servoCount = 1  "<<servoCount;
            m_widgetSingleUpgrading->setSingleUpgradeStatus(getCRSingleStatus.result);
            m_singleUpgradeTimer->stop();
            return;
        }
        qDebug()<<"servoCount  end $$$$$$$$$$$  "<<servoCount;
        if(getCRSingleStatus.result != 1 && getCRSingleStatus.result != 0 && !m_strLabelSingleFwAndXML.contains("ALL"))
        {

            QString errorMsg;
            DobotType::StructErrorCode structErrorCode;
            if(getCRSingleStatus.result == -1 && getCRSingleStatus.errorCode != 0)
            {
                structErrorCode = CommonData::getErrorCodes().value(getCRSingleStatus.errorCode);
                QString description;
                QString solution;
                if(g_isEnglish)
                {
                    description = structErrorCode.enDescription;
                    solution = "solution: "+structErrorCode.enSolution;
                }
                else
                {
                    description = structErrorCode.zhDescription;
                    solution = "解决方案: "+structErrorCode.zhSolution;
                }
                qDebug()<<"structErrorCode.errorCode +description ---- "
                        <<QString("%1").arg(structErrorCode.errorCode)+" "+description+"\n"+solution;
                m_widgetSingleUpgrading->setSingleUpgradeStatus(getCRSingleStatus.result
                                                                ,QString("%1").arg(structErrorCode.errorCode)+" "+description+"\n"+solution);
            }
            else
            {
                //单独升级过一次不允许再次升级
                if(getCRSingleStatus.result == 2)
                {
                    if(m_strLabelSingleFwAndXML=="UniIO")
                    {
                        CommonData::setUniIOFOEStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="UniIOXML")
                    {
                        CommonData::setUniIOXMLStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="FeedBack")
                    {
                        CommonData::setFeedbackStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="CCBOX")
                    {
                        CommonData::setCCBOXFOEStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="CCBOXXML")
                    {
                        CommonData::setCCBOXXMLStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML == "SafeIOXML")
                    {
                        CommonData::setSafeIOXMLStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML == "SafeIO")
                    {
                        CommonData::setSafeIOFOEStatus(2);
                        CommonData::setSafeBFOEStatus(2);
                    }

                    if(m_strLabelSingleFwAndXML=="ServoJ1")
                    {
                        CommonData::setServoJ1FOEStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="ServoJ1XML")
                    {
                        CommonData::setServoJ1XMLStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="ServoJ2")
                    {
                        CommonData::setServoJ2FOEStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="ServoJ2XML")
                    {
                        CommonData::setServoJ2XMLStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="ServoJ3")
                    {
                        CommonData::setServoJ3FOEStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="ServoJ3XML")
                    {
                        CommonData::setServoJ3XMLStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="ServoJ4")
                    {
                        CommonData::setServoJ4FOEStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="ServoJ4XML")
                    {
                        CommonData::setServoJ4XMLStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="ServoJ5")
                    {
                        CommonData::setServoJ5FOEStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="ServoJ5XML")
                    {
                        CommonData::setServoJ5XMLStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="ServoJ6")
                    {
                        CommonData::setServoJ6FOEStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="ServoJ6XML")
                    {
                        CommonData::setServoJ6XMLStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="Terminal")
                    {
                        CommonData::setTerminalFOEStatus(2);
                    }
                    if(m_strLabelSingleFwAndXML=="TerminalXML")
                    {
                        CommonData::setTerminalXMLStatus(2);
                    }
                }
                m_widgetSingleUpgrading->setSingleUpgradeStatus(getCRSingleStatus.result);
            }
            m_singleUpgradeTimer->stop();
            return;
        }
    });

    connect(ui->btnSingleIOFW,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getCCBOXFOEStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("CCBOX");
    });
    connect(ui->btnSingleIOFWXML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getCCBOXXMLStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("CCBOXXML");
    });
    connect(ui->btnSingleSafeIO,&QPushButton::clicked,this,[&]{
        if(!CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X))
        {
            if(setSingleFuncUpgradeStatus(CommonData::getSafeIOFOEStatus()) == 2)
            {
                return;
            }
        }
        else
        {
            if(CommonData::getSafeIOFOEStatus() == 2 && CommonData::getSafeBFOEStatus() == 2)
            {
                qDebug()<<" safeA --> "<<CommonData::getSafeIOFOEStatus()<<"  safeB--> "<<CommonData::getSafeBFOEStatus();
                setSingleFuncUpgradeStatus(2);
                return;
            }
        }
        showSingleUpgradeWidget("SafeIO");
    });
    connect(ui->btnSingleSafeIOXML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getSafeIOXMLStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("SafeIOXML");
    });
    connect(ui->btnSingleFeedback,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getFeedbackStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("FeedBack");
    });
    connect(ui->btnSingleUniIO,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUniIOFOEStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("UniIO");
    });
    connect(ui->btnSingleUniIOXML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUniIOXMLStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("UniIOXML");
    });
    connect(ui->btnSingleServoJ1,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ1FOEStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ1");
    });
    connect(ui->btnSingleServoJ1XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ1XMLStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ1XML");
    });
    connect(ui->btnSingleServoJ2,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ2FOEStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ2");
    });
    connect(ui->btnSingleServoJ2XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ2XMLStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ2XML");
    });
    connect(ui->btnSingleServoJ3,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ3FOEStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ3");
    });
    connect(ui->btnSingleServoJ3XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ3XMLStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ3XML");
    });
    connect(ui->btnSingleServoJ4,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ4FOEStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ4");
    });
    connect(ui->btnSingleServoJ4XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ4XMLStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ4XML");
    });
    connect(ui->btnSingleServoJ5,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ5FOEStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ5");
    });
    connect(ui->btnSingleServoJ5XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ5XMLStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ5XML");
    });
    connect(ui->btnSingleServoJ6,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ6FOEStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ6");
    });
    connect(ui->btnSingleServoJ6XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getServoJ6XMLStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("ServoJ6XML");
    });
    connect(ui->btnSingleTerminal,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getTerminalFOEStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("Terminal");
    });
    connect(ui->btnSingleTerminalXML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getTerminalXMLStatus()) == 2)
        {
            return;
        }
        showSingleUpgradeWidget("TerminalXML");
    });

}

void MainWidget2::initBackupToolWidget(bool reset)
{
    connect(ui->btnFileBackup,&QPushButton::clicked,this,[&]{
        FileBackupWidget* fileBackupWidget = new FileBackupWidget(this);
        fileBackupWidget->setGeometry(0,0,width(),height());
        fileBackupWidget->raise();
        fileBackupWidget->show();
        connect(fileBackupWidget,&FileBackupWidget::signalFileBackupStart,this,[&](QString lineSelectedBackupDir ,QString backupFileName){
            QPointer<Widget2ProgressDialog> ptFileBackupDlg = new Widget2ProgressDialog(this);
            ptFileBackupDlg->setGeometry(0,0,width(),height());
            ptFileBackupDlg->setTitle(tr("文件备份"));
            ptFileBackupDlg->setHeadLabel("backup");
            ptFileBackupDlg->setProgressRange(320);
            ptFileBackupDlg->show();

            QFile fileBat(QCoreApplication::applicationDirPath()+"/tool/fileBackup/dobotFileBackup.bat");
            if (fileBat.open(QFile::Truncate|QFile::WriteOnly)){
                QTextStream fos(&fileBat);
                fos.setCodec("UTF-8");
                fos<<"@echo off\r\n"
                     "chcp 65001\r\n"
                     "echo \"start backup\"\r\n"
                     "cd ./tool/FileBackup\r\n"
                     "cd /d %~dp0\r\n"
                     "rd /s /q backup\r\n"
                     "md backup\r\n"
                     "md backup\\project\r\n";
                fos<<"(\r\n"
                     "echo cd /dobot/userdata/project\r\n"
                     "echo rm -rf dobotfilebackupname.tar.gz\r\n"
                     "echo tar czvf dobotfilebackupname.tar.gz --exclude=logs *\r\n"
                     "echo exit\r\n";
                fos<<QString(") | plink.exe -ssh -l root -pw dobot %1\r\n").arg(g_strCurrentIP);
                fos<<QString("pscp.exe -pw dobot -r root@%1:/dobot/userdata/project/dobotfilebackupname.tar.gz ./backup/project\r\n").arg(g_strCurrentIP);
                fos<<"(\r\n"
                     "echo cd /dobot/userdata/project\r\n"
                     "echo rm -rf dobotfilebackupname.tar.gz\r\n"
                     "echo exit\r\n";
                fos<<QString(") | plink.exe -ssh -l root -pw dobot %1\r\n").arg(g_strCurrentIP);
                fos<<"cd ./backup/project\r\n"
                     "echo \"begin rename file...\"\r\n";
                fos<<QString("ren \"dobotfilebackupname.tar.gz\" \"%1.tar.gz\"\r\n").arg(backupFileName);
                fos<<"echo \"backup start successfully.\"\r\n";
                fileBat.close();
            }
            else
            {
                qDebug()<<fileBat.fileName();
                qDebug()<<"open/create dobotFileBackup.bat file error:"<<fileBat.errorString();
                ptFileBackupDlg->setHeadLabel("fail");
                return false;
            }

            QProcess* pFileBackup = new QProcess();
            static int fileBackupProgressValue = 1;
            connect(pFileBackup,&QProcess::readyRead,this,[&,pFileBackup,ptFileBackupDlg]{
                qDebug()<<"FileBackup-->"<<pFileBackup->readAll();
                ptFileBackupDlg->setProgressValue(fileBackupProgressValue++);
            });
            connect(pFileBackup,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
                qDebug()<<"exitCode "<<exitCode <<" exitStatus "<<exitStatus;
                //获取当前时间
                QDateTime current_time = QDateTime::currentDateTime();
                QString currentTime = current_time.toString("yyyyMMdd");

                //机器型号_控制柜型号_控制器版本_导出日期时间_Backup
                bool compress = m_zipDiyManager->zipDiyCompress(QCoreApplication::applicationDirPath()+"/tool/fileBackup/backup/project",
                                                                lineSelectedBackupDir+"/"+backupFileName+".zip",m_zipLogExportPassword);
                if(compress){
                    ptFileBackupDlg->setProgressValue(ptFileBackupDlg->progressRange());
                    ptFileBackupDlg->setHeadLabel("success");
                }else{
                    ptFileBackupDlg->setHeadLabel("fail");
                }
                fileBackupProgressValue = 0;
                pFileBackup->deleteLater();//new Code
            });
            connect(pFileBackup,&QProcess::errorOccurred,[&,ptFileBackupDlg](QProcess::ProcessError error){
                qWarning()<<"pFileBackup slot_ errorOccurred!" <<error;
                fileBackupProgressValue = 0;
                ptFileBackupDlg->setHeadLabel("fail");
                pFileBackup->deleteLater();
            });
            qDebug()<<" backupFileName ---> "<<backupFileName;
            pFileBackup->setProgram(fileBat.fileName());
            pFileBackup->start();
        },Qt::UniqueConnection);
    });

    connect(ui->btnFileRecovery,&QPushButton::clicked,this,[&]
    {
        FileRecoveryWidget* fileRecoveryWidget = new FileRecoveryWidget(this);
        connect(fileRecoveryWidget,&FileRecoveryWidget::signalFileRecoveryStart,this,[&](QString strSelectedBackupFileName){
            widget2FileRecoveryTip(strSelectedBackupFileName);
        });
        fileRecoveryWidget->setGeometry(0,0,width(),height());
        fileRecoveryWidget->raise();
        fileRecoveryWidget->show();
    });
}

void MainWidget2::widget2FileRecoveryTip(const QString& strSelectedBackupFileName)
{
    auto pFileRecoverTip = new Widget2FileRecoveryTip(this);
    pFileRecoverTip->setGeometry(0,0,width(),height());
    pFileRecoverTip->raise();
    pFileRecoverTip->show();
    connect(pFileRecoverTip,&Widget2FileRecoveryTip::signalsFileRecoveryStart,this,[=]{
        DobotType::ControllerType controllerType = m_httpProtocol->getControllerType(g_strCurrentIP);
        if(!strSelectedBackupFileName.contains(CommonData::getCurrentSettingsVersion().control)){
            qDebug()<<"pFileRecoverTip  控制柜版本不同";
            pFileRecoverTip->setWidgetStatus(false,tr("控制器版本"));
            return;
        }
        if(!strSelectedBackupFileName.contains(CommonData::getStrPropertiesCabinetType().name)){
            qDebug()<<"pFileRecoverTip  控制柜型号不同";
            pFileRecoverTip->setWidgetStatus(false,tr("控制柜型号"));
            return;
        }
        if(!strSelectedBackupFileName.contains(controllerType.name.simplified())){
            qDebug()<<"pFileRecoverTip  机器本体不同";
            pFileRecoverTip->setWidgetStatus(false,tr("机器本体"));
            return;
        }
        pFileRecoverTip->close();

        QDir dir(QCoreApplication::applicationDirPath()+"/tool/fileRecovery/project");
        if(dir.exists()){
            dir.removeRecursively();
        }
        qDebug()<< "dir.path() ==  "<<dir.path();

        static int widget2FileRecoveryTipProcessValue = 0;

        QPointer<Widget2ProgressDialog> ptFileRecoveryDlg = new Widget2ProgressDialog(this);
        ptFileRecoveryDlg->setGeometry(0,0,width(),height());
        ptFileRecoveryDlg->setHeadLabel("fileRecovery");
        ptFileRecoveryDlg->setTitle(tr("文件恢复"));
        ptFileRecoveryDlg->setWarnMessage(tr("提示：需重启控制柜完成文件恢复。"));
        ptFileRecoveryDlg->setProgressRange(260);
        ptFileRecoveryDlg->setProgressValue(++widget2FileRecoveryTipProcessValue);
        ptFileRecoveryDlg->show();

        //先删掉这个目录下所有的文件，防止残留导致上传一些无用的文件
        QDir dirDel(QApplication::applicationDirPath()+"/tool/fileRecovery/project");
        auto allFileInfo = dirDel.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
        for (auto info : allFileInfo){
            if (info.isFile()) {
                if (!QFile::remove(info.absoluteFilePath())) {
                    qDebug() << "Failed to remove file: " << info.absoluteFilePath();
                }
            } else if (info.isDir()) {
                if (!dirDel.rmdir(info.absoluteFilePath())) {
                    qDebug() << "Failed to remove directory: " << info.absoluteFilePath();
                }
            }
        }
        //解压
        if(m_zipDiyManager->zipUnCompress(strSelectedBackupFileName,QApplication::applicationDirPath()+"/tool/fileRecovery/",m_zipLogExportPassword)){
            ptFileRecoveryDlg->setProgressValue(++widget2FileRecoveryTipProcessValue);
        }else{
            qDebug()<<"ptFileRecoveryDlg zipUnCompress  failed";
            ptFileRecoveryDlg->setHeadLabel("fail");
            return;
        }
        QStringList tarFileNames = strSelectedBackupFileName.split("/");
        QString tarFileName = tarFileNames.at(tarFileNames.count()-1);
        tarFileName = tarFileName.mid(0,tarFileName.count()-4)+".tar.gz";
        qDebug()<<" tarFileName --> "<<tarFileName;
        allFileInfo = dirDel.entryInfoList(QDir::Files);
        //再找到targz的文件，重命名，防止乱七八糟的文件名导致上传后控制器无法解压。
        for (auto info : allFileInfo){
            QString str = info.absoluteFilePath();
            if (str.contains(CommonData::getCurrentSettingsVersion().control) &&
                    str.contains(CommonData::getStrPropertiesCabinetType().name) &&
                    str.contains(controllerType.name.simplified())){
                QString strNew = QApplication::applicationDirPath()+"/tool/fileRecovery/project/dobotmaintenanceproject.tar.gz";
                if (QFile::rename(str,strNew)){
                    tarFileName = "dobotmaintenanceproject.tar.gz";
                    qDebug()<<" tarFileName new name--> "<<tarFileName;
                    break;
                }
            }
        }

        QFile fileBat(QCoreApplication::applicationDirPath()+"/tool/fileRecovery/dobotFileRecovery.bat");
        if (fileBat.open(QFile::Truncate|QFile::WriteOnly)){
            QTextStream fos(&fileBat);
            fos.setCodec("UTF-8");
            fos<<"@echo off\r\n"
                 "chcp 65001\r\n"
                 "echo \"start fileRecovery\"\r\n"
                 "cd ./tool/fileRecovery\r\n"
                 "cd /d %~dp0\r\n";
            fos<<QString("pscp.exe -pw dobot -r ./project/* root@%1:/dobot/userdata/project/\r\n").arg(g_strCurrentIP);
            fos<<"(\r\n"
                 "echo cd /dobot/userdata/project\r\n";
            fos<<QString("echo tar -xvf \"%1\"\r\n").arg(tarFileName);
            fos<<QString("echo rm -rf \"%1\"\r\n").arg(tarFileName);
            fos<<"echo exit\r\n";
            fos<<QString(") | plink.exe -ssh -l root -pw dobot %1\r\n").arg(g_strCurrentIP);
            fos<<"echo \"fileRecovery start successfully.\"\r\n";
            fileBat.close();
        }
        else
        {
            qDebug()<<fileBat.fileName();
            qDebug()<<"open/create dobotFileRecovery.bat file error:"<<fileBat.errorString();
            ptFileRecoveryDlg->setHeadLabel("fail");
            return;
        }

        QProcess *pFileRecovery = new QProcess();
        connect(pFileRecovery,&QProcess::readyRead,this,[&,pFileRecovery,ptFileRecoveryDlg]{
            qDebug()<<"FileRecovery-->"<<pFileRecovery->readAll();
            ptFileRecoveryDlg->setProgressValue(widget2FileRecoveryTipProcessValue++);
        });
        connect(pFileRecovery,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
            qDebug()<<"exitCode "<<exitCode <<" exitStatus "<<exitStatus;
            //获取当前时间
            QDateTime current_time = QDateTime::currentDateTime();
            QString currentTime = current_time.toString("yyyyMMdd");
            if(exitStatus == QProcess::NormalExit){
                ptFileRecoveryDlg->setProgressValue(ptFileRecoveryDlg->progressRange());
                ptFileRecoveryDlg->setHeadLabel("success");
            }else{
                ptFileRecoveryDlg->setHeadLabel("fail");
            }
            widget2FileRecoveryTipProcessValue = 0;
            pFileRecovery->deleteLater();//new code
        });
        connect(pFileRecovery,&QProcess::errorOccurred,[&,ptFileRecoveryDlg](QProcess::ProcessError error){
            qWarning()<<"pFileRecovery slot_ errorOccurred!" <<error;
            widget2FileRecoveryTipProcessValue = 0;
            ptFileRecoveryDlg->setHeadLabel("fail");
            pFileRecovery->deleteLater();
        });
        pFileRecovery->setProgram(fileBat.fileName());
        pFileRecovery->start();
    });
}

void MainWidget2::initServoParamsWidget()
{
    ui->comboServo->setView(new QListView());
    connect(ui->btnServoParamPasswordConfirm,&QPushButton::clicked,this,[&]{
        int correctPassword = m_currentPasswordRandom|4353;
        bool ok;
        int password = ui->lineServoParamPassword->text().toInt(&ok);
        if(!ok)
        {
            auto pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("密码错误，请重新输入"));
            pWndMsg->show();
        }
        if(password==correctPassword)
        {
            m_isPasswordConfirm = true;
            ui->lineServoParamPassword->clear();
            showServoParamWidget();
        }
        else
        {
            auto pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("密码错误，请重新输入"));
            pWndMsg->show();
        }
    });

    connect(ui->btnReadPID,&QPushButton::clicked,this,[&]{
        QString servoJ = ui->comboServo->currentText();
        DobotType::StructSettingsServoParams settingsServoParams;

        auto pServoParamsWait = new Widget2ServoParamsProgress(this);
        pServoParamsWait->show();
        pServoParamsWait->loading();
        int readCount = 6;
        QVector<int> vcPidIndex;
        for(int i = 0; i < mServoParamsTableModel->rowCount();i++)
        {
            QStandardItem* item = mServoParamsTableModel->item(i,colParaIndex);
            if(item == nullptr || item->text().isEmpty())
            {
                qDebug()<<"exportServoParamsFile item->text().isEmpty() ";
                continue;
            }
            QString paramIndex = item->text();
            qDebug()<<"exportServoParamsFile paramIndex  "<<paramIndex;
            QString address = "addr"+paramIndex2Address(paramIndex);
            qDebug()<<"exportServoParamsFile  address  "<<address;
            int intParamIndex = paramIndex2Address(paramIndex).toInt();
            qDebug()<<" intParamIndex "<<intParamIndex;
            //获取 0800~0805功能码的数据 200801~200805
            if(intParamIndex>200800&&intParamIndex<=200806)
            {
                DobotType::StructServoParam servoParam;
                QModelIndex dotDividedBitsIndex = mServoParamsTableModel->index(i, colDotDividedBits, QModelIndex());
                int dotDividedBits = mServoParamsTableModel->data(dotDividedBitsIndex).toInt();
                QModelIndex index = mServoParamsTableModel->index(i, colValue, QModelIndex());
                double value = mServoParamsTableModel->data(index).toDouble();
                double writeValue = dotDividedBitsValue(false,dotDividedBits,value);

                servoParam.servoNum = servoJ;
                servoParam.key = address;
                servoParam.value = writeValue;
                settingsServoParams.servoParams.append(servoParam);

                vcPidIndex.append(i);
            }
        }

        bool isSuccess = m_httpProtocol->postSettingsReadServoParams(g_strCurrentIP,settingsServoParams);
        if(!isSuccess)
        {
            pServoParamsWait->setMessage(tr("读取失败!"),false);
            qDebug()<<"btnReadAllServoParams postSettingsReadServoParams failed ";
        }
        else
        {
            int sleepTime = readCount*1000;
            qDebug()<<" sleepTime "<<sleepTime;
            sleep(sleepTime);
            DobotType::StructSettingsServoParamsResult servoParamsResult = m_httpProtocol->getSettingsReadServoParams(g_strCurrentIP);
            if(servoParamsResult.status == true)
            {
                QStringList errorParams;
                QHash<QString,DobotType::StructServoParam> servoParamHash;
                for(DobotType::StructServoParam servoParam:servoParamsResult.servoParams)
                {
                    servoParamHash.insert(servoParam.key,servoParam);
                }
                for(int i = 0; i < mServoParamsTableModel->rowCount();i++)
                {
                    QStandardItem* item = mServoParamsTableModel->item(i,colParaIndex);
                    if(item == nullptr || item->text().isEmpty())
                    {
                        qDebug()<<"m_singleReadDelegate item->text().isEmpty() ";
                        continue;
                    }
                    QString paramIndex = item->text();
                    QString address = "addr"+paramIndex2Address(paramIndex);
                    qDebug()<<"m_singleReadDelegate  address.toLower()  "<<address.toLower();
                    if(!servoParamHash.contains(address.toLower()))
                    {
                        qDebug()<<"!contain --->  "<<paramIndex;
                        continue;
                    }
                    DobotType::StructServoParam servoParam = servoParamHash.value(address.toLower());
                    if(servoParam.status != true)
                    {
                        errorParams.append(paramIndex);
                        qDebug()<<"servoParam.status false  "<<servoParam.status;
                        continue;
                    }
                    QModelIndex dotDividedBitsIndex = mServoParamsTableModel->index(i, colDotDividedBits, QModelIndex());
                    int dotDividedBits = mServoParamsTableModel->data(dotDividedBitsIndex).toInt();
                    double viewValue = dotDividedBitsValue(true,dotDividedBits,servoParam.value);

                    QModelIndex index = mServoParamsTableModel->index(i, colValue, QModelIndex());
                    mServoParamsTableModel->setData(index,viewValue, Qt::EditRole);

                }
                if(errorParams.isEmpty())
                {
                    //如果没有返回任何数据,也是读取失败
                    if(servoParamsResult.servoParams.isEmpty())
                    {
                        pServoParamsWait->setMessage(tr("读取失败!"),false);
                    }
                    else
                    {
                        pServoParamsWait->setMessage(tr("读取成功!"),true);
                    }
                }
                else
                {
                    pServoParamsWait->setMessage(tr("读取失败!"),false,errorParams);
                }
                servoParamHash.clear();
            }
            else
            {
                pServoParamsWait->setMessage(tr("读取失败!"),false);
            }
        }
        if (vcPidIndex.size()>0){
            int iPidIndex = vcPidIndex[vcPidIndex.size()/2];
            QModelIndex index = mServoParamsTableModel->index(iPidIndex, 0);
            ui->tableServoParamView->scrollTo(index, QAbstractItemView::PositionAtCenter);
        }
    });

    connect(ui->btnExportExcel,&QPushButton::clicked,this,[&]{
        QString servoJ = ui->comboServo->currentText();
        QTime qTime = QTime::currentTime();
        QString time = qTime.toString("hh-mm-ss");
        QString csvName = CommonData::getControllerType().name+"_"
                                                        +CommonData::getStrPropertiesCabinetType().name+"_"
                                                        +CommonData::getCurrentSettingsVersion().control+"_"
                                                        +time+"_"+servoJ+".csv";
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                        QCoreApplication::applicationDirPath()+"/"
                                                        +csvName,
                                                        tr("CSV (*.csv)"));
        if(fileName.isEmpty())
        {
            return;
        }

        qDebug()<<"fileName  ---> "<<fileName;
        QFile file(fileName);
        if(file.exists())
        {
            file.remove();
        }

        QString strServoJData;
        strServoJData = "Version = 100121,,,,\nParaIndStr,ParaMapIndStr,ParaName,ParaValue,ReadOnly\n";
        for(int row = 0; row < mServoParamsTableModel->rowCount();row++)
        {
            QString strParaIndex = mServoParamsTableModel->data(mServoParamsTableModel->index(row,colParaIndex, QModelIndex())).toString();
            QString strParaMapIndex = mServoParamsTableModel->data(mServoParamsTableModel->index(row,colParaMapIndex, QModelIndex())).toString();
            QString strParaName = mServoParamsTableModel->data(mServoParamsTableModel->index(row,colParaName, QModelIndex())).toString();
            QString strParaValue = mServoParamsTableModel->data(mServoParamsTableModel->index(row,colValue, QModelIndex())).toString();
            QString strReadOnly = mServoParamsTableModel->data(mServoParamsTableModel->index(row,colIsReadOnly, QModelIndex())).toString();
            strServoJData += strParaIndex +","+strParaMapIndex+","+strParaName+","+strParaValue+","+strReadOnly+"\n";
        }
        //写文件
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        out << strServoJData;
        file.close();
        auto pWndMsg = new MessageWidget(this);
        pWndMsg->setMessage("",tr("导出关节表格成功"));
        pWndMsg->show();
    });

    connect(ui->btnImportExcel,&QPushButton::clicked,this,[&]{
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                        "",
                                                        tr("Excel (*.xlsx *.csv)"));

        importServoParamsExcel(fileName);

    });

    //MVD模式管理表格
    mServoParamsTableModel = new TableModel();

    ui->tableServoParamView->setShowGrid(false);
    ui->tableServoParamView->setModel(mServoParamsTableModel);
    ui->tableServoParamView->setMouseTracking(true);
    ui->tableServoParamView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableServoParamView->setContextMenuPolicy(Qt::CustomContextMenu);

    delegateServoParamsTable();
    mServoParamsTableModel->setRowCount(modelRowMax);
    mServoParamsTableModel->setColumnCount(modelColMax);

    //先导入模板文件，在声明一下的信号槽函数，放置初始化时就高亮
    connect(mServoParamsTableModel,&QStandardItemModel::itemChanged,this,[&](QStandardItem *item){
        if(item->column() == colValue)
        {
            if(ui->btnConnect->property("isConnected").toInt()==2)
            {
                item->setForeground(QBrush(QColor("#3EB72A")));
            }
        }
    });
    connect(ui->btnReadAllServoParams,&QPushButton::clicked,this,[&]{
        QStandardItem* item = mServoParamsTableModel->item(0,colParaIndex);
        if(item == nullptr || item->text().isEmpty())
        {
            auto pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("未导入Excel文件"));
            pWndMsg->show();
            return;
        }

        QString servoJ = ui->comboServo->currentText();
        QList<DobotType::StructServoParam> allSelectedServoParams;
        int readCount = 0;
        for(int i = 0; i < mServoParamsTableModel->rowCount();i++)
        {
            QStandardItem* isCheckItem = mServoParamsTableModel->item(i,colIsCheck);
            if(isCheckItem == nullptr )
            {
                qDebug()<<"m_singleReadDelegate item nullptr ";
                continue;
            }
            if(isCheckItem->data(Qt::UserRole).toBool())
            {
                readCount++;
            }
            else
            {
                continue;
            }

            QModelIndex dotDividedBitsIndex = mServoParamsTableModel->index(i, colDotDividedBits, QModelIndex());
            int dotDividedBits = mServoParamsTableModel->data(dotDividedBitsIndex).toInt();
            QModelIndex index = mServoParamsTableModel->index(i, colValue, QModelIndex());
            double value = mServoParamsTableModel->data(index).toDouble();
            double writeValue = dotDividedBitsValue(false,dotDividedBits,value);
            QStandardItem* item = mServoParamsTableModel->item(i,colParaIndex);
            if(item == nullptr || item->text().isEmpty())
            {
                qDebug()<<"exportServoParamsFile item->text().isEmpty() ";
                continue;
            }
            QString paramIndex = item->text();
            qDebug()<<"exportServoParamsFile paramIndex  "<<paramIndex;
            QString address = "addr"+paramIndex2Address(paramIndex);
            qDebug()<<"exportServoParamsFile  address  "<<address;

            DobotType::StructServoParam servoParam;
            servoParam.servoNum = servoJ;
            servoParam.key = address;
            servoParam.value = writeValue;
            allSelectedServoParams.append(servoParam);
        }

        qDebug()<<"readCount +++++"<<readCount;
        if(readCount == 0)
        {
            auto pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("没有勾选任何选项！"));
            pWndMsg->show();
            return;
        }

        auto pServoParamsWait = new Widget2ServoParamsProgress(this);
        pServoParamsWait->show();
        pServoParamsWait->loading();
        const int iSingleWaitTime = CLuaApi::getInstance()->getServoParamReadTimeoutMs().value();
        while (allSelectedServoParams.size()>0)
        {
            readCount = 0;
            DobotType::StructSettingsServoParams settingsServoParams;
            for (int i=0; i<allSelectedServoParams.size() && i<20; ++i){//接口每次最大只能支持20条，所以分批次读
                settingsServoParams.servoParams.append(allSelectedServoParams.takeFirst());
            }
            readCount = settingsServoParams.servoParams.size();

            bool isSuccess = m_httpProtocol->postSettingsReadServoParams(g_strCurrentIP,settingsServoParams);
            if(!isSuccess)
            {
                qDebug()<<"btnReadAllServoParams postSettingsReadServoParams failed ";
                pServoParamsWait->setMessage(tr("读取伺服参数失败!"),false);
                return;
            }
            int sleepTime = readCount*iSingleWaitTime;
            qDebug()<<" sleepTime "<<sleepTime;
            sleep(sleepTime);
            DobotType::StructSettingsServoParamsResult servoParamsResult = m_httpProtocol->getSettingsReadServoParams(g_strCurrentIP);
            if(!servoParamsResult.status)
            {
                qDebug()<<"btnReadAllServoParams getSettingsReadServoParams failed ";
                pServoParamsWait->setMessage(tr("获取伺服参数读取结果失败!"),false);
                return;
            }
            QStringList errorParams;
            QHash<QString,DobotType::StructServoParam> servoParamHash;
            for(DobotType::StructServoParam servoParam:servoParamsResult.servoParams)
            {
                servoParamHash.insert(servoParam.key,servoParam);
            }
            for(int i = 0; i < mServoParamsTableModel->rowCount();i++)
            {
                QStandardItem* item = mServoParamsTableModel->item(i,colParaIndex);
                if(item == nullptr || item->text().isEmpty())
                {
                    qDebug()<<"m_singleReadDelegate item->text().isEmpty() ";
                    continue;
                }
                QString paramIndex = item->text();
                QString address = "addr"+paramIndex2Address(paramIndex);
                if(!servoParamHash.contains(address.toLower())){
                    continue;
                }
                DobotType::StructServoParam servoParam = servoParamHash.value(address.toLower());
                if(servoParam.status != true)
                {
                    errorParams.append(paramIndex);
                    qDebug()<<"servoParam.status false  "<<servoParam.status;
                    continue;
                }
                QModelIndex dotDividedBitsIndex = mServoParamsTableModel->index(i, colDotDividedBits, QModelIndex());
                int dotDividedBits = mServoParamsTableModel->data(dotDividedBitsIndex).toInt();
                double viewValue = dotDividedBitsValue(true,dotDividedBits,servoParam.value);
                QModelIndex index = mServoParamsTableModel->index(i, colValue, QModelIndex());
                mServoParamsTableModel->setData(index,viewValue, Qt::EditRole);
            }
            if(errorParams.isEmpty())
            {
                //如果没有返回任何数据,也是读取失败
                if(servoParamsResult.servoParams.isEmpty())
                {
                    qDebug()<<"btnReadAllServoParams getSettingsReadServoParams failed,没有读取到任何伺服参数 ";
                    pServoParamsWait->setMessage(tr("读取失败!"),false);
                    return;
                }
            }
            else
            {
                pServoParamsWait->setMessage(tr("读取失败!"),false,errorParams);
                return;
            }
        }
        pServoParamsWait->setMessage(tr("读取成功!"),true);
    });

    connect(ui->btnWriteAllServoParams,&QPushButton::clicked,this,[&]{
        QString servoJ = ui->comboServo->currentText();
        int writeCount = 0;
        QList<DobotType::StructServoParam> allSelectedServoParams;
        for(int i = 0; i < mServoParamsTableModel->rowCount();i++)
        {
            QStandardItem* isCheckItem = mServoParamsTableModel->item(i,colIsCheck);
            if(isCheckItem == nullptr){
                continue;
            }
            QStandardItem* isReadOnlyItem = mServoParamsTableModel->item(i,colIsReadOnly);
            if(isReadOnlyItem == nullptr || isReadOnlyItem->text().isEmpty()||isReadOnlyItem->text() == "TRUE")
            {
                continue;
            }
            if(isCheckItem->data(Qt::UserRole).toBool()){
                writeCount++;
            }
            else{
                continue;
            }

            DobotType::StructServoParam servoParam;
            QModelIndex dotDividedBitsIndex = mServoParamsTableModel->index(i, colDotDividedBits, QModelIndex());
            int dotDividedBits = mServoParamsTableModel->data(dotDividedBitsIndex).toInt();
            QModelIndex index = mServoParamsTableModel->index(i, colValue, QModelIndex());
            double value = mServoParamsTableModel->data(index).toDouble();
            double writeValue = dotDividedBitsValue(false,dotDividedBits,value);
            QStandardItem* item = mServoParamsTableModel->item(i,colParaIndex);
            if(item == nullptr || item->text().isEmpty())
            {
                qDebug()<<"exportServoParamsFile item->text().isEmpty() ";
                continue;
            }
            QString paramIndex = item->text();
            QString address = "addr"+paramIndex2Address(paramIndex);
            servoParam.servoNum = servoJ;
            servoParam.key = address;
            servoParam.value = writeValue;
            allSelectedServoParams.append(servoParam);
        }

        qDebug()<<"writeCount +++++"<<writeCount;
        if(writeCount == 0)
        {
            auto pWndMsg = new MessageWidget(this);
            pWndMsg->setMessage("warn",tr("没有勾选任何选项！"));
            pWndMsg->show();
            return;
        }

        auto pServoParamsWait = new Widget2ServoParamsProgress(this);
        pServoParamsWait->show();
        pServoParamsWait->loading();
        const int iSingleWaitTime = CLuaApi::getInstance()->getServoParamWriteTimeoutMs().value();
        while (allSelectedServoParams.size()>0)
        {
            writeCount = 0;
            DobotType::StructSettingsServoParams settingsServoParams;
            settingsServoParams.src = "httpClient";
            for (int i=0; i<allSelectedServoParams.size() && i<20; ++i){//接口每次最大只能支持20条，所以分批次
                settingsServoParams.servoParams.append(allSelectedServoParams.takeFirst());
            }
            writeCount = settingsServoParams.servoParams.size();

            bool isSuccess = m_httpProtocol->postSettingsModifyServoParams(g_strCurrentIP,settingsServoParams);
            if(!isSuccess)
            {
                qDebug()<<"btnWriteAllServoParams postSettingsModifyServoParams failed ";
                pServoParamsWait->setMessage(tr("写入伺服参数失败"),false);
                return;
            }
            int sleepTime = writeCount*iSingleWaitTime;
            qDebug()<<" sleepTime "<<sleepTime;
            sleep(sleepTime);
            DobotType::StructSettingsServoParamsResult result = m_httpProtocol->getSettingsModifyServoParams(g_strCurrentIP);
            if(!result.status){
                pServoParamsWait->setMessage(tr("获取伺服参数写入结果失败"),false);
                return;
            }
            QStringList errorParamAddList;
            for(DobotType::StructServoParam structServoParam : result.servoParams)
            {
                if(structServoParam.status != true)
                {
                    errorParamAddList.append(address2ParamIndex(structServoParam.key));
                }
            }
            if(!errorParamAddList.isEmpty()){
                pServoParamsWait->setMessage(tr("写入失败"),false,errorParamAddList);
                return;
            }
        }
        pServoParamsWait->setMessage(tr("写入成功"),true);
    });
}

void MainWidget2::upgradeAllFirmwareAndXMLs()
{
    if(m_isInterruptUpgrade){
        qDebug()<<"upgradeAllFirmwareAndXMLs  m_isInterruptUpgrade true";
        return;
    }
    DobotType::UpdateFirmware updateFirmware;
    auto cabTypeLua = CLuaApi::getInstance()->getCabType();
    if (cabTypeLua.hasValue())
    {
        updateFirmware.cabType = cabTypeLua.value();
    }
    else{
        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)
                ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)){
            updateFirmware.cabType = "big";
        }else{
            updateFirmware.cabType = "small";
        }
    }
    updateFirmware.operation = "start";

    auto cabVersionLua = CLuaApi::getInstance()->getCabVersion();
    if (cabVersionLua.hasValue())
    {
        updateFirmware.cabVersion = cabVersionLua.value();
    }
    else{
        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)
                ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
        {
            updateFirmware.cabVersion = "V1";
        }
        else
        {
            updateFirmware.cabVersion = "V2";
        }
    }
    updateFirmware.currentVersion.unio = CommonData::getCurrentSettingsVersion().unio;
    updateFirmware.currentVersion.feedback = CommonData::getCurrentSettingsVersion().feedback;
    updateFirmware.currentVersion.safeio = CommonData::getCurrentSettingsVersion().safeio;
    updateFirmware.currentVersion.servo1 = CommonData::getCurrentSettingsVersion().servo1;
    updateFirmware.currentVersion.servo2 = CommonData::getCurrentSettingsVersion().servo2;
    updateFirmware.currentVersion.servo3 = CommonData::getCurrentSettingsVersion().servo3;
    updateFirmware.currentVersion.servo4 = CommonData::getCurrentSettingsVersion().servo4;
    updateFirmware.currentVersion.servo5 = CommonData::getCurrentSettingsVersion().servo5;
    updateFirmware.currentVersion.servo6 = CommonData::getCurrentSettingsVersion().servo6;
    updateFirmware.currentVersion.terminal = CommonData::getCurrentSettingsVersion().terminal;
    updateFirmware.currentVersion.control = CommonData::getCurrentSettingsVersion().control;
    QString upgradeFirmwareResult;
    if(CommonData::getCurrentSettingsVersion().control.contains(CommonData::getUpgradeSettingsVersion().control)){
        qDebug()<<"版本一样 跳过升级";
        updateFirmware.updateControlVersion = "";
    }
    else
    {
        QString updateControlVersion = CommonData::getUpgradeSettingsVersion().control.split("-")[0];
        updateFirmware.updateControlVersion = updateControlVersion;
    }
    if(!m_isInterruptUpgrade){
        g_isRestartServer = true;
        m_upgradeProgressTimer->stop();
        upgradeFirmwareResult = m_httpProtocol->postUpdateCRFwAndXml(g_strCurrentIP,updateFirmware);
        m_upgradeProgressTimer->start(1000);
    }
}

void MainWidget2::readUpgradeVersion(QString parentVersion,QString version)
{
    DobotType::SettingsVersion settingVersion;
    QString upgradeInfoPath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+parentVersion+"/"+version+"/"+version+"upgrade.ini";
    qDebug()<<"upgradeInfoPath  --- "<<upgradeInfoPath;
    QString safeIO;
    QString terminalIO;
    QString uniIO;
    QString servoV3;
    QString servoV4;
    QString CCBOXText;
    QString feedbackText;
    QString mainControl;
    QString upgradeSettingServo;

    QHash<QString,QHash<QString,QString>> allLuaVal;
    QSettings iniFiles(upgradeInfoPath,QSettings::IniFormat);
    QStringList allChilds = iniFiles.childGroups();
    for (int i=0; i<allChilds.size(); ++i){
        QString strGN = allChilds[i];
        QHash<QString,QString> allKV;
        iniFiles.beginGroup(strGN);
        QStringList allKeys = iniFiles.allKeys();
        for (int j=0; j<allKeys.size(); ++j){
            QString strKey = allKeys[j];
            QString strVal = iniFiles.value(strKey).toString();
            allKV.insert(strKey,strVal);
        }
        iniFiles.endGroup();
        allLuaVal.insert(strGN, allKV);
    }
    auto luaValueIni = CLuaApi::getInstance()->getUpgradeIniVersion(allLuaVal);
    if (luaValueIni.hasValue()){
        QHash<QString,QString> kvm = luaValueIni.value();
        mainControl = kvm.value("control");
        safeIO = kvm.value("safeio");
        terminalIO = kvm.value("terminal");
        uniIO = kvm.value("unio");
        CCBOXText = kvm.value("ccboxio");
        feedbackText = kvm.value("feedback");
        upgradeSettingServo = kvm.value("servo");
    }else{
        QSettings upgradeInfoSetting(upgradeInfoPath,QSettings::IniFormat);
        qDebug()<< upgradeInfoSetting.childGroups();
        upgradeInfoSetting.beginGroup("version");
        safeIO = upgradeInfoSetting.value("safeIO").toString();
        terminalIO = upgradeInfoSetting.value("terminalIO").toString();
        uniIO = upgradeInfoSetting.value("uniIO").toString();
        servoV3 = upgradeInfoSetting.value("servoV3").toString();
        servoV4 = upgradeInfoSetting.value("servoV4").toString();
        CCBOXText = upgradeInfoSetting.value("CCBOX").toString();
        feedbackText = upgradeInfoSetting.value("feedback").toString();
        mainControl = upgradeInfoSetting.value("mainControl").toString();
        qDebug()<<" CommonData::getSettingsVersion().servo1 "<<CommonData::getCurrentSettingsVersion().servo1;
        if (CommonData::getCurrentSettingsVersion().servo1.size()>0){
            if(CommonData::getCurrentSettingsVersion().servo1.at(0)=='3'){
                upgradeSettingServo = servoV3;
            }else if(CommonData::getCurrentSettingsVersion().servo1.at(0)=='4'){
                upgradeSettingServo = servoV4;
            }
        }
        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX)) {
           if (CommonData::getControllerType().name.contains("Nova")) {
                upgradeSettingServo = upgradeInfoSetting.value("NCServo").toString();
                terminalIO = upgradeInfoSetting.value("NCTerminal").toString();
            }else if(CommonData::getControllerType().name.contains("CR")) {
                upgradeSettingServo = upgradeInfoSetting.value("RXServo").toString();
                terminalIO = upgradeInfoSetting.value("RXTerminal").toString();
            }
        }
        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)){
            upgradeSettingServo = upgradeInfoSetting.value("RXServo").toString();
            terminalIO = upgradeInfoSetting.value("RXTerminal").toString();
        }
        if(CommonData::getControllerType().name.contains("CR20A")){
            if(upgradeInfoSetting.value("CR20Terminal").toString().isEmpty()){
                terminalIO = upgradeInfoSetting.value("RXTerminal").toString();
            }else{
                terminalIO = upgradeInfoSetting.value("CR20Terminal").toString();
            }
        }
        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6) &&
                CommonData::getCurrentSettingsVersion().servo1.size()>0){
            if(CommonData::getCurrentSettingsVersion().servo1.at(0)=='3'){
                upgradeSettingServo = upgradeInfoSetting.value("ED6ServoV3").toString();
            }else if(CommonData::getCurrentSettingsVersion().servo1.at(0)=='2'){
                upgradeSettingServo = upgradeInfoSetting.value("ED6ServoV2").toString();
            }
            terminalIO = upgradeInfoSetting.value("ED6Terminal").toString();
        }
        upgradeInfoSetting.endGroup();
    }

    if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX)){
        settingVersion.unio = CCBOXText;
    }else{
        settingVersion.unio = uniIO;
    }
    qDebug()<<"upgradeSettingServo  "<<upgradeSettingServo;
    settingVersion.servo1 = upgradeSettingServo;
    settingVersion.servo2 = upgradeSettingServo;
    settingVersion.servo3 = upgradeSettingServo;
    settingVersion.servo4 = upgradeSettingServo;
    settingVersion.servo5 = upgradeSettingServo;
    settingVersion.servo6 = upgradeSettingServo;

    settingVersion.control = mainControl;
    settingVersion.safeio = safeIO;
    settingVersion.terminal = terminalIO;
    CommonData::setUpgradeSettingsVersion(settingVersion);

    ui->btnUpgrade->setDisabled(false);
    if(CommonData::getCurrentSettingsVersion().servo1=="0."||CommonData::getCurrentSettingsVersion().terminal=="0."){
        ui->btnUpgrade->setDisabled(true);
    }
    if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)
            ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
    {
        if(CommonData::getCurrentSettingsVersion().servo1=="0.0.0.0"
                ||CommonData::getCurrentSettingsVersion().terminal=="0.0.0.0"
                ||CommonData::getCurrentSettingsVersion().unio=="0.0.0.0"){
            ui->btnUpgrade->setDisabled(true);
        }
    }
    if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X))
    {
        if(CommonData::getCurrentSettingsVersion().safeio=="0.0.0.0"
                ||CommonData::getCurrentSettingsVersion().feedback=="0.0.0.0"){
            ui->btnUpgrade->setDisabled(true);
        }
    }
    if(mainControl.isEmpty()&&m_upgradeParentVersion.contains("temporary"))
    {
        ui->btnUpgrade->setDisabled(true);
    }
    ui->labelUpgradeMainControlVersion->setText(m_upgradeVersion);
    if(!mainControl.isEmpty()){
        ui->labelUpgradeMainControlVersion->setText(mainControl);
    }else{
        ui->labelUpgradeMainControlVersion->setText("-");
    }
    if(!CCBOXText.isEmpty()){
        ui->labelUpgradeIOFirmwareVersion->setText(CCBOXText);
    }else{
        ui->labelUpgradeIOFirmwareVersion->setText("-");
    }
    if(!feedbackText.isEmpty()){
        ui->labelUpgradeFeedbackVersion->setText(feedbackText);
    }else{
        ui->labelUpgradeFeedbackVersion->setText("-");
    }
    if(!safeIO.isEmpty()){
        ui->labelUpgradeSafeIOVersion->setText(safeIO);
    }else{
        ui->labelUpgradeSafeIOVersion->setText("-");
    }
    if(!uniIO.isEmpty()){
        ui->labelUpgradeUnIOVersion->setText(uniIO);
    }else{
        ui->labelUpgradeUnIOVersion->setText("-");
    }
    if(!terminalIO.isEmpty()){
        ui->labelUpgradeTerminalVersion->setText(terminalIO);
    }else{
        ui->labelUpgradeTerminalVersion->setText("-");
    }
    if(!upgradeSettingServo.isEmpty()){
        ui->labelUpgradeJ1Version->setText(upgradeSettingServo);
        ui->labelUpgradeJ2Version->setText(upgradeSettingServo);
        ui->labelUpgradeJ3Version->setText(upgradeSettingServo);
        ui->labelUpgradeJ4Version->setText(upgradeSettingServo);
        ui->labelUpgradeJ5Version->setText(upgradeSettingServo);
        ui->labelUpgradeJ6Version->setText(upgradeSettingServo);
    }else{
        ui->labelUpgradeJ1Version->setText("-");
        ui->labelUpgradeJ2Version->setText("-");
        ui->labelUpgradeJ3Version->setText("-");
        ui->labelUpgradeJ4Version->setText("-");
        ui->labelUpgradeJ5Version->setText("-");
        ui->labelUpgradeJ6Version->setText("-");
    }
}

void MainWidget2::readUpgradeVersionOld(QString parentVersion,QString version)
{
    DobotType::SettingsVersion settingVersion;
    QString upgradeInfoPath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+parentVersion+"/"+version+"/"+version+"upgrade.ini";
    qDebug()<<"upgradeInfoPath  --- "<<upgradeInfoPath;
    QSettings upgradeInfoSetting(upgradeInfoPath,QSettings::IniFormat);
    qDebug()<< upgradeInfoSetting.childGroups();
    upgradeInfoSetting.beginReadArray("version");
    QString safeIO = upgradeInfoSetting.value("safeIO").toString();
    QString terminalIO = upgradeInfoSetting.value("terminalIO").toString();
    QString uniIO = upgradeInfoSetting.value("uniIO").toString();
    QString servoV3 = upgradeInfoSetting.value("servoV3").toString();
    QString servoV4 = upgradeInfoSetting.value("servoV4").toString();
    QString CCBOXText = upgradeInfoSetting.value("CCBOX").toString();
    QString feedbackText = upgradeInfoSetting.value("feedback").toString();
    QString mainControl = upgradeInfoSetting.value("mainControl").toString();

    settingVersion.control = mainControl;
    settingVersion.safeio = safeIO;

    ui->btnUpgrade->setDisabled(false);
    if(CommonData::getCurrentSettingsVersion().servo1=="0."||CommonData::getCurrentSettingsVersion().terminal=="0."){
        ui->btnUpgrade->setDisabled(true);
    }
    if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)
            ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
    {
        if(CommonData::getCurrentSettingsVersion().servo1=="0.0.0.0"
                ||CommonData::getCurrentSettingsVersion().terminal=="0.0.0.0"
                ||CommonData::getCurrentSettingsVersion().unio=="0.0.0.0"){
            ui->btnUpgrade->setDisabled(true);
        }
    }
    if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X))
    {
        if(CommonData::getCurrentSettingsVersion().safeio=="0.0.0.0"
                ||CommonData::getCurrentSettingsVersion().feedback=="0.0.0.0"){
            ui->btnUpgrade->setDisabled(true);
        }
    }
    if(mainControl.isEmpty()&&m_upgradeParentVersion.contains("temporary"))
    {
        ui->btnUpgrade->setDisabled(true);
    }

    if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX)){
        CommonData::getUpgradeSettingsVersion().unio = CCBOXText;
    }else{
        CommonData::getUpgradeSettingsVersion().unio = uniIO;
    }
    QString upgradeSettingServo;
    qDebug()<<" CommonData::getSettingsVersion().servo1 "<<CommonData::getCurrentSettingsVersion().servo1;
    if (CommonData::getCurrentSettingsVersion().servo1.size()>0){
        if(CommonData::getCurrentSettingsVersion().servo1.at(0)=='3'){
            upgradeSettingServo = servoV3;
        }else if(CommonData::getCurrentSettingsVersion().servo1.at(0)=='4'){
            upgradeSettingServo = servoV4;
        }
    }

    if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX)
            /*&& CommonData::getControllerType().name.contains("Nova")*/){
        upgradeSettingServo = upgradeInfoSetting.value("NCServo").toString();
        terminalIO = upgradeInfoSetting.value("NCTerminal").toString();
    }

    if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X))
    {
        upgradeSettingServo = upgradeInfoSetting.value("RXServo").toString();
        terminalIO = upgradeInfoSetting.value("RXTerminal").toString();
    }

    if(CommonData::getControllerType().name.contains("CR20A"))
    {
        if(upgradeInfoSetting.value("CR20Terminal").toString().isEmpty())
        {
            terminalIO = upgradeInfoSetting.value("RXTerminal").toString();
        }
        else
        {
            terminalIO = upgradeInfoSetting.value("CR20Terminal").toString();
        }
    }

    if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6) &&
            CommonData::getCurrentSettingsVersion().servo1.size()>0)
    {
        if(CommonData::getCurrentSettingsVersion().servo1.at(0)=='3'){
            upgradeSettingServo = upgradeInfoSetting.value("ED6ServoV3").toString();
        }else if(CommonData::getCurrentSettingsVersion().servo1.at(0)=='2'){
            upgradeSettingServo = upgradeInfoSetting.value("ED6ServoV2").toString();
        }
        terminalIO = upgradeInfoSetting.value("ED6Terminal").toString();
    }

    qDebug()<<"upgradeSettingServo  "<<upgradeSettingServo;
    settingVersion.servo1 = upgradeSettingServo;
    settingVersion.servo2 = upgradeSettingServo;
    settingVersion.servo3 = upgradeSettingServo;
    settingVersion.servo4 = upgradeSettingServo;
    settingVersion.servo5 = upgradeSettingServo;
    settingVersion.servo6 = upgradeSettingServo;

    settingVersion.terminal = terminalIO;
    CommonData::setUpgradeSettingsVersion(settingVersion);
    ui->labelUpgradeMainControlVersion->setText(m_upgradeVersion);
    if(!mainControl.isEmpty()){
        ui->labelUpgradeMainControlVersion->setText(mainControl);
    }else{
        ui->labelUpgradeMainControlVersion->setText("-");
    }
    if(!CCBOXText.isEmpty()){
        ui->labelUpgradeIOFirmwareVersion->setText(CCBOXText);
    }else{
        ui->labelUpgradeIOFirmwareVersion->setText("-");
    }
    if(!feedbackText.isEmpty()){
        ui->labelUpgradeFeedbackVersion->setText(feedbackText);
    }else{
        ui->labelUpgradeFeedbackVersion->setText("-");
    }

    if(!safeIO.isEmpty()){
        ui->labelUpgradeSafeIOVersion->setText(safeIO);
    }else{
        ui->labelUpgradeSafeIOVersion->setText("-");
    }

    if(!upgradeSettingServo.isEmpty()){
        ui->labelUpgradeJ1Version->setText(upgradeSettingServo);
        ui->labelUpgradeJ2Version->setText(upgradeSettingServo);
        ui->labelUpgradeJ3Version->setText(upgradeSettingServo);
        ui->labelUpgradeJ4Version->setText(upgradeSettingServo);
        ui->labelUpgradeJ5Version->setText(upgradeSettingServo);
        ui->labelUpgradeJ6Version->setText(upgradeSettingServo);
    }else{
        ui->labelUpgradeJ1Version->setText("-");
        ui->labelUpgradeJ2Version->setText("-");
        ui->labelUpgradeJ3Version->setText("-");
        ui->labelUpgradeJ4Version->setText("-");
        ui->labelUpgradeJ5Version->setText("-");
        ui->labelUpgradeJ6Version->setText("-");
    }

    if(!uniIO.isEmpty()){
        ui->labelUpgradeUnIOVersion->setText(uniIO);
    }else{
        ui->labelUpgradeUnIOVersion->setText("-");
    }

    if(!terminalIO.isEmpty()){
        ui->labelUpgradeTerminalVersion->setText(terminalIO);
    }else{
        ui->labelUpgradeTerminalVersion->setText("-");
    }
    upgradeInfoSetting.endArray();
}

bool MainWidget2::isCurrentVersionEnableUpgrade()
{
    if(!CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
    {
        if(CommonData::getCurrentSettingsVersion().safeio.isEmpty()||
                CommonData::getCurrentSettingsVersion().safeio == "0.")
        {
            ui->btnUpgrade->setDisabled(true);
            return false;
        }

        if(CommonData::getCurrentSettingsVersion().feedback.isEmpty()||
                CommonData::getCurrentSettingsVersion().feedback == "0.")
        {
            ui->btnUpgrade->setDisabled(true);
            return false;
        }
    }

    if(CommonData::getCurrentSettingsVersion().control.isEmpty()||
            CommonData::getCurrentSettingsVersion().control == "0.")
    {
        ui->btnUpgrade->setDisabled(true);
        return false;
    }

    if(CommonData::getCurrentSettingsVersion().unio.isEmpty()||
            CommonData::getCurrentSettingsVersion().unio == "0.")
    {
        ui->btnUpgrade->setDisabled(true);
        return false;
    }

    if(CommonData::getCurrentSettingsVersion().servo1.isEmpty()||
            CommonData::getCurrentSettingsVersion().servo1 == "0.")
    {
        ui->btnUpgrade->setDisabled(true);
        return false;
    }

    if(CommonData::getCurrentSettingsVersion().servo2.isEmpty()||
            CommonData::getCurrentSettingsVersion().servo2 == "0.")
    {
        ui->btnUpgrade->setDisabled(true);
        return false;
    }

    if(CommonData::getCurrentSettingsVersion().servo3.isEmpty()||
            CommonData::getCurrentSettingsVersion().servo3 == "0.")
    {
        ui->btnUpgrade->setDisabled(true);
        return false;
    }

    if(CommonData::getCurrentSettingsVersion().servo4.isEmpty()||
            CommonData::getCurrentSettingsVersion().servo4 == "0.")
    {
        ui->btnUpgrade->setDisabled(true);
        return false;
    }

    if(CommonData::getCurrentSettingsVersion().servo5.isEmpty()||
            CommonData::getCurrentSettingsVersion().servo5 == "0.")
    {
        ui->btnUpgrade->setDisabled(true);
        return false;
    }

    if(CommonData::getCurrentSettingsVersion().servo6.isEmpty()||
            CommonData::getCurrentSettingsVersion().servo6 == "0.")
    {
        ui->btnUpgrade->setDisabled(true);
        return false;
    }

    if(CommonData::getCurrentSettingsVersion().terminal.isEmpty()||
            CommonData::getCurrentSettingsVersion().terminal == "0.")
    {
        ui->btnUpgrade->setDisabled(true);
        return false;
    }
    return true;
}

#define EXPORT_SERVO_PARAMS(num) \
    QJsonObject jsonObject##num;\
    (jsonObject##num).insert("servoNum",servoJ);\
    (jsonObject##num).insert("key",g_strServoParamAddr##num);\
    (jsonObject##num).insert("value",ui->spinServoParam##num->value());\
    qDebug()<<(jsonObject##num).value("servoNum").toString();\
    jsonArray.append((jsonObject##num));\
    qDebug()<<"\'"#num"\' sssff"
void MainWidget2::exportServoParamsFile(QString exportFileDir)
{
    auto pWndMsg = new MessageWidget(this);

    QJsonDocument jsonDoc;
    QJsonArray jsonArray;
    QJsonObject jsonObject;
    QString servoJ = ui->comboServo->currentText();
    QStandardItem* item = mServoParamsTableModel->item(0,colParaIndex);
    if(item == nullptr || item->text().isEmpty())
    {
        pWndMsg->setMessage("warn",tr("未导入Excel文件"));
        pWndMsg->show();
        return;
    }

    for(int i = 0; i < mServoParamsTableModel->rowCount();i++)
    {
        QStandardItem* itemIsRead = mServoParamsTableModel->item(i,colIsReadOnly);
        if(itemIsRead == nullptr || itemIsRead->text() == g_strServoParamsColValueTrue){
            continue;
        }

        QJsonObject jsonObject;
        QModelIndex dotDividedBitsIndex = mServoParamsTableModel->index(i, colDotDividedBits, QModelIndex());
        int dotDividedBits = mServoParamsTableModel->data(dotDividedBitsIndex).toInt();
        QModelIndex index = mServoParamsTableModel->index(i, colValue, QModelIndex());
        double value = mServoParamsTableModel->data(index).toDouble();
        double writeValue = dotDividedBitsValue(false,dotDividedBits,value);
        QStandardItem* item = mServoParamsTableModel->item(i,colParaIndex);
        if(item == nullptr || item->text().isEmpty())
        {
            qDebug()<<"exportServoParamsFile item->text().isEmpty() ";
            continue;
        }
        QString paramIndex = item->text();
        qDebug()<<"exportServoParamsFile paramIndex  "<<paramIndex;
        QString address = "addr"+paramIndex2Address(paramIndex);
        qDebug()<<"exportServoParamsFile  address  "<<address;

        jsonObject.insert("servoNum",servoJ);
        jsonObject.insert("key",address);
        jsonObject.insert("value",writeValue);
        qDebug()<<jsonObject.value("servoNum").toString();
        jsonArray.append(jsonObject);
    }

    jsonObject.insert("body",jsonArray);
    jsonDoc.setObject(jsonObject);
    QByteArray json = jsonDoc.toJson();

    //写文件
    QFile file(exportFileDir+"/servoParams.json");
    file.open(QIODevice::WriteOnly);
    file.write(json);
    file.close();

    pWndMsg->setMessage("",tr("导出所有伺服参数成功\n文件名称为servoParams.json"));
    pWndMsg->show();
}

void MainWidget2::initLanguage()
{
    int languageIndex = 0;
    QSettings setting("config/config.ini",QSettings::IniFormat);
    QVariant var = setting.value("language/style");
    if (var.isNull())
    {
        QLocale locale;
        auto lang = locale.language();
        auto cou = locale.country();
        if (QLocale::Chinese == lang)
        {
            languageIndex = 0;
        }
        else
        {
            languageIndex = 1;
        }
    }
    else
    {
        languageIndex = var.toInt();
    }
    qDebug()<<"languageIndex  "<<languageIndex;
    if(languageIndex == 0)
    {
        g_isEnglish = true;
        ui->btnLanguage->setChecked(true);
        ui->btnLanguage->clicked(true);
    }
    else
    {
        g_isEnglish = false;
        ui->btnLanguage->setChecked(false);
        ui->btnLanguage->clicked(false);
    }

    LangeuageFontChange(g_isEnglish);
    return;
}

int MainWidget2::upgradeFwAndXmlStatusCheck(int upgradeFWStatus, int upgradeXMLStatus)
{
    if(upgradeFWStatus == AllUpgradeStatus_FAIILED){
        return AllUpgradeStatus_FWFAIILED;
    }
    if(upgradeXMLStatus == AllUpgradeStatus_FAIILED){
        return AllUpgradeStatus_XMLFAIILED;
    }
    if(upgradeFWStatus == AllUpgradeStatus_SUCCESS && upgradeXMLStatus == AllUpgradeStatus_SUCCESS){
        return AllUpgradeStatus_SUCCESS;
    }
    if(upgradeFWStatus == AllUpgradeStatus_INIT && upgradeXMLStatus == AllUpgradeStatus_INIT)
    {
        return AllUpgradeStatus_INIT;
    }

    return AllUpgradeStatus_DOING;
}

void MainWidget2::widget2UpgradeFileCopyToControlFinish(bool ok)
{
    if(!ok){
        qWarning()<<"固件导入控制柜失败！ 文件传输失败";
        return;
    }else{
        sleep(1000);
        if(!CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
        {
            QString updateDiskResult = m_httpProtocol->postUpdateDiskCheck(g_strCurrentIP);
            if(updateDiskResult.isEmpty() || updateDiskResult != "success"){
                m_upgrade2Widget->setDiskCheckStatus(-1);
                qDebug()<<"postUpdateDisk  disk "+updateDiskResult;
                return;
            }
        }
        m_upgrade2Widget->setDiskCheckStatus(2);
        QString upgradeControl;
        if(!CommonData::getUpgradeSettingsVersion().control.isEmpty())
        {
            upgradeControl = CommonData::getUpgradeSettingsVersion().control.mid(0,2);
        }
        else
        {
            qDebug()<<"no upgrade control";
            return;
        }
        if(upgradeControl.at(0) == 'V' || upgradeControl.at(0) == 'v')
        {
            upgradeControl = upgradeControl.mid(1);
        }
        QString currentControl = CommonData::getCurrentSettingsVersion().control.mid(0,2);
        if(currentControl.at(0) == 'V' || currentControl.at(0) == 'v')
        {
            currentControl = currentControl.mid(1);
        }
        if(upgradeControl.at(0) == '3' && currentControl.at(0) == '3')
        {

            QString cabVersion;
            auto cabVersionLua = CLuaApi::getInstance()->getCabVersion();
            if (cabVersionLua.hasValue())
            {
                cabVersion = cabVersionLua.value();
            }
            else{
                if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)
                        ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
                {
                    cabVersion = "V1";
                }
                else
                {
                    cabVersion = "V2";
                }
            }

            if(CommonData::getCurrentSettingsVersion().control.at(0)!='4')
            {
                QString result = m_httpProtocol->postUpdateCheckKernelVersion(g_strCurrentIP,cabVersion);
                if(result.isEmpty()||result!="same")
                {
                    widget2UpdateDiskKernel();
                    return;
                }
            }
        }
        Widget2MainControlUpgrade();
    }
}

void MainWidget2::Widget2MainControlUpgrade()
{
    QDir dir(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion);
    if(!dir.exists()){
        return;
    }
    QFileInfoList fileList =  dir.entryInfoList();
    QString A9UpdatePath;
    QString A9FileName;
    for(QFileInfo fileInfo:fileList){
        auto info = getMatchCRUpdateInfo(fileInfo.fileName());
        if(info.valid){
            A9UpdatePath = fileInfo.absoluteFilePath();
            A9FileName = fileInfo.fileName();
        }
    }
    if(A9UpdatePath.isEmpty()){
        qDebug()<<"A9UpdatePath is empyt";
        m_upgrade2Widget->setDownloadFirmwareStatus(-1);
        return;
    }
    m_A9UpdatePath = A9UpdatePath;
    qDebug()<<"A9UpdatePath "<<A9UpdatePath <<"  g_strCurrentIP  "<<g_strCurrentIP;
    qDebug()<<"A9FileName "<<"upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/"+A9FileName;

    upgradeAllFirmwareAndXMLs();
}

void MainWidget2::setHideServo6JWidget(bool isHide)
{
    if(isHide)
    {
        ui->labelSingleServoJ1->hide();
        ui->textSingleServoJ1->hide();
        ui->btnSingleServoJ1->hide();
        ui->textSingleServoJ1XML->hide();
        ui->btnSingleServoJ1XML->hide();

        ui->labelSingleServoJ2->hide();
        ui->textSingleServoJ2->hide();
        ui->btnSingleServoJ2->hide();
        ui->textSingleServoJ2XML->hide();
        ui->btnSingleServoJ2XML->hide();

        ui->labelSingleServoJ3->hide();
        ui->textSingleServoJ3->hide();
        ui->btnSingleServoJ3->hide();
        ui->textSingleServoJ3XML->hide();
        ui->btnSingleServoJ3XML->hide();

        ui->labelSingleServoJ4->hide();
        ui->textSingleServoJ4->hide();
        ui->btnSingleServoJ4->hide();
        ui->textSingleServoJ4XML->hide();
        ui->btnSingleServoJ4XML->hide();

        ui->labelSingleServoJ5->hide();
        ui->textSingleServoJ5->hide();
        ui->btnSingleServoJ5->hide();
        ui->textSingleServoJ5XML->hide();
        ui->btnSingleServoJ5XML->hide();

        ui->labelSingleServoJ6->hide();
        ui->textSingleServoJ6->hide();
        ui->btnSingleServoJ6->hide();
        ui->textSingleServoJ6XML->hide();
        ui->btnSingleServoJ6XML->hide();
    }
    else
    {
        ui->labelSingleServoJ1->show();
        ui->textSingleServoJ1->show();
        ui->btnSingleServoJ1->show();
        ui->textSingleServoJ1XML->show();
        ui->btnSingleServoJ1XML->show();

        ui->labelSingleServoJ2->show();
        ui->textSingleServoJ2->show();
        ui->btnSingleServoJ2->show();
        ui->textSingleServoJ2XML->show();
        ui->btnSingleServoJ2XML->show();

        ui->labelSingleServoJ3->show();
        ui->textSingleServoJ3->show();
        ui->btnSingleServoJ3->show();
        ui->textSingleServoJ3XML->show();
        ui->btnSingleServoJ3XML->show();

        ui->labelSingleServoJ4->show();
        ui->textSingleServoJ4->show();
        ui->btnSingleServoJ4->show();
        ui->textSingleServoJ4XML->show();
        ui->btnSingleServoJ4XML->show();

        ui->labelSingleServoJ5->show();
        ui->textSingleServoJ5->show();
        ui->btnSingleServoJ5->show();
        ui->textSingleServoJ5XML->show();
        ui->btnSingleServoJ5XML->show();

        ui->labelSingleServoJ6->show();
        ui->textSingleServoJ6->show();
        ui->btnSingleServoJ6->show();
        ui->textSingleServoJ6XML->show();
        ui->btnSingleServoJ6XML->show();
    }
}

bool MainWidget2::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        if(!ui->btnLanguage->isChecked())
        {
            g_isEnglish = true;
        }
        else
        {
            g_isEnglish = false;
        }
        //保存当前的值
        QString labelDNSStatusText = ui->labelDNSStatus->text();
        QString labelMACStatusText = ui->labelMACStatus->text();
        ui->retranslateUi(this);

        if(labelDNSStatusText.contains("正常") || labelDNSStatusText.contains("normal"))
        {
            ui->labelDNSStatus->setText(tr("正常"));
        }
        else if(labelDNSStatusText.contains("异常") || labelDNSStatusText.contains("abnormal"))
        {
            ui->labelDNSStatus->setText(tr("异常"));
        }

        if(labelMACStatusText.contains("正常") || labelMACStatusText.contains("normal"))
        {
            ui->labelMACStatus->setText(tr("正常"));
        }
        else if(labelMACStatusText.contains("异常") || labelMACStatusText.contains("abnormal"))
        {
            ui->labelMACStatus->setText(tr("异常"));
        }

        if(ui->boxDevice->currentText().contains("搜索设备中") || ui->boxDevice->currentText().contains("searching")){
            ui->boxDevice->setCurrentText(tr("搜索设备中..."));
        }

        if(ui->btnConnect->property("isConnected").toInt()==1){
            ui->btnConnect->setText(tr("连接中"));
        }else if(ui->btnConnect->property("isConnected").toInt()==2){
            ui->btnConnect->setText(tr("断开连接"));
        }
        if(isOnline==true){
            ui->btnUpgrade->setText(tr("下载并升级"));
        }
        if(g_isEnglish){
            //更新下面两个翻译
            ui->labelRandomNum->setText(QString(tr("Random code: %1")).arg(m_currentPasswordRandom));
        }else{
            //更新下面两个翻译
            ui->labelRandomNum->setText(QString(tr("随机码:  %1")).arg(m_currentPasswordRandom));
        }
        if(CommonData::getStrPropertiesCabinetType().name == g_strPropertiesCabinetTypeCC262)
        {
            ui->labelCurrentSafeIOTheme->setText(tr("安全IO板A："));
        }
        if(ui->btnConnect->property("isConnected").toInt()==2){
            if(!m_upgradeVersion.isEmpty()){
                readUpgradeVersion(m_upgradeParentVersion,m_upgradeVersion);
            }
            if(!CommonData::getCurrentSettingsVersion().control.isEmpty())
            {
                ui->labelCurrentMainControlVersion->setAttribute(Qt::WA_AlwaysShowToolTips);
                setLabelToolTip(ui->labelCurrentMainControlVersion,CommonData::getCurrentSettingsVersion().control);
                ui->textSingleMainControl->setAttribute(Qt::WA_AlwaysShowToolTips);
                setLabelToolTip(ui->textSingleMainControl,CommonData::getCurrentSettingsVersion().control);
            }

            ui->labelCurrentSafeIOVersion->setText(CommonData::getCurrentSettingsVersion().safeio);
            ui->labelCurrentIOFirmwareVersion->setText(CommonData::getCurrentSettingsVersion().unio);
            ui->labelCurrentFeedbackVersion->setText(CommonData::getCurrentSettingsVersion().feedback);
            ui->labelCurrentUnIOVersion->setText(CommonData::getCurrentSettingsVersion().unio);
            ui->labelCurrentJ1Version->setText(CommonData::getCurrentSettingsVersion().servo1);
            ui->labelCurrentJ2Version->setText(CommonData::getCurrentSettingsVersion().servo2);
            ui->labelCurrentJ3Version->setText(CommonData::getCurrentSettingsVersion().servo3);
            ui->labelCurrentJ4Version->setText(CommonData::getCurrentSettingsVersion().servo4);
            ui->labelCurrentJ5Version->setText(CommonData::getCurrentSettingsVersion().servo5);
            ui->labelCurrentJ6Version->setText(CommonData::getCurrentSettingsVersion().servo6);
            ui->labelCurrentTerminalVersion->setText(CommonData::getCurrentSettingsVersion().terminal);

            ui->textSingleSafeIO->setText(CommonData::getCurrentSettingsVersion().safeio);
            ui->textSingleFeedback->setText(CommonData::getCurrentSettingsVersion().feedback);
            ui->textSingleUniIO->setText(CommonData::getCurrentSettingsVersion().unio);
            ui->textSingleCCBOX->setText(CommonData::getCurrentSettingsVersion().unio);
            ui->textSingleServoJ1->setText(CommonData::getCurrentSettingsVersion().servo1);
            ui->textSingleServoJAll->setText(CommonData::getCurrentSettingsVersion().servo1);
            ui->textSingleServoJ2->setText(CommonData::getCurrentSettingsVersion().servo2);
            ui->textSingleServoJ3->setText(CommonData::getCurrentSettingsVersion().servo3);
            ui->textSingleServoJ4->setText(CommonData::getCurrentSettingsVersion().servo4);
            ui->textSingleServoJ5->setText(CommonData::getCurrentSettingsVersion().servo5);
            ui->textSingleServoJ6->setText(CommonData::getCurrentSettingsVersion().servo6);
            ui->textSingleTerminal->setText(CommonData::getCurrentSettingsVersion().terminal);
        }

        ui->btnConnect->repaint();
        QVector<QWidget*> vcWnd={ui->btnUpgradeWidgetServoParamAllWrite,ui->btnMenu,
                                 m_upgrade2Widget,m_diyProgressDialog,m_pSNAlarmBubbleTipWidget,m_widgetSingleUpgrading,
                                 m_pFormAbout,m_isDisableControlWidget,m_isControlLogBusyWidget,
                                 m_widgetSystemToolTip,m_widgetSingleUpgrade,m_widgetSingleUpgradeMainControl};
        for (auto pWnd : vcWnd){
            if (pWnd) QCoreApplication::sendEvent(pWnd, event);
        }
        return true;
    }
    return QWidget::event(event);
}

/*********************************************************************************/
/*********************************************************************************/
void MainWidget2::slotSNAlarmed(bool isAlarm)
{
    if(isAlarm)
    {
        if (!m_pSNAlarmBubbleTipWidget)
        {
            m_pSNAlarmBubbleTipWidget = new SNAlarmBubbleTipWidget(this);
        }
        int x = (this->width()-m_pSNAlarmBubbleTipWidget->width())/2;
        m_pSNAlarmBubbleTipWidget->setGeometry(x,ui->titleWidget->height()+10
                                               ,m_pSNAlarmBubbleTipWidget->width(),m_pSNAlarmBubbleTipWidget->height());
        m_pSNAlarmBubbleTipWidget->show();
    }
    else
    {
        if (nullptr != m_pSNAlarmBubbleTipWidget) m_pSNAlarmBubbleTipWidget->hide();
    }
}
void MainWidget2::slotFromTitleBarToSN(bool isSNEmpty)
{
    if(isSNEmpty)
    {
        ui->btnSystemSNWrite->show();
        ui->btnSystemSNRepair->hide();
    }
    else
    {
        ui->btnSystemSNWrite->hide();
        ui->btnSystemSNRepair->show();
    }
}

void MainWidget2::slot_ExportLogWhenError(int iType)
{
    QString strDir = QFileDialog::getExistingDirectory(nullptr,QString(),QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(strDir.isEmpty())
    {
        return;
    }
    if (!strDir.endsWith('/') && !strDir.endsWith('\\'))
    {
        strDir.append(QDir::separator());
    }

    auto pfnShowEvent=[this,iType](bool bVisible){
        switch(iType)
        {
        case 1:
            if (bVisible) {
                m_upgrade2Widget->setGeometry(0,0,this->width(),this->height());
                m_upgrade2Widget->raise();
            }
            m_upgrade2Widget->setVisible(bVisible);
            break;
        }
    };
    pfnShowEvent(false);

    QEventLoop loop;
    QRect rc = PublicgGetMainWindow()->geometry();
    auto pLoading = new LoadingUI(PublicgGetMainWindow());
    pLoading->setGeometry(0,0,rc.width(),rc.height());
    pLoading->show();
    auto pLink = new PlinkCmd(pLoading);
    pLink->setIp(g_strCurrentIP);
    pLink->setPscp(QCoreApplication::applicationDirPath()+"/tool/plink/pscp.exe");
    //QObject::connect(pLink, &PlinkCmd::signalDownloadFileProgress, pLoading, &LoadingUI::setText);
    QObject::connect(pLink, &PlinkCmd::signalFinishedDownloadFile, pLoading,[&]{
        QtConcurrent::run([&]{
            QDir dir(CLogger::getInstance()->getLogDir());
            QStringList filter;
            filter << "*-dbg.log";
            QFileInfoList fileInfo = dir.entryInfoList(filter);
            for (int i=0; i<fileInfo.count(); i++)
            {
                //emit pLink->signalDownloadFileProgress(fileInfo[i].absoluteFilePath());
                qDebug()<<QFile::copy(fileInfo[i].absoluteFilePath(),strDir+fileInfo[i].fileName());
            }
            qDebug()<<"维护工具日志文件复制完毕";
            loop.quit();
        });
    });
    const QString strLog="/mToolServer"; //控制器日志
    pLink->execDownloadFile(QList<QPair<QString,QString>>()<<qMakePair(strLog,strDir));
    loop.exec();
    pLoading->hide();
    pLoading->deleteLater();

    auto pMsgBox = new DlgMsgBox(this);
    connect(pMsgBox, &DlgMsgBox::destroyed, this, [pfnShowEvent]{
        pfnShowEvent(true);
    });
    pMsgBox->setTitle(tr("导出完成"),DlgMsgBox::Type::ok);
    pMsgBox->setOk(tr("好的"),true);
    pMsgBox->show();
}

void MainWidget2::slot_btnLookXmlVersion()
{
    auto ptr = new FormLookXmlVersion(this);
    ptr->setGeometry(0,0,width(),height());
    ptr->raise();
    ptr->show();
}

void MainWidget2::slot_btnSystemMacView()
{
    auto ptr = new FormMacDetail(this,m_allNetworkCardInfo);
    ptr->show();
}

void MainWidget2::slot_btnLogExport()
{
    slot_ExportLogWhenError(0);
}

void MainWidget2::slot_btnAboutMe()
{
    QSize sz = ui->titleWidget->size();
    if (!m_pFormAbout)
    {
        m_pFormAbout = new FormAbout(this);
        connect(m_pFormAbout, &FormAbout::emitLogExport, this, &MainWidget2::slot_btnLogExport);
    }
    m_pFormAbout->setGeometry(0,sz.height(),width(),height()-sz.height());
    m_pFormAbout->raise();
    m_pFormAbout->show();
}

void MainWidget2::slotClickVersionText()
{
    QString filePath;
    if(g_isEnglish){
        filePath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/"+m_upgradeVersion+"detailEnglish.txt";
    }else{
        filePath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/"+m_upgradeVersion+"detailChinese.txt";
    }
    QFile file(filePath);
    if (file.open(QFile::ReadOnly))
    {
        QString str = file.readAll();
        file.close();
        auto ptr = new FormPackageVersionDetail(str,this);
        ptr->setGeometry(0,0,width(),height());
        ptr->raise();
        ptr->show();
    }
}

void MainWidget2::slot_DownloadRecovery()
{
    const QString strFile = getFactoryRecoveryFile();
    QFile::remove(strFile);

    QStringList ver = CommonData::getCurrentSettingsVersion().control.split('.');
    QString strPack;
    if (ver.size()>=3){
        strPack = ver[0]+'.'+ver[1]+'.'+ver[2];
    }else{
        strPack = CommonData::getCurrentSettingsVersion().control;
    }
    const QString strUrl = QString("%1/%2/backdefault.json").arg(g_strHttpDobotPrefix, strPack);

    auto pLoading = new LoadingUI(this);
    pLoading->setGeometry(0,0,width(),height());
    const QString strTips = QString("<html><head/><body><font color=\"red\">%1</font></body></html>").arg(tr("正在下载云端配置文件"));
    pLoading->setBigText(strTips);
    pLoading->show();

    auto pLoop = new QEventLoop(pLoading);
    auto pDownload = new DownloadTool(pLoading);
    connect(pDownload,&DownloadTool::sigDownloadFinished,this, [pLoop](bool ok){
        qDebug()<<"tmpfactoryrecovery.json download ------------------>"<<ok;
        pLoop->setProperty("downloadok",ok);
        pLoop->quit();
    });
    pDownload->startDownloadFile(strUrl,strFile);
    pLoop->exec();
    bool bOk = pLoop->property("downloadok").toBool();
    pLoading->hide();
    pLoading->deleteLater();

    auto* messageWidget = new MessageWidget(this);
    messageWidget->setMessage(bOk?"ok":"warn", bOk?tr("下载成功"):tr("下载失败"));
    messageWidget->show();
}
void MainWidget2::slot_RecoveryFactory()
{
    QEventLoop loop;
    auto pDlg = new DlgMsgBox(this);
    connect(pDlg, &DlgMsgBox::destroyed, &loop, &QEventLoop::quit);
    connect(pDlg, &DlgMsgBox::clickedOk, &loop, [&loop]{
        loop.setProperty("recoveryok",true);
    });
    connect(pDlg, &DlgMsgBox::clickedCancel, &loop, [&loop]{
        loop.setProperty("recoveryok",false);
    });
    pDlg->setTitle(tr("是否确认恢复出厂默认标准？"),DlgMsgBox::Type::none);
    pDlg->setOk(tr("是"));
    pDlg->setCancel(tr("否"));
    pDlg->show();
    loop.exec();
    if (!loop.property("recoveryok").toBool()) return;

    auto pLoading = new LoadingUI(this);
    pLoading->setGeometry(0,0,width(),height());
    QString strTips = QString("<html><head/><body><font color=\"red\">%1</font></body></html>").arg(tr("正在进行恢复出厂默认标准，此过程中请勿关机"));
    pLoading->setBigText(strTips);
    pLoading->show();
    const QString strFile = getFactoryRecoveryFile();
    auto result = CLuaApi::getInstance()->execRecoveryFactory(strFile);
    pLoading->hide();
    pLoading->deleteLater();

    if (result.second.isEmpty()){
        if (result.first) strTips = tr("恢复出厂默认标准成功");
        else strTips = tr("恢复出厂默认标准失败");
    }else{
        strTips = result.second;
    }
    auto* messageWidget = new MessageWidget(this);
    messageWidget->setMessage(result.first?"ok":"warn", strTips);
    messageWidget->show();
}
