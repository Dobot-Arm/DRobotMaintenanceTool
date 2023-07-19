#include "MainWidget2.h"
#include "ui_MainWidget2.h"
typedef long (*ZWZipCompress)(const char* lpszSourceFiles, const char* lpszDestFile, bool bUtf8);
typedef long (*ZWZipExtract)(const char* lpszSourceFiles, const char* lpszDestFile);
MainWidget2::MainWidget2(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::MainWidget2)
    , mouse_press(false)
    , m_allVersionSetting(QCoreApplication::applicationDirPath()+"/upgradeFiles/allVersion.ini",QSettings::IniFormat)
    , m_macAddressSetting(QCoreApplication::applicationDirPath()+"/config/macAddress.ini",QSettings::IniFormat)
    ,m_posDiyX(QApplication::desktop()->geometry().width()/2-this->width()/2)
    ,m_posDiyY(QApplication::desktop()->geometry().height()/2-this->height()/2)
{
    ui->setupUi(this);

//    DLogger::initLog();
    QWidget::setAttribute(Qt::WA_QuitOnClose,true);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);//无边框，置顶
    m_Trans = new QTranslator(this);

    ui->widgetFirmwareCheck->hide();

    m_widgetV4SN = new WidgetV4SN();
    connect(m_widgetV4SN,&WidgetV4SN::singal_wigdetV4SNToUpgradeDevice,this,[&](QString controlAndSN,QString V4ExportDir){
        m_V4ControlAndSN = controlAndSN;
        qDebug()<<" m_V4ControlAndSN "<<m_V4ControlAndSN;
        m_V4ExportDir = V4ExportDir;
        qDebug()<<" m_V4ExportDir "<<m_V4ExportDir;
        widget2UpgradeDevice();
    });



    connect(ui->btnLanguage,&QPushButton::clicked,this,&MainWidget2::slot_btnLanguageChange);



    ShadowWindow* shawdowUpgradeIsOldElectronicSkin = new ShadowWindow();
    m_upgradeIsOldElectronicSkinWidget = new UpgradeIsOldElectronicSkinWidget();
    shawdowUpgradeIsOldElectronicSkin->setTopWidget(this);
    shawdowUpgradeIsOldElectronicSkin->installWidget(m_upgradeIsOldElectronicSkinWidget);

    connect(m_upgradeIsOldElectronicSkinWidget,&UpgradeIsOldElectronicSkinWidget::signal_upgradeIsOldElectronicSkinWidget_toUpgradeDevice,this,[&]{
        widget2UpgradeDevice();
    });
    m_httpProtocol = new DobotProtocol();
    m_zipLogExportPassword = "dobot123";
    m_zipDiyManager = new ZipDiyManager();
    m_csFtpClient = new CSFtpClient();
    m_controlAutoConnect = new SmallControlAutoConnect();

    m_pFileTransfer = new QProcess();


    connect(m_pFileTransfer,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<"m_pFileTransfer  exitCode "<<exitCode <<" exitStatus "<<exitStatus;

        //由于V4回退V3 会覆盖之前的files 需重新上传
        if(m_isControlBack)
        {
            qDebug()<<"m_isControlBack true ";
            m_isControlBack = false;
            upgradeAllFirmwareAndXMLs();
            return;
        }

        if(m_isInterruptUpgrade)
        {
            qDebug()<<"m_pFileTransfer,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished  m_isInterruptUpgrade";
            return;
        }

        qDebug()<<" widget2UpgradeFileCopyToControlFinish not 3541 351  ";
        widget2UpgradeFileCopyToControlFinish(true);

    });

    m_pFileSingleTransfer = new QProcess();
    connect(m_pFileSingleTransfer,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<"m_pFileSingleTransfer,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished)  ";
        if(CommonData::getStrPropertiesCabinetType().contains("CC"))
        {
            qDebug()<<"m_updateFile1  "<<m_updateFile1;
            qDebug()<<"m_updateFile2  "<<m_updateFile2;
            wigdet2UpdateCRSingleFwAndXML(m_updateFile1,m_updateFile2);
        }
    });


    m_pFileServoParamsTransfer = new QProcess();
    connect(m_pFileServoParamsTransfer,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<"m_pFileServoParamsTransfer,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished)  ";
        DobotType::StructSettingsServoParams settingsServoParams;
        settingsServoParams.src = "files";
        bool result = m_httpProtocol->postSettingsModifyServoParams(m_currentIp,settingsServoParams);
        if(result == true)
        {
            int intSleepTime =m_servoParamsSleep*1800;
            qDebug()<<"m_servoParamsSleep sleepTime "<<intSleepTime;
            sleep(intSleepTime);

            DobotType::StructSettingsServoParamsResult result = m_httpProtocol->getSettingsModifyServoParams(m_currentIp);
            if(result.status == true){
                QString errorParamAddList;
                for(DobotType::StructServoParam structServoParam : result.servoParams)
                {
                    if(structServoParam.status != true)
                    {
                        errorParamAddList.append(structServoParam.key+"\n");

                    }
                }
                qDebug()<<" errorParamAddList "<<errorParamAddList;
                if(errorParamAddList.isEmpty()){
                    m_widget2ServoParamsWait->setStatus(2,tr("写入所有成功"));
                }else{
                    m_widget2ServoParamsWait->setStatus(-2,tr("写入所有失败"),errorParamAddList);
                }

            }else{
                m_widget2ServoParamsWait->setStatus(-1,tr("写入所有失败"));
            }

        }
        else
        {
            m_widget2ServoParamsWait->setStatus(-1,tr("写入所有失败"));
        }
    });
    m_widgetSingleUpgrade = new WidgetSingleUpgrade();
    ShadowWindow* s14 = new ShadowWindow(this);
    s14->setTopWidget(this);
    s14->installWidget(m_widgetSingleUpgrade);
    connect(m_widgetSingleUpgrade,&WidgetSingleUpgrade::signal_upgradeSingleFwOrXML,this,&MainWidget2::slot_upgradeSingleFwOrXML);


    m_widgetSystemFileCheckTip = new WidgetSystemFileCheckTip();
    ShadowWindow* widgetSystemFileCheckShadow = new ShadowWindow(this);
    widgetSystemFileCheckShadow->setTopWidget(this);
    widgetSystemFileCheckShadow->installWidget(m_widgetSystemFileCheckTip);


    m_widget2FileBackupProgressDialog = new Widget2ProgressDialog();

    m_widget2FileRecoveryProgressDialog = new Widget2ProgressDialog();
    connect(m_csFtpClient,&CSFtpClient::signalOnChannelFileInfoAvailableFinish,this,&MainWidget2::slot_csFtpClientOnChannelFileInfoAvailableFinish);
    connect(m_csFtpClient,&CSFtpClient::signalFinishedJob,this,&MainWidget2::slot_cSFtpClientFinishedJob);

    connect(ui->btnClose,&QPushButton::clicked,this,&MainWidget2::slot_closeWindow);
    connect(ui->btnMini,&QPushButton::clicked,this,&MainWidget2::slot_miniWindow);
    connect(ui->btnMax,&QPushButton::clicked,this,&MainWidget2::slot_maxWindow);


    connect(ui->btnBackToUpgrade,&QPushButton::clicked,this,&MainWidget2::slot_backToUpgrade);
    connect(ui->btnSingleFunc,&QPushButton::clicked,this,&MainWidget2::slot_singleFunc);


    searchDeviceDisable(true);
    m_softwareDownload = new DownloadTool();
    connect(m_softwareDownload,&DownloadTool::sigDownloadFinished,this,&MainWidget2::slot_downloadedSoftwareTime);
    m_softwareDownload->startDownload("http://cdn.release.dobot.cc/dobotUpgradeToolFile/softWareInfo.ini"
                            ,QCoreApplication::applicationDirPath()+"/config");

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


    connect(ui->btnFirmwareUpgrade,&QPushButton::clicked,this,&MainWidget2::slot_onPanelBoxChanged);
    connect(ui->btnFileRecovery,&QPushButton::clicked,this,&MainWidget2::slot_onPanelBoxChanged);
    connect(ui->btnFirmwareCheck,&QPushButton::clicked,this,&MainWidget2::slot_onPanelBoxChanged);
    connect(ui->btnSystemTool,&QPushButton::clicked,this,&MainWidget2::slot_onPanelBoxChanged);
    connect(ui->btnBackup,&QPushButton::clicked,this,&MainWidget2::slot_onPanelBoxChanged);

    m_widgetSystemSyncTimePassword = new WidgetServoParamsPassword();
    connect(m_widgetSystemSyncTimePassword,&WidgetServoParamsPassword::signal_servoParamsPassword,this,[&]{
        m_isPasswordConfirm = true;
        m_widgetSystemSyncTimePassword->hide();
        systemToolSyncTime();

    });


    m_widgetSystemLaserCabPassword = new WidgetServoParamsPassword();
    connect(m_widgetSystemLaserCabPassword,&WidgetServoParamsPassword::signal_servoParamsPassword,this,[&]{
        m_isPasswordConfirm = true;
        m_widgetSystemLaserCabPassword->hide();
        m_pPluginLaserCalibrationProcess->start(QCoreApplication::applicationDirPath()+"/tool/LaserCalibration/PluginLaserCalibration.exe");
    });

    m_widgetServoParamsPassword = new WidgetServoParamsPassword();
    connect(m_widgetServoParamsPassword,&WidgetServoParamsPassword::signal_servoParamsPassword,this,[&]{
        m_isPasswordConfirm = true;
        m_widgetServoParamsPassword->hide();
        if(m_bIsStackedWidgetDisable)
        {

            m_messageWidget->setMessage("warn",tr("当前22000端口服务断开"));
            m_messageWidget->show();
            ui->widgetFirmwareUpgrade->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
            ui->btnFirmwareUpgrade->setChecked(true);
            return;

        }
        showServoParamWidget();

    });
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
            QString result = m_httpProtocol->postFaultRepair(m_currentIp,"dns");
            if(result =="success"){
                 m_firmwareCheckItemDns->setLabelStatus(2);
            }else{
                 m_firmwareCheckItemDns->setLabelStatus(-1);
            }
        }


       if(m_firmwareCheckItemMacAddress->theme().contains("异常")){
            m_firmwareCheckItemMacAddress->setLabelStatus(1);
           if(m_httpProtocol->postFaultRepair(m_currentIp,"mac",getCurrentUseAddress())=="success"){
                m_firmwareCheckItemMacAddress->setLabelStatus(2);
           }else{
                m_firmwareCheckItemMacAddress->setLabelStatus(-1);
           }
       }

    },Qt::UniqueConnection);
    initFirmwareCheckWidget();

    initSystemToolWidget();

    connect(ui->btnFileBackupStart,&QPushButton::clicked,this,[&]{
        if(ui->lineSelectedBackupDir->text().isEmpty()){
            qDebug()<<" 未选择相应的备份位置";
            return ;
        }
        SFtpOptFile opt;
        opt.id = FileBackupStart_OPT_LISTDIR;
        opt.strRemotePath = "/dobot/userdata/project";
        opt.strLocalPath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+g_strUpgradeParentVersion+"/"+g_strUpgradeVersion+"/files";
        opt.opt = SFtpOption::OPT_LISTDIR;
        m_csFtpClient->ssh(opt);
    });

    connect(ui->btnBakupSelectDir,&QPushButton::clicked,this,[&]{
       QString backupSelectDir = QFileDialog::getExistingDirectory(nullptr,QString(),
                                                            "/home",
                                                            QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks);
        if(!backupSelectDir.isEmpty()){
            ui->lineSelectedBackupDir->setText(backupSelectDir);

        }
        ui->lineSelectedBackupDir->repaint();
        return;
    });

    connect(ui->btnFileRevoverySelectDir,&QPushButton::clicked,this,[&]{
       QString fileName = QFileDialog::getOpenFileName(nullptr,QString(),
                                                            "/home",
                                                            tr("Rar (*.rar)"));
       if(!fileName.isEmpty()){
           ui->lineFileRecoveryDir->setText(fileName);

       }
       ui->lineFileRecoveryDir->repaint();
       return;
    });

    m_widget2FileRecoveryTip = new Widget2FileRecoveryTip();

    connect(ui->btnFileRecoveryStart,&QPushButton::clicked,this,[&]{
        m_widget2FileRecoveryTip->show();
    });
    connect(m_widget2FileRecoveryTip,&Widget2FileRecoveryTip::signalsFileRecoveryStart,this,[&]{
        DobotType::ControllerType controllerType = m_httpProtocol->getControllerType(m_currentIp);
        if(!ui->lineFileRecoveryDir->text().contains(CommonData::getCurrentSettingsVersion().control)){
            qDebug()<<"m_widget2FileRecoveryTip  控制柜版本不同";
            m_widget2FileRecoveryTip->setWidgetStatus(false,tr("控制器版本"));
            return;
        }
        if(!ui->lineFileRecoveryDir->text().contains(CommonData::getStrPropertiesCabinetType())){
            qDebug()<<"m_widget2FileRecoveryTip  控制柜型号不同";
            m_widget2FileRecoveryTip->setWidgetStatus(false,tr("控制柜型号"));
            return;
        }
        if(!ui->lineFileRecoveryDir->text().contains(controllerType.name.simplified())){
            qDebug()<<"m_widget2FileRecoveryTip  机器本体不同";
            m_widget2FileRecoveryTip->setWidgetStatus(false,tr("机器本体"));
            return;

        }
        m_widget2FileRecoveryTip->close();


        qDebug()<<"m_widget2FileRecoveryTip  ";

        QDir dir(QCoreApplication::applicationDirPath()+"/tool/fileRecovery/project");
        static int widget2FileRecoveryTipProcessValue = 0;
        m_widget2FileRecoveryProgressDialog->setHeadLabel("fileRecovery");
        m_widget2FileRecoveryProgressDialog->setTitle(tr("文件恢复"));
        m_widget2FileRecoveryProgressDialog->setWarnMessage(tr("提示：需重启控制柜完成文件恢复。"));
        m_widget2FileRecoveryProgressDialog->setProgressRange(200);
        ShadowWindow* s4 = new ShadowWindow(this);
        s4->setTopWidget(this);
        s4->installWidget(m_widget2FileRecoveryProgressDialog);
        qDebug()<< "dir.path() ==  "<<dir.path();
        if(dir.exists()){
            dir.removeRecursively();
        }
        m_widget2FileRecoveryProgressDialog->setProgressValue(++widget2FileRecoveryTipProcessValue);


        if(m_zipDiyManager->zipUnCompress(ui->lineFileRecoveryDir->text(),QApplication::applicationDirPath()+"/tool/fileRecovery/",m_zipLogExportPassword)){
            m_widget2FileRecoveryProgressDialog->setProgressValue(++widget2FileRecoveryTipProcessValue);
        }else{
            qDebug()<<"m_widget2FileRecoveryProgressDialog zipUnCompress  failed";
            m_widget2FileRecoveryProgressDialog->setHeadLabel("fail");
            return;
        }


        QProcess *pFileRecovery = new QProcess();

        connect(pFileRecovery,&QProcess::readyRead,this,[&]{
            m_widget2FileRecoveryProgressDialog->setProgressValue(widget2FileRecoveryTipProcessValue++);
        });

        connect(pFileRecovery,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
            qDebug()<<"exitCode "<<exitCode <<" exitStatus "<<exitStatus;
            //获取当前时间
            QDateTime current_time = QDateTime::currentDateTime();
            QString currentTime = current_time.toString("yyyyMMdd");
            if(exitStatus == QProcess::NormalExit){
                m_widget2FileRecoveryProgressDialog->setProgressValue(m_widget2FileRecoveryProgressDialog->progressRange());
                m_widget2FileRecoveryProgressDialog->setHeadLabel("success");
            }else{
                m_widget2FileRecoveryProgressDialog->setHeadLabel("fail");
            }

            widget2FileRecoveryTipProcessValue = 0;
            pFileRecovery->deleteLater();//new code
        });

        connect(pFileRecovery,&QProcess::errorOccurred,[&](QProcess::ProcessError error){
            qWarning()<<"slot_ errorOccurred!" <<error;
        });


        m_widget2FileRecoveryProgressDialog->show();
        pFileRecovery->start(QCoreApplication::applicationDirPath()+"/tool/fileRecovery/fileRecovery.bat",QStringList()<<m_currentIp);


    });

    ui->btnControlVersion->setDisabled(true);
    connect(ui->btnControlVersion,&QPushButton::clicked,this,[&]{
         m_messageWidget->setMessage("",m_httpProtocol->getProtocolVersion(m_currentIp));
         m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
         m_messageWidget->show();
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

    m_posDiyX = (QApplication::desktop()->geometry().width()-this->width())/2;

    m_posDiyY = QApplication::desktop()->geometry().height()/2-this->height()/2;

    ui->labelTopCabinetType->hide();
    setDiyPos();
    ui->btnFirmwareUpgrade->clicked(true);
    setStackedWidgetDisable(true,false);
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
    ui->labelCurrentControl->setDisabled(disable);
    ui->labelCurrentSystem->setDisabled(disable);
    ui->labelDeviceIcon->setDisabled(disable);
    ui->btnExpandMore->setDisabled(disable);
//    showCurrentAndUpgradeFirmware(ui->labelCurrentIOFirmwareTheme,ui->labelCurrentIOFirmwareVersion,ui->labelUpgradeIOFirmwareVersion,false);
}

void MainWidget2::searchMenuItems()
{
    m_controlMenu->clear();
    QHash<QString,QStringList> versions;
    QDir dir(QCoreApplication::applicationDirPath()+"/upgradeFiles");
    if(!dir.exists()){
        QMessageBox::warning(this,"waring","固件文件夹被删除！");
    }
    QFileInfoList fileInfoList = dir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList) {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;
        if (fileInfo.isDir()) {
            QDir sonDir(dir.filePath(fileInfo.fileName()));
            QStringList sonVersions;
            QFileInfoList sonFileInfoList = sonDir.entryInfoList();
            foreach(QFileInfo sonFileInfo, sonFileInfoList){
                if (sonFileInfo.fileName() == "." || sonFileInfo.fileName() == "..")
                    continue;
                if(sonFileInfo.isDir()){
                    sonVersions.append(sonFileInfo.fileName());
                }
            }
            versions.insert(fileInfo.fileName(),sonVersions);
        }
    }

    for(QString group:m_allVersionSetting.childGroups()){
        if(CommonData::getStrPropertiesCabinetType().contains(g_strPropertiesCabinetTypeCC26X)&&!group.contains("CRV2"))
        {
            continue;
        }

        if(!CommonData::getStrPropertiesCabinetType().contains(g_strPropertiesCabinetTypeCC26X)&&group.contains("CRV2"))
        {
            continue;
        }
        QMenu* currentBigVersion = m_controlMenu->addMenu(group);
        currentBigVersion->setWindowFlag(Qt::NoDropShadowWindowHint);
        if(versions.contains(group)){
            QStringList existVersions = versions.value(group);
            m_allVersionSetting.beginGroup(group);
            QStringList keyVersions = m_allVersionSetting.allKeys();
            m_allVersionSetting.endGroup();
            for(QString version:keyVersions){
                if(existVersions.contains(version)){
                    currentBigVersion->addAction(QIcon(":/image/images/icon_downloaded.png"),version);
                }else{
                    currentBigVersion->addAction(QIcon(":/image/images/icon_undownload.png"),version);
                }
            }
        }else{
            m_allVersionSetting.beginGroup(group);
            QStringList keyVersions = m_allVersionSetting.allKeys();
            m_allVersionSetting.endGroup();
            for(QString version:keyVersions){
                currentBigVersion->addAction(QIcon(":/image/images/icon_undownload.png"),version);
            }
        }


    }
    if(versions.contains("temporary")){
        QMenu* currentBigVersion = m_controlMenu->addMenu("temporary");
        currentBigVersion->setWindowFlag(Qt::NoDropShadowWindowHint);
        QStringList smallVersions = versions.value("temporary");
        for(QString version:smallVersions){
            if(CommonData::getStrPropertiesCabinetType().contains("CC26")&&!version.contains("CRV2"))
            {
                continue;
            }
            if(!CommonData::getStrPropertiesCabinetType().contains("CC26")&&version.contains("CRV2"))
            {
                continue;
            }
            currentBigVersion->addAction(QIcon(":/image/images/icon_downloaded.png"),version);
        }
    }


    ui->btnMenu->setMenu(m_controlMenu);
}

QString MainWidget2::readUpgradeDetail(QString fileParentDir,QString fileName)
{
    QString filePath;
    filePath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+fileParentDir+"/"+fileName+"/"+fileName+"detailChinese.txt";

    if(!g_isEnglish){
        filePath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+fileParentDir+"/"+fileName+"/"+fileName+"detailChinese.txt";
    }else{
        filePath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+fileParentDir+"/"+fileName+"/"+fileName+"detailEnglish.txt";
    }
    qDebug()<<"filePath  "<<filePath;
    QFile file(filePath);
    if(!file.exists()){
        filePath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+fileParentDir+"/"+fileName+"/"+fileName+"detailChinese.txt";
    }
    //指定为utf-8
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
          qDebug()<<"文件打开失败";
          return NULL;
      }
    //每次都去文件中的一行，然后输出读取到的字符串
    QString upgradeDetail;
    char * str = new char[1024];
    qint64 readNum = file.readLine(str,1024);
    //当读取出现错误（返回 -1）或者读取到的字符数为 0 时，结束读取
    while((readNum !=0) && (readNum != -1)){
        upgradeDetail.append(QString(codec->toUnicode(str)));
        qDebug() << str;
        readNum = file.readLine(str,1024);
    }
    file.close();
    delete[] str;
    return upgradeDetail;
}

void MainWidget2::slot_menuClicked(QAction *action)
{
    QMenu* menuParent = (QMenu*)action->parent();
    qDebug()<<"menuParent->title();  "<<menuParent->title();

    g_strUpgradeVersion = action->text();
    g_strUpgradeParentVersion = menuParent->title();
    m_upgradeVersion = action->text();
    m_upgradeParentVersion = menuParent->title();
    m_upgradeTime = m_allVersionSetting.value(m_upgradeParentVersion+"/"+m_upgradeVersion).toString();
    qDebug()<<"m_upgradeTime  "<<m_upgradeTime;

    ui->btnMenu->setText(action->text()+" "+m_upgradeTime);
    ui->btnMenu->setIcon(action->icon());
    ui->btnMenu->setIconSize(QSize(45,28));
    ui->btnMenu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QString upgradeDetail =readUpgradeDetail(m_upgradeParentVersion,m_upgradeVersion);
    ui->btnUpgrade->show();
    ui->btnExpandMore->show();
    ui->textUpgradeDetail->setWordWrapMode(QTextOption::WrapAnywhere);
    ui->textUpgradeDetail->show();
    ui->textUpgradeDetail->setText(upgradeDetail);

    ui->btnUpgrade->setDisabled(false);
    readUpgradeVersion(m_upgradeParentVersion,m_upgradeVersion);


    isCurrentVersionEnableUpgrade();



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
        g_strCurrentIP = ui->boxDevice->currentText();
        m_currentIp = ui->boxDevice->currentText();
        if(g_isRestartServer){
            g_isRestartServer = false;
            initPlink();
            SFtpConnectParam prm;
            prm.iPort = 22;
            prm.strPwd = "dobot";
            prm.strHost = m_currentIp;
            prm.strUserName = "root";
            m_csFtpClient->connectHost(prm);
            widget2StartServer();
//            sleep(6000);
        }



        //初始化单个升级功能里的所有固件是否可升级状态
        CommonData::setUpgrade2WidgetSafeIOFOEStatus(0);
        CommonData::setUpgrade2WidgetSafeIOXMLStatus(0);

        CommonData::setUpgrade2WidgetFeedbackStatus(0);

        CommonData::setUpgrade2WidgetUniIOFOEStatus(0);
        CommonData::setUpgrade2WidgetUniIOXMLStatus(0);

        CommonData::setUpgrade2WidgetCCBOXFOEStatus(0);
        CommonData::setUpgrade2WidgetCCBOXXMLStatus(0);

        CommonData::setUpgrade2WidgetServoJ1FOEStatus(0);
        CommonData::setUpgrade2WidgetServoJ1XMLStatus(0);

        CommonData::setUpgrade2WidgetServoJ2FOEStatus(0);
        CommonData::setUpgrade2WidgetServoJ2XMLStatus(0);

        CommonData::setUpgrade2WidgetServoJ3FOEStatus(0);
        CommonData::setUpgrade2WidgetServoJ3XMLStatus(0);

        CommonData::setUpgrade2WidgetServoJ4FOEStatus(0);
        CommonData::setUpgrade2WidgetServoJ4XMLStatus(0);

        CommonData::setUpgrade2WidgetServoJ5FOEStatus(0);
        CommonData::setUpgrade2WidgetServoJ5XMLStatus(0);

        CommonData::setUpgrade2WidgetServoJ6FOEStatus(0);
        CommonData::setUpgrade2WidgetServoJ6XMLStatus(0);


        CommonData::setUpgrade2WidgetTerminalFOEStatus(0);
        CommonData::setUpgrade2WidgetTerminalXMLStatus(0);

        CommonData::setUpgrade2WidgetSafeBFOEStatus(0);

        m_isUpgradeSuccessStatus = 0;
        ui->labelTopControlType->show();
        ui->stackedWidget->setDisabled(false);
        setStackedWidgetDisable(false,true);
        ui->widgetFuncSelection->setDisabled(false);
        ui->btnControlVersion->setDisabled(false);

        DobotType::SettingsVersion settingsVersion = m_httpProtocol->postSettingsVersion(m_currentIp,QString());
        PropertiesCabinetType propertiesCabinetType = m_httpProtocol->getPropertiesCabinetType(m_currentIp);
        CommonData::setStrPropertiesCabinetType(m_httpProtocol->getPropertiesCabinetTypeStr(m_currentIp));
        qDebug()<<"CommonData::getStrPropertiesCabinetType() " <<CommonData::getStrPropertiesCabinetType();
        if(settingsVersion.control.isEmpty()){
            settingsVersion = m_httpProtocol->getSettingsVersion(m_currentIp);
        }
        if(settingsVersion.control.isEmpty())
        {
            qDebug()<<"get SettingsVersion failed,  Please check the network cable plugging and unplugging ";
            m_searchDeviceAndIsOnlineTimer->start(2500);
            return;
        }
        CommonData::setCurrentSettingsVersion(settingsVersion);
        //TODO 要设置为false
        setbtnUpgradeServoParamAllWriteHidden(true);
        QString strControl = CommonData::getCurrentSettingsVersion().control;
        QString servo = CommonData::getCurrentSettingsVersion().servo1;
        if(strControl.at(0) == '3' && servo.at(0) == '4')
        {

            QStringList strV3List = strControl.split(".");
            int V1 = strV3List[0].toInt();
            int V2 = strV3List[1].toInt();
            int V3 = strV3List[2].toInt();
            int V4 = strV3List[3].toInt();
            int intVersion = V1*1000+V2*100+V3*10+V4;
            qDebug()<<"setbtnUpgradeServoParamAllWriteHidden intVersion "<<intVersion;

            QStringList strServoList = servo.split(".");
            int servo1 = strServoList[0].toInt();
            int servo2 = strServoList[1].toInt();
            int servo3 = strServoList[2].toInt();
            int servo4 = strServoList[3].toInt();
            int intServoVersion = servo1*1000+servo2*100+servo3*10+servo4;
            qDebug()<<"setbtnUpgradeServoParamAllWriteHidden intServoVersion "<<intServoVersion;

            if(intVersion >= 3560 && intServoVersion >= 4133)
            {
                setbtnUpgradeServoParamAllWriteHidden(false);
            }

        }

        if(propertiesCabinetType == PropertiesCabinetType::CCBOX){
            ui->labelTopCabinetType->setText(tr("小型控制柜")+"(CCBOX)");
            m_propertiesCabinetType = "小型控制柜(CCBOX)";
            qDebug()<< "CCBOXCCBOX ";
        }else if(propertiesCabinetType == PropertiesCabinetType::CC162){
            ui->labelTopCabinetType->setText(tr("标准控制柜")+"(CC162)");
            m_propertiesCabinetType = "标准控制柜(CC162)";
            qDebug()<< "CC162CC162 ";


            ui->labelCurrentSafeIOTheme->setText(tr("安全IO固件："));
        }else if(propertiesCabinetType == PropertiesCabinetType::CC262){
            ui->labelTopCabinetType->setText(tr("标准控制柜")+"(CC262)");
            m_propertiesCabinetType = "标准控制柜(CC262)";
            ui->labelCurrentSafeIOTheme->setText(tr("安全IO板A："));
            qDebug()<< "CC262CC262 ";
        }else{
            m_propertiesCabinetType = "标准控制柜";
            ui->labelTopCabinetType->setText(tr("标准控制柜"));
            ui->labelCurrentSafeIOTheme->setText(tr("安全IO固件："));
            qDebug()<< "not get PropertiesCabinetType";
        }
        cabinetTypeInfoShow(propertiesCabinetType);

        DobotType::ControllerType controllerType = m_httpProtocol->getControllerType(m_currentIp);
        CommonData::setControllerType(controllerType);
        ui->labelTopControlType->setText(CommonData::getControllerType().name+"-"+CommonData::getStrPropertiesCabinetType());

        m_widgetSingleUpgrade->m_currentServo = settingsVersion.servo1;

        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());

        ui->labelCurrentControl->setText(settingsVersion.control);


        setLabelToolTip(ui->labelCurrentMainControlVersion,CommonData::getCurrentSettingsVersion().control);
        if(settingsVersion.control.at(1)=='4'){
            m_currentControlVersionSmbDir = "developOnly";
        }else if(settingsVersion.control.at(1)=='3'){
            m_currentControlVersionSmbDir = "project";
        }
        ui->labelCurrentSystem->setText(tr("当前系统版本：")+settingsVersion.system.simplified());

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


        searchMenuItems();

        ui->btnConnect->setStyleSheet("QPushButton { \n  border: none;\n  border-radius: 4px;"
                                        "\n  background-color: rgba(255,255,255,20);\n  color: rgb(255, 255, 255);\n}QPushButton:disabled{ /* all types of tool button */\n  border: none;\n  border-radius: 4px;\n  background-color: #C7C7C7;\n  color: rgb(255, 255, 255);\n}\n");


        sleep(1000);



        m_searchDeviceAndIsOnlineTimer->start(9000);

        initFileBackupWidget();

        widgetSingleFunc();




        qDebug()<<" ---------------******** ";
        QString reply;
        for(int i = 0;i < 3; i++)
        {
            reply = m_httpProtocol->getProtocolVersion(g_strCurrentIP);
            if(!reply.isEmpty())
            {
                break;
            }
        }



        if(reply.isEmpty())
        {
            g_isRestartServer = true;
            m_messageWidget->setMessage("warn",tr("连接控制器服务失败，请断开重连"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
            setStackedWidgetDisable(true,false);
        }
        else
        {
             searchDeviceDisable(false);
        }

        ui->btnConnect->setText(tr("断开连接"));
        ui->btnConnect->style()->unpolish(ui->btnConnect); //清除旧的样式

        ui->btnConnect->setProperty("isConnected",2);
        ui->btnConnect->style()->polish(ui->btnConnect);   //更新为新的样式
        ui->btnConnect->repaint();
        qDebug()<<" ---------------******** ";


    }else{

        ui->stackedWidget->setDisabled(false);
        setStackedWidgetDisable(true,true);
//        initFirmwareCheckWidgetStatus();
        ui->labelTopControlType->hide();
        ui->labelTopCabinetType->hide();
        ui->btnDownload->show();
        ui->btnImport->show();
        ui->btnMenu->show();
        ui->btnUpgrade->show();
        textClear();
        searchDeviceDisable(true);
        ui->btnConnect->setDisabled(false);
        ui->labelDeviceIcon->setDisabled(false);
        QFont font;
        font.setUnderline(true);
        ui->btnControlVersion->setDisabled(true);
        ui->btnConnect->setText(tr("连接"));
        ui->btnConnect->setProperty("isConnected",-1);
        ui->btnConnect->setStyleSheet("QPushButton { \n  border: none;\n  border-radius: 4px;"
                                        "\n  background-color: #30CF58;\n  color: rgb(255, 255, 255);\n}QPushButton:disabled{ /* all types of tool button */\n  border: none;\n  border-radius: 4px;\n  background-color: #C7C7C7;\n  color: rgb(255, 255, 255);\n}\n");
        m_searchDeviceAndIsOnlineTimer->start(2500);
        initFileBackupWidget(true);
        initFileRecoveryWidget();
    }

}

void MainWidget2::slot_searchDevice()
{
    int connectCount = 0;
    bool currentIPExist = true;
    for(QString ip : m_ipList)
    {

//        qDebug()<<" ip****** "<<ip;
        int exist = false;
        if(m_httpProtocol->getConnectionState(ip) != ConnectState::unconnected){
            ui->btnDownload->show();
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
            if(ip == m_currentIp){
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

    if(connectCount == 0){
        ui->labelTopCabinetType->hide();
        ui->labelTopControlType->hide();

        textClear();

        qDebug()<<"  connectCount == 0  ======";


        if(!m_httpProtocol->getProtocolVersion(g_strCurrentIP).isEmpty())
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
        ui->btnControlVersion->setDisabled(true);
        g_isRestartServer = true;
        initFileRecoveryWidget();
        initFileBackupWidget(true);
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

        m_searchDeviceAndIsOnlineTimer->start(2500);
    }

//    if(connectCount != 0 && !currentIPExist){
//        if(!m_httpProtocol->getProtocolVersion(g_strCurrentIP).isEmpty())
//        {
//            setStackedWidgetDisable(true,true);
//        }
////        ui->widgetFuncSelection->setDisabled(true);
////        ui->stackedWidget->setDisabled(true);
//        ui->btnDownload->show();
//        ui->btnImport->show();
//        ui->btnMenu->show();
//        ui->btnUpgrade->show();
//        textClear();
//        searchDeviceDisable(true);
//        ui->btnConnect->hide();
//        ui->btnConnect->setDisabled(false);
//        ui->labelDeviceIcon->setDisabled(false);
//        QFont font;
//        font.setUnderline(true);
//        ui->btnConnect->setProperty("isConnected",-1);
//        ui->btnConnect->setText(tr("连接"));
//        ui->btnControlVersion->setDisabled(true);
//        g_isRestartServer = true;
//        initFileBackupWidget(true);
//        initFileRecoveryWidget();
////        initFirmwareCheckWidgetStatus();
//        ui->btnConnect->setStyleSheet("QPushButton { \n  border: none;\n  border-radius: 4px;"
//                                        "\n  background-color: #30CF58;\n  color: rgb(255, 255, 255);\n}QPushButton:disabled{ /* all types of tool button */\n  border: none;\n  border-radius: 4px;\n  background-color: #C7C7C7;\n  color: rgb(255, 255, 255);\n}\n");


//        if(!ui->btnFirmwareUpgrade->isChecked())
//        {
//             ui->btnFirmwareUpgrade->clicked(true);
//        }
//        m_searchDeviceAndIsOnlineTimer->start(2500);
//    }

}

void MainWidget2::slot_openManuallyIPWidget()
{
    m_manualIPShadowWindow->setTopWidget(this);
    m_manualIpWidget->initIP();
    m_manualIPShadowWindow->installWidget(m_manualIpWidget);
    m_manualIpWidget->show();
}

void MainWidget2::slot_saveIpsFromManual(QStringList ipList)
{
    m_searchDeviceAndIsOnlineTimer->stop();
    m_ipList.clear();
    m_ipList.append("192.168.5.1");
    m_ipList.append("192.168.1.6");
    m_ipList.append("192.168.9.1");
    m_ipList.append(ipList);
    sleep(500);
    m_searchDeviceAndIsOnlineTimer->start(2500);
}

void MainWidget2::slot_download()
{
    qDebug()<< " slot_download **** ";
    if(ui->btnMenu->text().isEmpty()){
        m_messageWidget->setMessage("warn",tr("请先选择控制器版本"));
        m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
//        m_messageWidget->setGeometry(this->x()+400,this->y()+300,m_messageWidget->width(),m_messageWidget->height());
        m_messageWidget->show();
        return;
    }
    m_diyProgressDialog->move(this->pos().x()+width()/2-m_diyProgressDialog->width()/2,this->pos().y()+height()/2-m_diyProgressDialog->height()/2);
    m_diyProgressDialog->setHeadLabel("downloading...");
    m_diyProgressDialog->setTitle(tr("下载中"));
    if(!m_upgrade2Widget->isVisible()){
        m_diyProgressDialog->show();
    }
    QString upgradeDir = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion;


    qDebug()<<"m_download QCoreApplication::applicationDirPath()"<<QCoreApplication::applicationDirPath();
    m_upgradeFileZipDownload->startDownload("http://cdn.release.dobot.cc/dobotUpgradeToolFile/"+m_upgradeParentVersion+"/"+m_upgradeVersion+".zip",QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion);

}

void MainWidget2::slot_downloadProgress(qint64 bytesRead, qint64 totalBytes, qreal progress)
{
    qDebug()<<" bytesRead "<<bytesRead<<" totalBytes "<<totalBytes<<endl;
    m_diyProgressDialog->setProgressRange(totalBytes);
    m_diyProgressDialog->setProgressValue(bytesRead);
}

void MainWidget2::slot_downloadFinished(bool ok)
{
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

            QString upgradeDetail =readUpgradeDetail(m_upgradeParentVersion,m_upgradeVersion);

            ui->btnUpgrade->show();
            ui->btnExpandMore->show();
            ui->textUpgradeDetail->show();
            ui->textUpgradeDetail->setText(upgradeDetail);
            readUpgradeVersion(m_upgradeParentVersion,m_upgradeVersion);

            ui->btnMenu->setIcon(QIcon(":/image/images/icon_downloaded.png"));

            if(m_upgrade2Widget->isVisible()){
                widget2UpgradeFirmwareDowloadFinish(true);

            }

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
    }

}

void MainWidget2::slot_importTempVersion()
{
    QString srcDir = QFileDialog::getExistingDirectory(nullptr,QString(),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    m_diyProgressDialog->setGeometry(this->x()+250,this->y()+200,m_diyProgressDialog->width(),m_diyProgressDialog->height());

    if(srcDir.isEmpty()){
        return;
    }
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
    m_messageWidget->close();
    qApp->quit();
}

void MainWidget2::slot_miniWindow()
{
    this->showMinimized();
}
#include <windows.h>
void MainWidget2::slot_maxWindow(bool ok)
{

//    qDebug()<<"ok  "<<ok;
    if(ok){
//        ::ShowWindow((HWND)winId(),SW_MAXIMIZE);
//        setWindowState((windowState() |Qt::WindowMaximized) | Qt::WindowActive);
        this->showMaximized();
        ui->btnMax->setStyleSheet("QPushButton{border-image: url(:/image/images2/icon_main_max_after.png);color: gray;}");
    }else{
        this->showNormal();
        ui->btnMax->setStyleSheet("image: url(:/image/images2/icon_main_max.png);border:none;");
    }

}

void MainWidget2::slot_expandMore(bool checked)
{
    qDebug()<<"ui->textUpgradeDetail->height() "<<ui->textUpgradeDetail->height();
    qDebug()<<"ui->scrollAreaWidgetContents->height() "<<ui->scrollAreaWidgetContents->height();
    int expandHeight = 130;
    qDebug()<<"checked "<<checked;
    if(checked){
        ui->textUpgradeDetail->setFixedHeight(ui->textUpgradeDetail->height()+expandHeight);
        ui->scrollAreaWidgetContents->setFixedHeight(ui->scrollAreaWidgetContents->height()+expandHeight);
        ui->btnExpandMore->setGeometry(ui->btnExpandMore->x(),ui->btnExpandMore->y()+expandHeight,ui->btnExpandMore->width(),ui->btnExpandMore->height());
        ui->btnExpandMore->setText(tr("收回"));
        ui->btnExpandMore->setProperty("isExpanded",true);
    }else{
        ui->textUpgradeDetail->setFixedHeight(ui->textUpgradeDetail->height()-expandHeight);
        ui->scrollAreaWidgetContents->setFixedHeight(ui->scrollAreaWidgetContents->height()-expandHeight);
        ui->btnExpandMore->setGeometry(ui->btnExpandMore->x(),ui->btnExpandMore->y()-expandHeight,ui->btnExpandMore->width(),ui->btnExpandMore->height());
        ui->btnExpandMore->setText(tr("展开更多"));
        ui->btnExpandMore->setProperty("isExpanded",false);
    }
}

void MainWidget2::slot_onPanelBoxChanged(bool checked)
{

    QObject* btn = sender();
    if(btn == ui->btnServoParams){

        ui->btnServoParams->setChecked(false);
        ui->widgetFuncSelection->repaint();
        if(m_isPasswordConfirm)
        {
            if(m_bIsStackedWidgetDisable)
            {
                m_messageWidget->setMessage("warn",tr("当前22000端口服务断开"));
                m_messageWidget->show();
                ui->widgetFirmwareUpgrade->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
                ui->btnFirmwareUpgrade->setChecked(true);
                return;
            }
            showServoParamWidget();
        }
        else
        {
            m_widgetServoParamsPassword->move(this->pos().x()+width()/2-m_widgetServoParamsPassword->width()/2,this->pos().y()+height()/2-m_widgetServoParamsPassword->height()/2);
            m_widgetServoParamsPassword->initStatus();
            m_widgetServoParamsPassword->show();
        }
        return;
    }

    ui->widgetBackup->setStyleSheet("border:none;");
    ui->widgetFirmwareUpgrade->setStyleSheet("border:none;");
    ui->widgetSystemTool->setStyleSheet("border:none;");
    ui->widgetFirmwareCheck->setStyleSheet("border:none;");
    ui->widgetFileRecovery->setStyleSheet("border:none;");
    ui->widgetServoParams->setStyleSheet("border:none;");

    ui->btnFirmwareUpgrade->setChecked(false);
    ui->btnSystemTool->setChecked(false);
    ui->btnFileRecovery->setChecked(false);
    ui->btnFirmwareCheck->setChecked(false);
    ui->btnBackup->setChecked(false);
    ui->btnServoParams->setChecked(false);

    if (btn == ui->btnFirmwareUpgrade){

        ui->widgetFirmwareUpgrade->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
        ui->stackedWidget->setCurrentIndex(0);
        ui->btnFirmwareUpgrade->setChecked(true);

    }
    if(btn == ui->btnFirmwareCheck){
        if(m_bIsStackedWidgetDisable)
        {

            m_messageWidget->setMessage("warn",tr("当前22000端口服务断开"));
            m_messageWidget->show();
            ui->widgetFirmwareUpgrade->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
            ui->btnFirmwareUpgrade->setChecked(true);
            return;

        }
        ui->widgetFirmwareCheck->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
        ui->stackedWidget->setCurrentIndex(1);
        ui->btnFirmwareCheck->setChecked(true);
    }
    if(btn == ui->btnFileRecovery){
        if(m_bIsStackedWidgetDisable)
        {

            m_messageWidget->setMessage("warn",tr("当前22000端口服务断开"));
            m_messageWidget->show();
            ui->widgetFirmwareUpgrade->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
            ui->btnFirmwareUpgrade->setChecked(true);
            return;

        }
        ui->widgetFileRecovery->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
        ui->stackedWidget->setCurrentIndex(4);
        ui->btnFileRecovery->setChecked(true);
    }
    if(btn == ui->btnSystemTool){
        if(m_bIsStackedWidgetDisable)
        {
            m_messageWidget->setMessage("warn",tr("当前22000端口服务断开"));
            m_messageWidget->show();
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

            m_messageWidget->setMessage("warn",tr("当前22000端口服务断开"));
            m_messageWidget->show();
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
    if (!m_httpProtocol->getIsOnline()) {
        //网络未连接，发送信号通知
        qDebug() << " fail ????????????";
        isOnline = false;
        g_isOnline = false;
        ui->btnUpgrade->setText(tr("升级"));
    }else{
        isOnline = true;
        g_isOnline = true;
        ui->btnUpgrade->setText(tr("下载并升级"));
    }

}

void MainWidget2::slot_btnLanguageChange(bool isZh)
{

    qDebug()<<"isZh : "<<isZh;
    QSettings setting("config/language.ini",QSettings::IniFormat);
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

void MainWidget2::slot_getHttpExchange()
{
     if(m_httpProtocol->getConnectionState(m_currentIp) != ConnectState::unconnected){
        DobotType::SettingsVersion settingVersion;
        sleep(10000);
        settingVersion = m_httpProtocol->postSettingsVersion(m_currentIp,QString());

        if(settingVersion.control.isEmpty()){
            settingVersion = m_httpProtocol->getSettingsVersion(m_currentIp);
        }

        qDebug()<<" settingVersion ";
        settingVersion.toString();
        qDebug()<<" lastVersion ";
        CommonData::getUpgradeSettingsVersion().toString();
        if(CommonData::getUpgradeSettingsVersion().control.isEmpty()||settingVersion.control.isEmpty()){
            return;
        }
        //todo 去除大小写问题。
        if(!settingVersion.unio.simplified().contains(CommonData::getUpgradeSettingsVersion().unio.simplified(),Qt::CaseInsensitive)||
                !settingVersion.control.simplified().contains(CommonData::getUpgradeSettingsVersion().control.simplified(),Qt::CaseInsensitive)||
                !settingVersion.servo1.simplified().contains(CommonData::getUpgradeSettingsVersion().servo1.simplified(),Qt::CaseInsensitive)||
                !settingVersion.servo2.simplified().contains(CommonData::getUpgradeSettingsVersion().servo2.simplified(),Qt::CaseInsensitive)||
                !settingVersion.servo3.simplified().contains(CommonData::getUpgradeSettingsVersion().servo3.simplified(),Qt::CaseInsensitive)||
                !settingVersion.servo4.simplified().contains(CommonData::getUpgradeSettingsVersion().servo4.simplified(),Qt::CaseInsensitive)||
                !settingVersion.servo5.simplified().contains(CommonData::getUpgradeSettingsVersion().servo5.simplified(),Qt::CaseInsensitive)||
                !settingVersion.servo6.simplified().contains(CommonData::getUpgradeSettingsVersion().servo6.simplified(),Qt::CaseInsensitive)||
                !settingVersion.terminal.simplified().contains(CommonData::getUpgradeSettingsVersion().terminal.simplified(),Qt::CaseInsensitive)){
            m_controlAutoConnect->setUpgadeStatus(-1);
        }else {
            m_controlAutoConnect->setUpgadeStatus(2);
        }

        m_isUpgradeSuccessTimer->stop();
     }
}





void MainWidget2::slot_upgradeWidgetUpgradeSingle2Count(int seqId,int status)
{
    m_upgradeProgressTimer->stop();
    sleep(5000);
    qDebug()<<"slot_upgradeWidgetUpgradeSingle2Count upgradeAllFirmwareAndXMLs";
    upgradeAllFirmwareAndXMLs();

//    m_upgradeProgressTimer->stop();
//    DobotType::StructCRSingleFwAndXml structCRSingleFwAndXml;
////    if(status == -2)
////    {
////        structCRSingleFwAndXml.cabType = "XML";
////    }
////    if(status == -1)
////    {
////        structCRSingleFwAndXml.cabType = "firmware";
////    }
////    if(CommonData::getStrPropertiesCabinetType().contains(g_strPropertiesCabinetTypeCC26X))
////    {
////        structCRSingleFwAndXml.cabVersion = "V2";
////    }
////    else
////    {
////        structCRSingleFwAndXml.cabVersion = "V1";
////    }
////    if(CommonData::getStrPropertiesCabinetType().contains(g_strPropertiesCabinetTypeCCBOX))
////    {
////        structCRSingleFwAndXml.cabType = "small";
////    }
////    else
////    {
////        structCRSingleFwAndXml.cabType = "big";
////    }
////    structCRSingleFwAndXml.operationType = "start";
////    //TODO
////    structCRSingleFwAndXml.seqId = seqId;
//    structCRSingleFwAndXml.seqId = seqId;
//    QString result = m_httpProtocol->postUpdateCRSingleFwAndXml(g_strCurrentIP,structCRSingleFwAndXml);
//    if(!result.isEmpty()||result == g_strSuccess)
//    {
//        sleep(2000);
//    }
//    else
//    {
//        qDebug()<<"slot_upgradeWidgetUpgradeSingle2Count postUpdateCRSingleFwAndXml  failed";
//    }
//    m_upgradeProgressTimer->start(1000);
//    QEventLoop event;
//    while(true)
//    {
//        int status = m_httpProtocol->getUpdateSingleStatus(g_strCurrentIP);
//        if(status == -1)
//        {
//            if(m_httpProtocol->postUpdateCRSingleFwAndXml(g_strCurrentIP,structCRSingleFwAndXml) == g_strSuccess)
//            {

//            }
//            else
//            {

//            }
//        }
//        else
//        {

//        }
//    }

    //    event.exec();
}

int MainWidget2::setSingleFuncUpgradeStatus(int status)
{
    if(status == 2)
    {
        m_messageWidget->setMessage("warn",tr("该板卡已升级成功，请选择升级失败的板卡。"));
        m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
        m_messageWidget->show();
    }
    return status;
}

void MainWidget2::slot_upgradeProgress()
{
    DobotType::UpdateStatus upgradeStatus = m_httpProtocol->getUpdateCRStatus(m_currentIp);
//    qDebug()<<" upgradeStatus.FWUpdateResult.size() "<<upgradeStatus.FWUpdateResult.size();
    QList<int> intUpgradeStatusList;

    intUpgradeStatusList = upgradeStatus.FWUpdateResult;
    qDebug()<<"upgradeStatus.bIsOutage : "<<upgradeStatus.bIsOutage;
    if(upgradeStatus.bIsOutage)
    {

        m_upgrade2Widget->m_bIsOutage = upgradeStatus.bIsOutage;
        qDebug()<<"m_upgrade2Widget->m_bIsOutage "<<m_upgrade2Widget->m_bIsOutage;
        m_upgrade2Widget->setUpgradeStatus(intUpgradeStatusList);
        return;
    }

    CommonData::setUpgrade2WidgetSafeIOFOEStatus(upgradeStatus.FWUpdateResult[0]);
    CommonData::setUpgrade2WidgetSafeIOXMLStatus(upgradeStatus.XMLUpdateResult[0]);

    CommonData::setUpgrade2WidgetFeedbackStatus(upgradeStatus.FWUpdateResult[1]);

    CommonData::setUpgrade2WidgetUniIOFOEStatus(upgradeStatus.FWUpdateResult[2]);
    CommonData::setUpgrade2WidgetUniIOXMLStatus(upgradeStatus.XMLUpdateResult[1]);

    CommonData::setUpgrade2WidgetCCBOXFOEStatus(upgradeStatus.FWUpdateResult[2]);
    CommonData::setUpgrade2WidgetCCBOXXMLStatus(upgradeStatus.XMLUpdateResult[1]);

    CommonData::setUpgrade2WidgetServoJ1FOEStatus(upgradeStatus.FWUpdateResult[3]);
    CommonData::setUpgrade2WidgetServoJ1XMLStatus(upgradeStatus.XMLUpdateResult[2]);

    CommonData::setUpgrade2WidgetServoJ2FOEStatus(upgradeStatus.FWUpdateResult[4]);
    CommonData::setUpgrade2WidgetServoJ2XMLStatus(upgradeStatus.XMLUpdateResult[3]);

    CommonData::setUpgrade2WidgetServoJ3FOEStatus(upgradeStatus.FWUpdateResult[5]);
    CommonData::setUpgrade2WidgetServoJ3XMLStatus(upgradeStatus.XMLUpdateResult[4]);

    CommonData::setUpgrade2WidgetServoJ4FOEStatus(upgradeStatus.FWUpdateResult[6]);
    CommonData::setUpgrade2WidgetServoJ4XMLStatus(upgradeStatus.XMLUpdateResult[5]);

    CommonData::setUpgrade2WidgetServoJ5FOEStatus(upgradeStatus.FWUpdateResult[7]);
    CommonData::setUpgrade2WidgetServoJ5XMLStatus(upgradeStatus.XMLUpdateResult[6]);

    CommonData::setUpgrade2WidgetServoJ6FOEStatus(upgradeStatus.FWUpdateResult[8]);
    CommonData::setUpgrade2WidgetServoJ6XMLStatus(upgradeStatus.XMLUpdateResult[7]);


    CommonData::setUpgrade2WidgetTerminalFOEStatus(upgradeStatus.FWUpdateResult[9]);
    CommonData::setUpgrade2WidgetTerminalXMLStatus(upgradeStatus.XMLUpdateResult[8]);

    CommonData::setUpgrade2WidgetSafeBFOEStatus(upgradeStatus.FWUpdateResult[10]);

    for(int i = 0, xmlIndex = 0;i < upgradeStatus.FWUpdateResult.size()-1;i++){
        if(i == 1){
            i=i+1;
            xmlIndex = 1;
        }
        intUpgradeStatusList[i] = upgradeFwAndXmlStatusCheck(upgradeStatus.FWUpdateResult[i],upgradeStatus.XMLUpdateResult[i-xmlIndex]);
    }
    intUpgradeStatusList[upgradeStatus.FWUpdateResult.size()-1] = upgradeStatus.FWUpdateResult[upgradeStatus.FWUpdateResult.size()-1];
    qDebug()<<"intUpgradeStatusList[upgradeStatus.FWUpdateResult.size()-1] "<<intUpgradeStatusList[upgradeStatus.FWUpdateResult.size()-1];
    m_upgrade2Widget->setUpgradeStatus(intUpgradeStatusList);
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
    DowloadTipWidget* downloadTipWidget = new DowloadTipWidget();
    ShadowWindow* s_message = new ShadowWindow(this);
    s_message->setTopWidget(this);
    s_message->installWidget(downloadTipWidget);
    if(m_currentSoftwareVersion<downloadedSoftwareVersion.toInt()){
        QString url = softwareSetting.value("software/url").toString();
        if(!url.isEmpty()){
            downloadTipWidget->setMessage(url);
        }
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
    searchMenuItems();
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

    if(m_httpProtocol->postFaultCheck(m_currentIp,"dns")=="success"){
        m_firmwareCheckItemDns->setThemeStatus(2);
    }else{
        m_firmwareCheckItemDns->setThemeStatus(-1);
        m_firmwareCheckItemDns->setLabelStatus(10);
        ui->btnAllRepair->show();
    }

    m_firmwareCheckItemMacAddress->setLabelStatus(0);
    m_firmwareCheckItemMacAddress->setThemeStatus(1);

    if(m_httpProtocol->postFaultCheck(m_currentIp,"mac")=="success"){
        m_firmwareCheckItemMacAddress->setThemeStatus(2);
    }else{
        m_firmwareCheckItemMacAddress->setThemeStatus(-1);
        m_firmwareCheckItemMacAddress->setLabelStatus(10);
        ui->btnAllRepair->show();
    }

    ui->btnRestartDiagnose->show();
    m_firmwareCheckItem0->setMainThemeStatus(2);
}

void MainWidget2::slot_cSFtpClientFinishedJob(qint64 id, bool bOk, QString strErrMsg)
{
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
}

void MainWidget2::slot_singleFunc()
{
    qDebug()<<"m_isUpgradeSuccessStatus  "<<m_isUpgradeSuccessStatus;
    if(m_isUpgradeSuccessStatus == 2)
    {
        m_messageWidget->setMessage("warn",tr("如需使用高级功能，请重启后操作。"));
        m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
        m_messageWidget->show();
        return;
    }

    m_widgetSingleUpgrade->searchMenuItems();
    ui->stackedWidget->setCurrentIndex(5);
    widgetSingleFunc();
}

void MainWidget2::slot_backToUpgrade()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWidget2::slot_upgradeSingleFwOrXML(QString updateFilePath, QString updateFile,QString updateFilePath2,QString updateFile2)
{

    m_widgetSingleUpgrading->setSingleUpgradeStatus(1);
    m_widgetSingleUpgrading->show();
    QStringList projectList;
    projectList.append(updateFilePath);
    if(!updateFilePath2.isEmpty())
    {
        projectList.append(updateFilePath2);
    }
    QFile file(QCoreApplication::applicationDirPath()+"/tool/fileSingleTransfer/fileSingleTransfer.txt");
    if(file.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Truncate)){
        QString temp; //写入内容
        for(QString projectDir:projectList){
            temp += projectDir+"\n";
        }

        qDebug()<<" fileTransfer.txt........ "<<temp;
        // 将内容写入文件
        QTextStream out(&file);
        out << temp;
        file.close();
        qDebug()<<"fileTransfer.txt completed.";
    }else{
        qDebug()<<"failed to create a new file!";
    }
    m_updateFile1 = updateFile;
    m_updateFile2 = updateFile2;
    m_pFileSingleTransfer->start(QCoreApplication::applicationDirPath()+"/tool/fileSingleTransfer/fileSingleTransfer.bat",QStringList()<<m_currentIp);


}

void MainWidget2::slot_csFtpClientOnChannelFileInfoAvailableFinish(qint64 job, const QList<QSsh::SftpFileInfo> &fileInfoList)
{
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
    if(job == SystemLogExport_OPT_LISTDIR){
        QStringList projectList;
        for(QSsh::SftpFileInfo sftpFileInfo : fileInfoList){
            if(sftpFileInfo.name=="."||sftpFileInfo.name==".."){
                continue;
            }
            if(sftpFileInfo.type == QSsh::SftpFileType::FileTypeDirectory){
                projectList.append(sftpFileInfo.name);
            }
        }
        m_widgetSystemLogExport->setProjectList(projectList);
        m_widgetSystemLogExport->repaint();
        m_widgetSystemLogExport->move(this->pos().x()+width()/2-m_widgetSystemLogExport->width()/2,this->pos().y()+height()/2-m_widgetSystemLogExport->height()/2);
        m_widgetSystemLogExport->show();
    }

    if(job == FileBackupStart_OPT_LISTDIR){
        QStringList projectList;
        for(QSsh::SftpFileInfo sftpFileInfo : fileInfoList){
            if(sftpFileInfo.name=="."||sftpFileInfo.name==".."||sftpFileInfo.name =="logs"){
                continue;
            }
            projectList.append(sftpFileInfo.name);
        }

        m_widget2FileBackupProgressDialog->setTitle(tr("文件备份"));
        m_widget2FileBackupProgressDialog->setHeadLabel("backup");
        m_widget2FileBackupProgressDialog->setProgressRange(300);
        QFile file(QCoreApplication::applicationDirPath()+"/tool/FileBackup/projectBackupFilesDir.txt");
        if(file.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Truncate)){
            QString temp; //写入内容
            for(QString projectDir:projectList){
                temp += projectDir+"\n";
            }
            qDebug()<<" projectBackupFilesDir........ "<<temp;
            // 将内容写入文件
            QTextStream out(&file);
            out << temp;
            file.close();
            qDebug()<<"completed.";
        }else{
            qDebug()<<"failed to create a new file!";
        }

        QProcess* pFileBackup = new QProcess();


        static int fileBackupProgressValue = 1;
        connect(pFileBackup,&QProcess::readyRead,this,[&]{
            m_widget2FileBackupProgressDialog->setProgressValue(fileBackupProgressValue++);

        });

        connect(pFileBackup,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
            qDebug()<<"exitCode "<<exitCode <<" exitStatus "<<exitStatus;
            //获取当前时间
            QDateTime current_time = QDateTime::currentDateTime();
            QString currentTime = current_time.toString("yyyyMMdd");

            //机器型号_控制柜型号_控制器版本_导出日期时间_Backup
            bool compress = m_zipDiyManager->zipDiyCompress(QCoreApplication::applicationDirPath()+"/tool/fileBackup/backup/project",ui->lineSelectedBackupDir->text().simplified()+"/"+CommonData::getControllerType().name+"_"+CommonData::getStrPropertiesCabinetType()+"_"+CommonData::getCurrentSettingsVersion().control+"_"+currentTime+"_Backup.rar",m_zipLogExportPassword);
            if(compress){
                m_widget2FileBackupProgressDialog->setProgressValue(m_widget2FileBackupProgressDialog->progressRange());
                m_widget2FileBackupProgressDialog->setHeadLabel("success");
            }else{
                m_widget2FileBackupProgressDialog->setHeadLabel("fail");
            }

            fileBackupProgressValue = 0;
            pFileBackup->deleteLater();//new Code
        });

        connect(pFileBackup,&QProcess::errorOccurred,[&](QProcess::ProcessError error){
            qWarning()<<"slot_ errorOccurred!" <<error;
        });
        ShadowWindow* s6 = new ShadowWindow(this);
        s6->setTopWidget(this);
        s6->installWidget(m_widget2FileBackupProgressDialog);
        m_widget2FileBackupProgressDialog->show();
        pFileBackup->start(QCoreApplication::applicationDirPath()+"/tool/fileBackup/backup.bat",QStringList()<<m_currentIp);
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
    ui->widgetFileRecovery->setStyleSheet("border:none;");
    ui->widgetServoParams->setStyleSheet("border:none;");

    ui->btnFirmwareUpgrade->setChecked(false);
    ui->btnSystemTool->setChecked(false);
    ui->btnFileRecovery->setChecked(false);
    ui->btnFirmwareCheck->setChecked(false);
    ui->btnBackup->setChecked(false);
    ui->btnServoParams->setChecked(false);

    ui->widgetServoParams->setStyleSheet("background-color: rgb(250, 253, 255);\nborder-radius: 8px 8px 8px 8px;");
//        if(!ui->btnBackup->isChecked()&&!ui->btnFirmwareUpgrade->isChecked()&&!ui->btnFileRecovery->isChecked()&&!ui->btnFirmwareCheck->isChecked()){
//            ui->btnBackup->setChecked(!checked);
//            return;
//        }

    ui->btnServoParams->setChecked(true);
    ui->widgetFuncSelection->repaint();
}



QString MainWidget2::getCurrentUseAddress()
{
    QString group = m_macAddressSetting.childGroups()[0];
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

QString MainWidget2::paramIndexToAddress(QString paramIndex)
{
    QStringList addressList = paramIndex.split(".");
    QString address0 = addressList.at(0);
    QString address1 = addressList.at(1);
    int intSubAddress = address1.toInt() + 1;
    if(intSubAddress <10){
        address1 = QString("0%1").arg(intSubAddress);
    }else{
        address1 = QString("%1").arg(intSubAddress);
    }
    QString mainIndex = address0.mid(1,2);
    QString completeAddress ="20"+mainIndex+address1;
    completeAddress = completeAddress.toUpper();
    qDebug()<<" completeAddress.toUpper "<<completeAddress;
    //由于get 的时候 返回的是 小写字母  需转换成 大写字母
    return completeAddress;
}

QString MainWidget2::paramIndexToJAddress(QString ParaIndStr)
{

    QStringList addressList = ParaIndStr.split(".");
    QString address0 = addressList.at(0);
    QString address1 = addressList.at(1);
    int intSubAddress = address1.toInt() + 1;
    if(intSubAddress <10){
        address1 = QString("0%1").arg(intSubAddress);
    }else{
        address1 = QString("%1").arg(intSubAddress);
    }
    QString mainIndex = address0.mid(1);
    QString completeAddress ="200"+mainIndex+address1;
    completeAddress = completeAddress.toUpper();
    qDebug()<<" completeAddress.toUpper "<<completeAddress;
    //由于get 的时候 返回的是 小写字母  需转换成 大写字母
    return completeAddress;
}

void MainWidget2::setbtnUpgradeServoParamAllWriteHidden(bool isHidden)
{
    if(isHidden)
    {
        ui->btnUpgradeWidgetServoParamAllWrite->hide();
        ui->labelUpgradeWidgetServoParamAllWrite->hide();
    }
    else
    {
        ui->btnUpgradeWidgetServoParamAllWrite->show();
        ui->labelUpgradeWidgetServoParamAllWrite->show();
    }
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
//        qDebug()<<widget;

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
        if(isTrue)
        {
            for(int row = 0;row < mServoParamsTableModel->rowCount();row++)
            {
                QStandardItem* item = mServoParamsTableModel->item(row,colIsCheck);
                if(item == nullptr)
                {
                    qDebug()<<"m_singleReadDelegate item->text().isEmpty() ";
                    continue;
                }
                item->setText(g_strServoParamsColValueTrue);

            }

        }
        else
        {
            for(int row = 0;row < mServoParamsTableModel->rowCount();row++)
            {
                QStandardItem* item = mServoParamsTableModel->item(row,colIsCheck);
                if(item == nullptr)
                {
                    qDebug()<<"m_singleReadDelegate item->text().isEmpty() ";
                    continue;
                }
                item->setText(g_strServoParamsColValueFalse);

            }

        }
    });
    ui->tableServoParamView->setHorizontalHeader(m_myCheckBoxHeader);

    //单独写入
    m_singleWriteDelegate = new SingleWriteDelegate();
    connect(m_singleWriteDelegate,&SingleWriteDelegate::startSingleWrite, this, [&](int row){
        QStandardItem* item = mServoParamsTableModel->item(row,colIsReadOnly);
        if(item == nullptr || item->text() == g_strServoParamsColValueTrue){
            qDebug()<<"m_singleWriteDelegate only read  ";
            m_messageWidget->setMessage("warn",tr("该列只允许读，不允许写"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
            return;
        }

        QString paramIndex = item->text();

        QStandardItem* addressItem = mServoParamsTableModel->item(row,colParaIndex);
        QString address = paramIndexToAddress(addressItem->text());
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
        bool result = m_httpProtocol->postSettingsModifyServoParams(m_currentIp,settingsServoParams);
        if(result == true)
        {
            sleep(300);
            DobotType::StructSettingsServoParamsResult servoParamsResult = m_httpProtocol->getSettingsModifyServoParams(m_currentIp);
            if(servoParamsResult.servoParams[0].status != true)
            {
                qDebug()<<"m_singleWriteDelegate  getSettingsModifyServoParams failed ";
                m_messageWidget->setMessage("warn",tr("写入失败"));
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
            }
            else
            {
                qDebug()<<"m_singleWriteDelegate  getSettingsModifyServoParams success ";
                m_messageWidget->setMessage("",tr("写入成功"));
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
            }
        }
        else
        {
            qDebug()<<"m_singleWriteDelegate  postSettingsModifyServoParams success ";
            m_messageWidget->setMessage("warn",tr("写入失败"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
        }



    },Qt::QueuedConnection);
    //单独读取
    m_singleReadDelegate = new SingleReadDelegate();
    connect(m_singleReadDelegate,&SingleReadDelegate::startSingleRead, this, [&](int row){
        m_messageWidget->setMessage("doing",tr("读取中"));
        m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
        m_messageWidget->show();
        QStandardItem* item = mServoParamsTableModel->item(row,colParaIndex);
        if(item == nullptr || item->text().isEmpty())
        {
            qDebug()<<"m_singleReadDelegate item->text().isEmpty() ";
            return;
        }
        QString paramIndex = item->text();
        qDebug()<<"m_singleReadDelegate paramIndex  "<<paramIndex;
        QString address = paramIndexToAddress(paramIndex);
        DobotType::StructSettingsServoParams settingsServoParams;
        DobotType::StructServoParam servoParam;
        servoParam.servoNum = ui->comboServo->currentText();
        servoParam.key = "addr"+address;
        settingsServoParams.servoParams.append(servoParam);

        bool isSuccess = m_httpProtocol->postSettingsReadServoParams(m_currentIp,settingsServoParams);
        if(!isSuccess)
        {
            m_messageWidget->setMessage("warn",tr("读取单个失败"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
        }else
        {
            sleep(300);
            DobotType::StructSettingsServoParamsResult servoParamsResult = m_httpProtocol->getSettingsReadServoParams(m_currentIp);
            if(servoParamsResult.status == true)
            {
                m_messageWidget->setMessage("",tr("读取成功"));\
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
                if(servoParamsResult.servoParams[0].status == true)
                {
                    double value = servoParamsResult.servoParams[0].value;
                    qDebug()<<"m_singleReadDelegate value  "<<value;
                    QModelIndex dotDividedBitsIndex = mServoParamsTableModel->index(row, colDotDividedBits, QModelIndex());
                    int dotDividedBits = mServoParamsTableModel->data(dotDividedBitsIndex).toInt();
                    double viewValue = dotDividedBitsValue(true,dotDividedBits,value);

                    QModelIndex index = mServoParamsTableModel->index(row, colValue, QModelIndex());
                    mServoParamsTableModel->setData(index,viewValue, Qt::EditRole);
                }
                else{
                    qDebug()<<" m_singleReadDelegate servoParamsResult.servoParams[0].status == false ";
                    m_messageWidget->setMessage("warn",tr("读取单个失败"));
                    m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                    m_messageWidget->show();
                }
            }
            else
            {
                qDebug()<<" getSettingsReadServoParams Failed ";
                m_messageWidget->setMessage("warn",tr("读取失败"));
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
            }
        }


    },Qt::QueuedConnection);


}

void MainWidget2::importServoParamsExcel(QString filePath)
{
    if(filePath.isEmpty()){
        return;
    }

    QFile inFile(filePath);
    QStringList lines;/*行数据*/

    int rowCount = 0;

    if (inFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream_text(&inFile);
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
                ui->tableServoParamView->repaint();
                return;
            }
            else
            {
                m_messageWidget->setMessage("warn",tr("请导入正确的表格"));
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
                return;
            }
            qDebug()<<" ";

        }

        for (int col = 0; col < headDataList.size(); col++)
        {
            mServoParamsTableModel->setHeaderData(col+2, Qt::Horizontal, headDataList.at(col), Qt::EditRole);
        }

        for (int row = 2, modelRow = 0; row < lines.size(); row++,modelRow++)
        {
            QString line = lines.at(row);
            QStringList split = line.split(",");/*列数据*/


            if(split[0].contains(g_strServoParamsColValueTrue)||split[0].contains(g_strServoParamsColValueFalse))
            {

            }
            else{
                modelRow--;
                continue;
            }
//            if(split[colIsReadOnly-2]=="TRUE")
//            {
//                modelRow--;
//                continue;
//            }
            for (int col = 2; col < split.size()+2; col++)
            {



                QString value = split.at(col-2);
                if(col == colValue)
                {
                    value = split.at(col-2).split(":")[0];
                }
                QModelIndex index = mServoParamsTableModel->index(modelRow, col, QModelIndex());
                mServoParamsTableModel->setData(index, value, Qt::EditRole);
            }
            m_servoParamsCount = modelRow+1;
        }
        inFile.close();

    }
    qDebug()<<" m_servoParamsCount "<<m_servoParamsCount;
    mServoParamsTableModel->setRowCount(m_servoParamsCount);
    for (int row = 2; row < rowCount; row++) {
        QModelIndex indexWirte = mServoParamsTableModel->index(row, colWirte, QModelIndex());
        mServoParamsTableModel->setData(indexWirte, QStringLiteral(COLUMN_SINGLEWRITE), Qt::EditRole);
        ui->tableServoParamView->setItemDelegateForColumn(colWirte, m_singleWriteDelegate);

        QModelIndex indexRead = mServoParamsTableModel->index(row, colRead, QModelIndex());
        mServoParamsTableModel->setData(indexRead, QStringLiteral(COLUMN_SINGLEREAD), Qt::EditRole);
        ui->tableServoParamView->setItemDelegateForColumn(colRead, m_singleReadDelegate);


    }
    for(int row = 1; row < rowCount; row++ )
    {
        ui->tableServoParamView->setItemDelegateForColumn(colIsCheck, m_singleIsCheckDelegate);
    }


    ui->tableServoParamView->setColumnWidth(0, 60);
    ui->tableServoParamView->setColumnWidth(1, 60);
    ui->tableServoParamView->setColumnWidth(2, 100);
    ui->tableServoParamView->setColumnWidth(4, 80);
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

    if(CommonData::getStrPropertiesCabinetType().contains("CCBOX")){
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


//        ui->labelSingleServoJ2->show();
//        ui->textSingleServoJ2->show();
//        ui->textSingleServoJ2XML->show();
//        ui->btnSingleServoJ2->show();
//        ui->btnSingleServoJ2XML->show();

//        ui->labelSingleServoJ3->show();
//        ui->textSingleServoJ3->show();
//        ui->textSingleServoJ3XML->show();
//        ui->btnSingleServoJ3->show();
//        ui->btnSingleServoJ3XML->show();
//        ui->labelSingleServoJ4->show();
//        ui->textSingleServoJ4->show();
//        ui->textSingleServoJ4XML->show();
//        ui->btnSingleServoJ4->show();
//        ui->btnSingleServoJ4XML->show();

//        ui->labelSingleServoJ5->show();
//        ui->textSingleServoJ5->show();
//        ui->textSingleServoJ5XML->show();
//        ui->btnSingleServoJ5->show();
//        ui->btnSingleServoJ5XML->show();

//        ui->labelSingleServoJ6->show();
//        ui->textSingleServoJ6->show();
//        ui->textSingleServoJ6XML->show();
//        ui->btnSingleServoJ6->show();
//        ui->btnSingleServoJ6XML->show();

    }else if(CommonData::getStrPropertiesCabinetType().contains("CC")){
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

//        ui->labelSingleServoJ2->show();
//        ui->textSingleServoJ2->show();
//        ui->textSingleServoJ2XML->show();
//        ui->btnSingleServoJ2->show();
//        ui->btnSingleServoJ2XML->show();

//        ui->labelSingleServoJ3->show();
//        ui->textSingleServoJ3->show();
//        ui->textSingleServoJ3XML->show();
//        ui->btnSingleServoJ3->show();
//        ui->btnSingleServoJ3XML->show();

//        ui->labelSingleServoJ4->show();
//        ui->textSingleServoJ4->show();
//        ui->textSingleServoJ4XML->show();
//        ui->btnSingleServoJ4->show();
//        ui->btnSingleServoJ4XML->show();

//        ui->labelSingleServoJ5->show();
//        ui->textSingleServoJ5->show();
//        ui->textSingleServoJ5XML->show();
//        ui->btnSingleServoJ5->show();
//        ui->btnSingleServoJ5XML->show();

//        ui->labelSingleServoJ6->show();
//        ui->textSingleServoJ6->show();
//        ui->textSingleServoJ6XML->show();
//        ui->btnSingleServoJ6->show();
//        ui->btnSingleServoJ6XML->show();

    }else if(CommonData::getStrPropertiesCabinetType().contains("MG400")){

    }else if(CommonData::getStrPropertiesCabinetType().contains("M1Pro")){

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
    }

    setHideServo6JWidget(true);
}

void MainWidget2::widget2UpgradeDevice()
{
    m_upgrade2Widget->initStatus();
    m_isInterruptUpgrade = false;
    DobotType::ProtocolExchangeResult exchange = m_httpProtocol->getProtocolExchange(m_currentIp);
    if(exchange.prjState != "stopped"){
        m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
        m_messageWidget->setMessage("warn",tr("当前设备处于工程运行状态或报警\n不适合升级"));
        m_messageWidget->show();
        return;
    }

    m_mainControlProcess = new QProcess();


    m_upgrade2Widget->setCabinetType();




    m_upgrade2WidgetShawdowPng->move(this->pos().x()+width()/2-m_upgrade2WidgetShawdowPng->width()/2,this->pos().y()+height()/2-m_upgrade2WidgetShawdowPng->height()/2);
    m_upgrade2WidgetShawdowPng->show();

    qDebug()<<"m_upgrade2Widget "<<m_upgrade2Widget->x() <<"  "<<m_upgrade2Widget->y() ;

    QString strUpgradeDir = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion;
    QDir upgradeDir(strUpgradeDir);
    qDebug()<<"strUpgradeDir  "<<strUpgradeDir;
    if(!upgradeDir.exists()){
       if(isOnline==false){
//           ShadowWindow* s1 = new ShadowWindow(this);
//           s1->setTopWidget(this);
//           s1->installWidget(m_tipWidget);
           m_tipWidget->show();
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
    }else{
        m_diyProgressDialog->setTitle(tr("导入成功"));
        qDebug()<<"m_diyProgressDialog->progressRange()  "<<m_diyProgressDialog->progressRange();;
        if(m_diyProgressDialog->progressRange()==0)
        {
            m_diyProgressDialog->setProgressValue(100);
        }
        m_diyProgressDialog->setHeadLabel("success!");
        searchMenuItems();


        g_strUpgradeParentVersion = "temporary";
        m_upgradeParentVersion = "temporary";
        m_upgradeVersion = m_strImportDirName;
        QString upgradeDetail =readUpgradeDetail(m_upgradeParentVersion,m_upgradeVersion);


        ui->btnUpgrade->show();
        ui->btnExpandMore->show();
        ui->textUpgradeDetail->show();
        ui->textUpgradeDetail->setText(upgradeDetail);
        ui->btnMenu->setText(m_strImportDirName);
        ui->btnMenu->setIcon(QIcon(":/image/images/icon_downloaded.png"));
        ui->btnMenu->setIconSize(QSize(45,28));
        ui->btnMenu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        readUpgradeVersion(m_upgradeParentVersion,m_upgradeVersion);

    }

      isCurrentVersionEnableUpgrade();

}

void MainWidget2::slot_upgradeDevice()
{
    m_isControlBack = false;



    QString strV3Control = CommonData::getCurrentSettingsVersion().control.split("-")[0];
    if(strV3Control.at(0) == 'V' || strV3Control.at(0) == 'v')
    {
        strV3Control = strV3Control.mid(1);
    }

    if(strV3Control.at(0) == '3')
    {
        QStringList strV3List = strV3Control.split(".");
        int V1 = strV3List[0].toInt();
        int V2 = strV3List[1].toInt();
        int V3 = strV3List[2].toInt();
        int V4 = strV3List[3].toInt();
        int intVersion = V1*1000+V2*100+V3*10+V4;
        qDebug()<<"slot_upgradeDevice intVersion "<<intVersion;
        if(intVersion < 3540)
        {
            if(CommonData::getUpgradeSettingsVersion().control.contains("V4"))
            {
                m_messageWidget->setMessage("warn",tr("V3.5.4.0以下不允许升级V4"));
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
                return;
            }
        }

    }

    if(strV3Control.at(0) == '3')
    {
        QStringList strV3List = strV3Control.split(".");
        int V1 = strV3List[0].toInt();
        int V2 = strV3List[1].toInt();
        int V3 = strV3List[2].toInt();
        int V4 = strV3List[3].toInt();
        int intVersion = V1*1000+V2*100+V3*10+V4;
        qDebug()<<"slot_upgradeDevice intVersion V3.5.2.4以下不允许升级 "<<intVersion;
        if(intVersion < 3524)
        {
            m_messageWidget->setMessage("warn",tr("V3.5.2.4以下不允许升级"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
            return;
        }
    }


    QString strUpgradeControl;
    if(g_strUpgradeParentVersion == "temporary")
    {
        if(CommonData::getUpgradeSettingsVersion().control.isEmpty()||CommonData::getUpgradeSettingsVersion().control.size()<7)
        {
            m_messageWidget->setMessage("warn",tr("当前升级版本有问题"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
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
        if(strUpgradeControl.at(0) == '3')
        {
            QStringList strV3List = strUpgradeControl.split(".");
            if(strV3List.size()<4)
            {
                m_messageWidget->setMessage("warn",tr("当前升级版本有问题"));
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
                return;
            }
            int V1 = strV3List[0].toInt();
            int V2 = strV3List[1].toInt();
            int V3 = strV3List[2].toInt();
            int V4 = strV3List[3].toInt();
            int intVersion = V1*1000+V2*100+V3*10+V4;
            qDebug()<<"slot_upgradeDevice intVersion 升级版本只能是3.5.4.0及以上 "<<intVersion;
            if(intVersion < 3540)
            {
                m_messageWidget->setMessage("warn",tr("升级版本只能是3.5.4.0及以上"));
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
                return;
            }
        }
    }



    if(CommonData::getStrPropertiesCabinetType().contains(g_strPropertiesCabinetTypeCC26X)&&
            CommonData::getUpgradeSettingsVersion().control.contains("V3"))
    {
        m_messageWidget->setMessage("warn",tr("二代控制柜不允许回退V3"));
        m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
        m_messageWidget->show();
        return;
    }


    if(CommonData::getCurrentSettingsVersion().servo1.contains("3.1"))
    {

        m_messageWidget->setMessage("warn",tr("该控制柜版本不支持一键升级请联系技术支持"));
        m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
        m_messageWidget->show();
        qDebug()<<"伺服为3.1版本，不允许升级";
        return;
    }
    qDebug()<<" CommonData::getSettingsVersion().servo1 "<<CommonData::getCurrentSettingsVersion().servo1;
    qDebug()<<" CommonData::getSettingsVersion().servo1.at(0) "<<CommonData::getCurrentSettingsVersion().servo1.at(0);
    qDebug()<<" CommonData::getSettingsVersion().servo1.at(2) "<<CommonData::getCurrentSettingsVersion().servo1.at(2);
    qDebug()<<"CommonData::getSettingsVersion().servo1.mid(6,1).toInt()  "<<CommonData::getCurrentSettingsVersion().servo1.mid(6,1).toInt();
    if(CommonData::getCurrentSettingsVersion().servo1.contains("3.2.D", Qt::CaseInsensitive))
    {
        qDebug()<<"CommonData::getSettingsVersion().servo1.mid(6,1).toInt()  "<<CommonData::getCurrentSettingsVersion().servo1.mid(6,1).toInt();
        if(CommonData::getCurrentSettingsVersion().servo1.mid(6,1).toInt() >= 4)
        {
            if(CommonData::getUpgradeSettingsVersion().control.contains("V3.5.2"))
            {
                qDebug()<<"V3.2.D.4以上 不允许回退 V3.5.2";
                m_messageWidget->setMessage("warn",tr("伺服固件V3.2.D.4及以上\n不允许回退352版本固件"));
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
                return;
            }
        }
    }




    QString strV4Servo = CommonData::getCurrentSettingsVersion().servo1;
    qDebug()<<"strV4Servo  "<<strV4Servo;
    if(strV4Servo.at(0) == '4')
    {
        QStringList strV4List = strV4Servo.split(".");
        int V1 = strV4List[0].toInt();
        int V2 = strV4List[1].toInt();
        int V3 = strV4List[2].toInt();
        int V4 =  0;
        qDebug()<<"strV4List.count()  "<< strV4List.count();
        if(strV4List.count()>=4)
        {
            V4 = strV4List[3].toInt();
        }
        int intVersion = V1*1000+V2*100+V3*10+V4;
        qDebug()<<"slot_upgradeDevice servo1 intVersion "<<intVersion;
        if(intVersion >= 4102)
        {
            if(CommonData::getUpgradeSettingsVersion().control.contains("V3.5.2"))
            {
                qDebug()<<"伺服 4.1.0.2以上 不允许回退 V3.5.2";
                m_messageWidget->setMessage("warn",tr("伺服固件V4.1.0.2及以上\n不允许回退352版本固件"));
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
                return;
            }
        }


    }

    if(strV3Control.at(0) == '3')
    {
        QStringList strV3List = strV3Control.split(".");
        int V1 = strV3List[0].toInt();
        int V2 = strV3List[1].toInt();
        int V3 = strV3List[2].toInt();
        int V4 = strV3List[3].toInt();
        int intVersion = V1*1000+V2*100+V3*10+V4;
        qDebug()<<"slot_upgradeDevice intVersion V3.5.3.0  如果当前机器带电子皮肤，若升级至3.5.3及以上版本，电子皮肤会失效 "<<intVersion;
        if(intVersion < 3530)
        {
            m_upgradeIsOldElectronicSkinWidget->move(this->pos().x()+width()/2-m_upgradeIsOldElectronicSkinWidget->width()/2,this->pos().y()+height()/2-m_upgradeIsOldElectronicSkinWidget->height()/2);
            m_upgradeIsOldElectronicSkinWidget->show();
            return;
        }
    }

    if(!CommonData::getStrPropertiesCabinetType().contains(g_strPropertiesCabinetTypeCC26X))
    {

        if(CommonData::getUpgradeSettingsVersion().control.at(0) =='4')
        {
            qDebug()<<"一代柜不允许升级V4";
            m_messageWidget->setMessage("warn",tr("一代柜不允许升级V4"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
            return;
        }
    }
//    if(CommonData::getCurrentSettingsVersion().control.contains("V3")&&CommonData::getUpgradeSettingsVersion().control.contains("V4"))
//    {
//        m_widgetV4SN->setControl(CommonData::getCurrentSettingsVersion().control);
//        m_widgetV4SN->show();
//        return;
//    }

//    if(CommonData::getCurrentSettingsVersion().control.at(0) =='3'&&CommonData::getUpgradeSettingsVersion().control.contains("V4"))
//    {
//        m_widgetV4SN->setControl(CommonData::getCurrentSettingsVersion().control);
//        m_widgetV4SN->show();
//        return;
//    }

    if(CommonData::getStrPropertiesCabinetType().contains(g_strPropertiesCabinetTypeCC26X))
    {
        qDebug()<<"当前不支持二代柜升级";
        m_messageWidget->setMessage("warn",tr("当前不支持二代柜升级"));
        m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
        m_messageWidget->show();
        return;
    }


    if(CommonData::getCurrentSettingsVersion().control.contains("V4")||CommonData::getCurrentSettingsVersion().control.at(0) == '4' )
    {

        if(CommonData::getUpgradeSettingsVersion().control.contains("V3")||CommonData::getUpgradeSettingsVersion().control.at(0) =='3')
        {
            m_messageWidget->setMessage("warn",tr("当前不支持V4回退V3"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
            return;
        }

    }

    QString postUpdateCheckPartitionOccupancyResult = m_httpProtocol->getUpdateCheckPartitionOccupancy(g_strCurrentIP);
    if(postUpdateCheckPartitionOccupancyResult.isEmpty()||postUpdateCheckPartitionOccupancyResult != "idle")
    {

        m_isControlLogBusyWidget->move(this->pos().x()+width()/2-m_isControlLogBusyWidget->width()/2,this->pos().y()+height()/2-m_isControlLogBusyWidget->height()/2);
        m_isControlLogBusyWidget->show();
        return;
    }





    widget2UpgradeDevice();



}
bool MainWidget2::isUpgrade(QString currentVersion,QString upgradeVersion){
    if(currentVersion.isEmpty()||upgradeVersion.isEmpty()){
        return false;
    }
    QStringList currentVersions = currentVersion.split(".");
    QStringList upgradeVersions = upgradeVersion.split(".");
    for(int i =0; i<currentVersions.length()&&i<upgradeVersions.length();i++){
        if(upgradeVersions.at(i).toInt()>currentVersions.at(i).toInt()){
            return true;
        }else{
            qDebug()<< "upgradeVersions.at(i) "<<upgradeVersions.at(i) <<"currentVersions.at(i) "<<currentVersions.at(i);
        }
    }
    return false;
}

void MainWidget2::textClear()
{
    ui->labelCurrentControl->clear();
    ui->labelCurrentSystem->clear();
    ui->textUpgradeDetail->clear();

    ui->btnUpgrade->hide();
    ui->btnExpandMore->hide();
    ui->textUpgradeDetail->hide();

    ui->btnMenu->setText(QString());
    ui->btnMenu->setIcon(QIcon());

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


void MainWidget2::cabinetTypeInfoShow(PropertiesCabinetType propertiesCabinetType)
{
    if(propertiesCabinetType == PropertiesCabinetType::CCBOX){
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
    }else{
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

    ui->pageUpgrade->setDisabled(false);

    if(isDisabled)
    {
        ui->btnMenu->setDisabled(true);
        ui->pageFileBackup->setDisabled(true);
        ui->pageFileRecovery->setDisabled(true);
        ui->pageSystemTool->setDisabled(true);
        ui->pageServoParams->setDisabled(true);
        ui->btnUpgradeWidgetServoParamAllWrite->setDisabled(true);
        m_bIsStackedWidgetDisable = true;
    }
    else
    {
        ui->btnMenu->setDisabled(false);
        ui->pageFileBackup->setDisabled(false);
        ui->pageFileRecovery->setDisabled(false);
        ui->pageSystemTool->setDisabled(false);
        ui->pageServoParams->setDisabled(false);
        ui->btnUpgradeWidgetServoParamAllWrite->setDisabled(false);
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
    m_widgetSystemToolTip->move(this->pos().x()+width()/2-m_widgetSystemToolTip->width()/2,this->pos().y()+height()/2-m_widgetSystemToolTip->height()/2);
    m_widgetSystemToolTip->show();
    if(m_httpProtocol->postFaultRepair(m_currentIp,"synTime","", date,time)!="success")
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
    QProcess* p = new QProcess();
    QString* allResult = new QString();
    QEventLoop eventLoop;
    QObject::connect(p,&QProcess::errorOccurred,[&](QProcess::ProcessError error){
        qWarning()<<" widget2StartServer slot_ errorOccurred!" <<error;
        p->terminate();
        p->kill();

    });
//    QObject::connect(p,&QProcess::readyRead,[=](){
//        allResult->append(p->readAll());
//        qDebug()<< "widget2StartServer  readyRead"<<allResult;
//    });

    QObject::connect(p,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[&](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<" widget2StartServer finished! " <<exitStatus;
        QString cmdResult = p->readAll();
        qDebug()<< "cmdResult "<<cmdResult;
        delete allResult;
        eventLoop.quit();
        p->terminate();
        p->kill();
        p->deleteLater();

    });
    qDebug()<<QCoreApplication::applicationDirPath()+"/tool/startServer/startToolServer.exe  "<<m_currentIp;
    p->start(QCoreApplication::applicationDirPath()+"/tool/startServer/startToolServer.exe",QStringList()<<m_currentIp);
    eventLoop.exec();
}

void MainWidget2::widget2StartServerFinish(bool ok)
{

//    //判断当前是否是Nova设备
//    DobotType::ControllerType controllerType = m_httpProtocol->getControllerType(m_currentIp);
    QString strNova = "RX";
//    bool isNova = false;
//    if(controllerType.name.contains("Nova")){
//        isNova = true;
//    }

    QStringList projectList;
    QDir dir(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/files");
    if(!dir.exists())
    {
        qDebug()<<"固件升级文件夹不存在 ";
    }
    for(QFileInfo fileInfo:dir.entryInfoList())
    {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..") continue;
//        if(isNova){
//            if(fileInfo.fileName().contains("Servo")){
//                if(fileInfo.fileName().contains(strNova)){
//                    projectList.append(fileInfo.fileName());
//                }
//            }else if(fileInfo.fileName().contains("TerminalIO")){
//                if(fileInfo.fileName().contains(strNova)){
//                    projectList.append(fileInfo.fileName());
//                }
//            }else{
//                projectList.append(fileInfo.fileName());
//            }
//        }else{
         if(fileInfo.fileName().contains("Servo")){
             if(fileInfo.fileName().contains("XML"))
             {
                 qDebug()<<" fileInfo.fileName() Servo  XML"<<fileInfo.fileName();

                 if(fileInfo.fileName().contains("V2")&&CommonData::getCurrentSettingsVersion().servo1.at(0)=='6')
                 {
                    projectList.append(fileInfo.fileName());
                 }

                 if(fileInfo.fileName().contains("3.0.0")&&CommonData::getCurrentSettingsVersion().servo1.at(0)!='6')
                 {
                    projectList.append(fileInfo.fileName());
                 }

             }
             else
             {

                 if(ui->labelCurrentJ1Version->text().at(0)=='4'){
                    if(fileInfo.fileName().contains("V4")){
                       projectList.append(fileInfo.fileName());
                    }
                 }
                 if(ui->labelCurrentJ1Version->text().at(0)=='3'){
                    if(fileInfo.fileName().contains("V3")){
                        projectList.append(fileInfo.fileName());
                    }
                 }
                 if(fileInfo.fileName().contains(strNova)&&CommonData::getCurrentSettingsVersion().servo1.at(0)=='6')
                 {
                    projectList.append(fileInfo.fileName());
                 }
             }

         }
         else if(fileInfo.fileName().contains("TerminalIO")){
             if(fileInfo.fileName().contains(strNova)&&CommonData::getCurrentSettingsVersion().terminal.at(0)=='6')
             {
                qDebug()<<"TerminalIO  fileInfo.fileName().contains(V6)&&CommonData::getSettingsVersion().terminal.at(0)=='6'";
                projectList.append(fileInfo.fileName());
             }
             if(!fileInfo.fileName().contains(strNova)&&CommonData::getCurrentSettingsVersion().terminal.at(0)!='6')
             {
                qDebug()<<"TerminalIO  !fileInfo.fileName().contains(V6)&&CommonData::getSettingsVersion().terminal.at(0)!='6'";
                projectList.append(fileInfo.fileName());
             }
         }
         else {
             projectList.append(fileInfo.fileName());
         }

//        }

    }

    qDebug()<<" m_pFileTransfer  send ++++++++++++++++++++++++++++++ ";
    QFile file(QCoreApplication::applicationDirPath()+"/tool/fileTransfer/fileTransfer.txt");
    if(file.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Truncate)){
        QString temp; //写入内容
        for(QString projectDir:projectList){
            temp += projectDir+"\n";
        }

        qDebug()<<" fileTransfer.txt........ "<<temp;
        // 将内容写入文件
        QTextStream out(&file);
        out << temp;
        file.close();
        qDebug()<<"fileTransfer.txt completed.";
    }else{
        qDebug()<<"failed to create a new file!";
    }

    m_pFileTransfer->start(QCoreApplication::applicationDirPath()+"/tool/fileTransfer/fileTransfer.bat",QStringList()<<m_currentIp<<m_upgradeParentVersion+"/"+m_upgradeVersion);



}

void MainWidget2::widget2UpdateDiskKernel()
{
    QProcess* p = new QProcess();
    QObject::connect(p,&QProcess::errorOccurred,[&](QProcess::ProcessError error){
        qWarning()<<"slot_ errorOccurred!" <<error;
    });

    QObject::connect(p,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<"finished! " <<exitStatus;
        QString cmdResult = p->readAll();
        qDebug()<< "升级内核 cmdResult "<<cmdResult;
        if(cmdResult.contains("update kernel success!")){
            m_upgrade2Widget->setDiskUpdateStatus(2,tr("升级内核成功"));
            Widget2MainControlUpgrade();
        }else{
            m_upgrade2Widget->setDiskUpdateStatus(-1,tr("升级内核失败"));
        }
        p->deleteLater();
    });
    qDebug()<< "升级内核开始 ";
    p->start(QCoreApplication::applicationDirPath()+"/tool/System_update_0804/system_update_5.1.bat",QStringList()<<m_currentIp);
    m_upgrade2Widget->setDiskUpdateStatus(1,tr("升级内核中"));

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
    p->start(QCoreApplication::applicationDirPath()+"/tool/System_update_0804/system_update_5.1.bat",QStringList()<<m_currentIp);
    m_upgrade2Widget->setDiskUpdateStatus(1,tr("现在升级内核修复该问题"));
}

void MainWidget2::wigdet2UpdateCRSingleFwAndXML(QString updateFile1,QString updateFile2)
{

    QString strCRSingleFwAndXML = m_strLabelSingleFwAndXML;
    qDebug()<<" m_strLabelSingleFwAndXML "<<m_strLabelSingleFwAndXML;
    DobotType::StructCRSingleFwAndXml singleCRFwAndXml;
    if(m_propertiesCabinetType.contains("标准控制柜"))
    {
        singleCRFwAndXml.cabType = "big";
    }
    else
    {
        singleCRFwAndXml.cabType = "small";
    }
    if(strCRSingleFwAndXML.contains("SafeIO")||strCRSingleFwAndXML.contains("FeedBack"))
    {
        singleCRFwAndXml.slaveId = 1;
    }
    if(strCRSingleFwAndXML.contains("CCBOX"))
    {
        singleCRFwAndXml.slaveId = 1;
    }
    if(strCRSingleFwAndXML.contains("UniIO"))
    {
        singleCRFwAndXml.slaveId = 2;
    }

    if(strCRSingleFwAndXML.contains("Terminal"))
    {

        singleCRFwAndXml.slaveId = 8;
        if(m_propertiesCabinetType.contains("标准控制"))
        {
            singleCRFwAndXml.slaveId++;
        }

    }

    if(strCRSingleFwAndXML.contains("Servo"))
    {

        if(strCRSingleFwAndXML.contains("J1"))
        {
            singleCRFwAndXml.slaveId = 2;
        }
        if(strCRSingleFwAndXML.contains("J2"))
        {
            singleCRFwAndXml.slaveId = 3;
        }
        if(strCRSingleFwAndXML.contains("J3"))
        {
            singleCRFwAndXml.slaveId = 4;
        }
        if(strCRSingleFwAndXML.contains("J4"))
        {
            singleCRFwAndXml.slaveId = 5;
        }
        if(strCRSingleFwAndXML.contains("J5"))
        {
            singleCRFwAndXml.slaveId = 6;
        }
        if(strCRSingleFwAndXML.contains("J6"))
        {
            singleCRFwAndXml.slaveId = 7;
        }

        if(strCRSingleFwAndXML.contains("JALL"))
        {
            singleCRFwAndXml.slaveId = 2;
        }


        if(m_propertiesCabinetType.contains("标准"))
        {
            singleCRFwAndXml.slaveId++;

        }


        qDebug()<<"singleCRFwAndXml.slaveId  "<<singleCRFwAndXml.slaveId;

    }


    singleCRFwAndXml.operationType = "start";

    if(CommonData::getStrPropertiesCabinetType()==g_strPropertiesCabinetTypeCC162||CommonData::getStrPropertiesCabinetType()==g_strPropertiesCabinetTypeCCBOX)
    {
        singleCRFwAndXml.cabVersion = "V1";
    }
    else if(CommonData::getStrPropertiesCabinetType()==g_strPropertiesCabinetTypeCC262)
    {
        singleCRFwAndXml.cabVersion = "V2";
    }

    if(strCRSingleFwAndXML.contains("XML"))
    {
        singleCRFwAndXml.updateType = "xml";
    }
    else
    {
        singleCRFwAndXml.updateType = "firmware";
    }
    if(singleCRFwAndXml.cabVersion == "V2" && singleCRFwAndXml.slaveId == 1)
    {
        singleCRFwAndXml.updateFile = updateFile1;
        singleCRFwAndXml.updateFile2 = updateFile2;
    }
    else
    {
        singleCRFwAndXml.updateFile = updateFile1;
        singleCRFwAndXml.updateFile2 = "";
    }

    qDebug()<<"singleCRFwAndXml ";
    singleCRFwAndXml.toString();
    m_singleCRFwAndXml = singleCRFwAndXml;
    if(m_httpProtocol->postUpdateCRSingleFwAndXml(m_currentIp,singleCRFwAndXml) == g_strSuccess){
        sleep(10000);
        m_singleUpgradeTimer->start(1000);
    }else{
        qDebug()<<"  single upgrade postUpdateCRSingleFwAndXml Failed ";
    }


}

void MainWidget2::wigdet2UpdateM1ProSingleFwAndXML(QString strLabelSingleFwAndXML,QString currentLabelFwOrXMLLocation)
{
    QString strSingleFwAndXML = strLabelSingleFwAndXML;
    DobotType::StructM1ProSingleFwAndXml singleM1ProFwAndXml;
    if(strSingleFwAndXML.contains("UniIO"))
    {
        singleM1ProFwAndXml.slaveId = 1;
    }

    //J1-J4 ui获取当前选项label
    QString currentServoLocation = currentLabelFwOrXMLLocation;
    if(strSingleFwAndXML.contains("Servo"))
    {

        if(currentServoLocation.contains("J1"))
        {
            singleM1ProFwAndXml.slaveId = 2;
        }
        if(currentServoLocation.contains("J2"))
        {
            singleM1ProFwAndXml.slaveId = 3;
        }
        if(currentServoLocation.contains("J3"))
        {
            singleM1ProFwAndXml.slaveId = 4;
        }
        if(currentServoLocation.contains("J4"))
        {
            singleM1ProFwAndXml.slaveId = 5;
        }
    }


    singleM1ProFwAndXml.operationType = "start";

    if(strSingleFwAndXML.contains("XML"))
    {
        singleM1ProFwAndXml.updateType = "xml";
    }
    else
    {
        singleM1ProFwAndXml.updateType = "firmware";
    }

    singleM1ProFwAndXml.updateFile = strSingleFwAndXML;

    if(m_httpProtocol->postUpdateM1ProSingleFwAndXml(m_currentIp,singleM1ProFwAndXml) == "success"){
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

    if(strSingleFwAndXML.contains("XML"))
    {
        singleMG400FwAndXml.updateType = "xml";
    }
    else
    {
        singleMG400FwAndXml.updateType = "firmware";
    }

    singleMG400FwAndXml.updateFile = strSingleFwAndXML;

    if(m_httpProtocol->postUpdateMG400SingleFwAndXml(m_currentIp,singleMG400FwAndXml) == "success"){
        //set True TODO
    }else{
        //set False TODO
    }
}

void MainWidget2::setDiyPos()
{
    this->setGeometry(m_posDiyX,m_posDiyY,this->width(),this->height());
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
        ui->btnMax->setChecked(false);
        ui->btnMax->clicked(false);
        this->move(move_pos-move_point);
    }
}

void MainWidget2::mouseReleaseEvent(QMouseEvent *e)
{
    mouse_press = false;
    this->setCursor(Qt::ArrowCursor);
}

void MainWidget2::sleep(int milliseconds)
{
    QTime dieTime = QTime::currentTime().addMSecs(milliseconds);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


void MainWidget2::initUpgradeWidget()
{
    ShadowWindow* isControlLogBusyShadowWindow = new ShadowWindow(this);
    m_isControlLogBusyWidget = new IsControlLogBusyWidget();
    isControlLogBusyShadowWindow->setTopWidget(this);
    isControlLogBusyShadowWindow->installWidget(m_isControlLogBusyWidget);

    connect(m_isControlLogBusyWidget,&IsControlLogBusyWidget::signal_isContinueUpgrade,this,[&](bool isContinueUpgrade){
        if(isContinueUpgrade)
        {
            QString result = m_httpProtocol->getUpdateCheckPartitionOccupancy(g_strCurrentIP);
            qDebug()<<" isContinueUpgrade --> "<<isContinueUpgrade<<" postUpdateCheckPartitionOccupancy result -->  "<<result;
            if(!result.isEmpty()&&result=="idle")
            {
                m_isControlLogBusyWidget->hide();


//                if(CommonData::getCurrentSettingsVersion().control.contains("V4")||CommonData::getCurrentSettingsVersion().control.at(0) == '4' )
//                {

//                    if(CommonData::getUpgradeSettingsVersion().control.contains("V3")||CommonData::getUpgradeSettingsVersion().control.at(0) =='3')
//                    {
//                        m_widget2SystemControlBack->show();
//                        return;
//                    }

//                }


                widget2UpgradeDevice();
            }
        }
    });

    setbtnUpgradeServoParamAllWriteHidden(true);
    m_tipWidget = new TipWidget();
    ShadowWindow* messageShadowWindow = new ShadowWindow();
    m_messageWidget = new MessageWidget();
    messageShadowWindow->setTopWidget(this);
    messageShadowWindow->installWidget(m_messageWidget);
    ui->btnDownload->hide();
    ui->btnImport->hide();
    ui->btnConnect->hide();
    m_upgradeProgressTimer = new QTimer(this);
    ui->labelTopControlType->hide();
    ui->labelTopCabinetType->hide();
    ui->btnUpgrade->hide();
    ui->btnExpandMore->hide();
    ui->textUpgradeDetail->hide();

    searchDeviceDisable(true);

    ShadowWindow* servoParamsAllUpgradeShadowWindow = new ShadowWindow(this);
    m_servoParamsAllUpgradeWidget = new ServoParamsAllUpgradeWidget();
    servoParamsAllUpgradeShadowWindow->setTopWidget(this);
    servoParamsAllUpgradeShadowWindow->installWidget(m_servoParamsAllUpgradeWidget);

    m_diyProgressDialog = new ProgressDialog();
    ShadowWindow* s1 = new ShadowWindow(this);
    s1->setTopWidget(this);
    s1->installWidget(m_diyProgressDialog);

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
    ShadowWindow* servoParamAllWriteTipWidgetShawdow = new ShadowWindow(this);
    servoParamAllWriteTipWidgetShawdow->setTopWidget(this);
    m_servoParamAllWriteTipWidget = new ServoParamAllWriteTipWidget();
    servoParamAllWriteTipWidgetShawdow->installWidget(m_servoParamAllWriteTipWidget);
    connect(m_servoParamAllWriteTipWidget,&ServoParamAllWriteTipWidget::signal_servoParamsAllWrite,this,[&]{
        m_servoParamAllWriteTipWidget->hide();
        m_servoParamAllWriteTipWidget->repaint();
        m_servoParamsAllUpgradeWidget->move(this->pos().x()+width()/2-m_servoParamsAllUpgradeWidget->width()/2,this->pos().y()+height()/2-m_servoParamsAllUpgradeWidget->height()/2);
        m_servoParamsAllUpgradeWidget->initStatus();
        m_servoParamsAllUpgradeWidget->show();

        DobotType::StructSettingsServoParams settingsServoParams;
        QList<DobotType::StructServoParam> servoParamsList;


        QHash<QString,int> dotDividedBitsHash;


        QFile inFile(QCoreApplication::applicationDirPath()+"/2023.05.24V1.0Para.csv");
        QStringList lines;/*行数据*/


        if (inFile.open(QIODevice::ReadOnly))
        {
            QTextStream stream_text(&inFile);
            while (!stream_text.atEnd())
            {
                lines.push_back(stream_text.readLine());

            }
        }
        for (int row = 2, modelRow = 0; row < lines.size(); row++,modelRow++)
        {

            QString line = lines.at(row);
            QStringList split = line.split(",");/*列数据*/

            if(split[0].contains(g_strServoParamsColValueTrue)||split[0].contains(g_strServoParamsColValueFalse))
            {
                QString address = "addr"+paramIndexToAddress(split[3]);
                int dotDivided = split[11].toInt();
                qDebug()<<"  =++++++++++++++++++++++  ";
                qDebug()<<"address   "<<address;
                qDebug()<<" dotDivided ==================== "<<dotDivided;
                dotDividedBitsHash.insert(address,dotDivided);
                qDebug()<<"  =++++++++++++++++++++++  ";
            }
            else{
                modelRow--;
                continue;
            }
            qDebug()<<"   ------  "<<split[3];
            qDebug()<<"  ---  "<<split[11].toInt();

        }

        QString errorParamsAddress;
        for(int i = 0; i < 6;i++)
        {
            servoParamsList.clear();
            QString servoParamPath =QCoreApplication::applicationDirPath()+"/upgradeFiles/"+g_strUpgradeParentVersion+"/"+g_strUpgradeVersion+"/servoCSV/"+CommonData::getControllerType().name+"/"+QString("J%1.csv").arg(i+1);
            qDebug()<<"signal_servoParamsAllWrite servoParamPath "<<servoParamPath;
            QFile servoParamFile(servoParamPath);
            if(!servoParamFile.exists())
            {
                m_messageWidget->setMessage("warn",tr("%1版本升级包中未找到%2的伺服参数文件，请重新下载该版本升级包").arg(CommonData::getUpgradeSettingsVersion().control).arg(CommonData::getControllerType().name));
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
                return;
            }
            QStringList servoParamsLines;/*行数据*/
            if (servoParamFile.open(QIODevice::ReadOnly))
            {
                QTextStream servoParamstream_text(&servoParamFile);
                while (!servoParamstream_text.atEnd())
                {
                    servoParamsLines.push_back(servoParamstream_text.readLine());

                }
            }
            QStringList servoParamsHeadDataList = servoParamsLines.at(1).split(",");

            if(servoParamsHeadDataList.size()>=4){
                if(servoParamsHeadDataList[0] == "ParaIndStr")
                {
                   for(int row = 2; row < servoParamsLines.size();row++)
                   {

                           if(servoParamsLines.at(row).split(",")[4] == "TRUE"){
                                qDebug()<<"TRUE ---   address ---"<<servoParamsLines.at(row).split(",")[0];
                                continue;
                           }
                           DobotType::StructServoParam servoParam;
                           QString paramIndex = servoParamsLines.at(row).split(",")[0];
                           QString address = "addr"+paramIndexToJAddress(paramIndex);
                           double srcValue = servoParamsLines.at(row).split(",")[3].split(":")[0].toDouble();
                           servoParam.key = address;
                           double value = dotDividedBitsValue(false,dotDividedBitsHash.value(address),srcValue);
                           qDebug()<<QString("adddress=%1, asrcValue=%2, newValue=%3").arg(address).arg(srcValue).arg(value);
                           servoParam.value = value;
                           servoParam.servoNum = QString("J%1").arg(i+1);
                           if(address == g_strAddr200118)
                           {
                               qDebug()<<QString("g_strAddr200118  key %1  value %2  servoNum %3")
                                            .arg(servoParam.key)
                                            .arg(servoParam.value)
                                            .arg(servoParam.servoNum);
                               continue;

                           }
                           servoParamsList.append(servoParam);
                   }
                }

            }

            int intSleep = servoParamsList.size()*300;
            qDebug()<<" servoParamsList #################  "<<intSleep;
            settingsServoParams.servoParams = servoParamsList;
            settingsServoParams.src = "httpClient";
            bool isPostSuccess = m_httpProtocol->postSettingsModifyServoParams(g_strCurrentIP,settingsServoParams);
            if(isPostSuccess)
            {
                sleep(intSleep);
                DobotType::StructSettingsServoParamsResult settingsServoParamsResult = m_httpProtocol->getSettingsModifyServoParams(g_strCurrentIP);

                if(settingsServoParamsResult.status)
                {
                   for(DobotType::StructServoParam servoParam : settingsServoParamsResult.servoParams)
                   {
                       if(!servoParam.status)
                       {
                           errorParamsAddress.append(QString("J%1 : ").arg(i+1)+servoParam.key+"\n");
                       }
                   }
                }
                else
                {
                    m_servoParamsAllUpgradeWidget->setStatus(-1,tr("升级失败！"));
                    m_servoParamsAllUpgradeWidget->show();
                    return;
                }
            }
            else
            {
                m_servoParamsAllUpgradeWidget->setStatus(-1,tr("升级失败！"));
                m_servoParamsAllUpgradeWidget->show();
                return;
            }
        }


        if(!errorParamsAddress.isEmpty())
        {
            m_servoParamsAllUpgradeWidget->setStatus(-2,tr("升级失败！"),errorParamsAddress);
            m_servoParamsAllUpgradeWidget->show();
        }
        else
        {
            m_servoParamsAllUpgradeWidget->setStatus(2,tr("升级完成！"));
            m_servoParamsAllUpgradeWidget->show();
        }
    });
    connect(ui->btnUpgradeWidgetServoParamAllWrite,&QPushButton::clicked,this,[&]{




        DobotType::StructSettingsServoParams settingsServoParams;
        QList<DobotType::StructServoParam> servoParamList;
        settingsServoParams.src = "";
        //TODO 处理下伺服参数是否是最新值的逻辑
        DobotType::ControllerType controllerType = CommonData::getControllerType();
        QString path =QCoreApplication::applicationDirPath()+"/KeyParameter.csv";
        qDebug()<<"path &&&&&&&&&&&&& "<<path;
        QFile inFile(path);
        if(!inFile.exists())
        {
            m_messageWidget->setMessage("warn",tr("没有找到表格"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
            return;
        }
        QStringList lines;/*行数据*/
        if (inFile.open(QIODevice::ReadOnly))
        {
            QTextStream stream_text(&inFile);
            while (!stream_text.atEnd())
            {
                lines.push_back(stream_text.readLine());

            }
        }
        QStringList headDataList = lines.at(1).split(",");
        for(int i = 0 ; i < 6;i++)
        {

            //寻找机型表格
            QString servoParamPath =QCoreApplication::applicationDirPath()+"/upgradeFiles/"+g_strUpgradeParentVersion+"/"+g_strUpgradeVersion+"/servoCSV/"+controllerType.name+"/"+QString("J%1.csv").arg(i+1);
            qDebug()<<"servoParamPath "<<servoParamPath;
            QFile servoParamFile(servoParamPath);
            if(!servoParamFile.exists())
            {
                //3.5.6版本升级包中未找到CR5的伺服参数文件，请重新下载该版本升级包
                m_messageWidget->setMessage("warn",tr("%1版本升级包中未找到%2的伺服参数文件，请重新下载该版本升级包").arg(CommonData::getUpgradeSettingsVersion().control).arg(CommonData::getControllerType().name));
                m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
                m_messageWidget->show();
                return;
            }

            m_servoParamAllWriteTipWidget->move(this->pos().x()+width()/2-m_servoParamAllWriteTipWidget->width()/2,this->pos().y()+height()/2-m_servoParamAllWriteTipWidget->height()/2);
            m_servoParamAllWriteTipWidget->initStatus();
            m_servoParamAllWriteTipWidget->show();
            servoParamList.clear();
            QHash<QString,int> dotDividedBitsHash;
            if(headDataList.size()>=4){
                if(headDataList[0] == "ParaIndStr")
                {
                   for(int row = 2; row < lines.size();row++)
                   {

                           DobotType::StructServoParam servoParam;
                           QString paramIndex = lines.at(row).split(",")[0];
//                           qDebug()<<"lines.at(row).split(\",\")[0] --- "<<paramIndex;
                           QString address = "addr"+paramIndexToJAddress(paramIndex);
//                           qDebug()<<"DobotType::StructServoParam servoParam address "<<address;
                           servoParam.servoNum = QString("J%1").arg(i+1);
                           servoParam.key = address;
                           servoParamList.append(servoParam);
                           dotDividedBitsHash.insert(address,lines.at(row).split(",")[5].toInt());

                   }
                }

            }
            qDebug()<<"servoParamList 999999999999999999999999 "<<servoParamList.size();
            settingsServoParams.servoParams = servoParamList;
            bool ispostSettingsReadServoParams = m_httpProtocol->postSettingsReadServoParams(g_strCurrentIP,settingsServoParams);
            if(!ispostSettingsReadServoParams)
            {
                qDebug()<<" ServoParamAllWriteTipWidget::signal_servoParamsAllWrite postSettingsReadServoParams failed ";
                return;
            }


            QStringList servoParamsLines;/*行数据*/
            if (servoParamFile.open(QIODevice::ReadOnly))
            {
                QTextStream servoParamstream_text(&servoParamFile);
                while (!servoParamstream_text.atEnd())
                {
                    servoParamsLines.push_back(servoParamstream_text.readLine());

                }
            }
            QStringList servoParamsHeadDataList = servoParamsLines.at(1).split(",");

            QHash<QString,double> servoParamsCSVHash;
            if(servoParamsHeadDataList.size()>=4){
                if(servoParamsHeadDataList[0] == "ParaIndStr")
                {
                   for(int row = 2; row < servoParamsLines.size();row++)
                   {

                           DobotType::StructServoParam servoParam;
                           QString paramIndex = servoParamsLines.at(row).split(",")[0];
//                           qDebug()<<"lines.at(row).split(\",\")[0] --- "<<paramIndex;
                           QString address = "addr"+paramIndexToJAddress(paramIndex);
//                           qDebug()<<"DobotType::StructServoParam servoParam address "<<address;
                           servoParam.key = address;
                           double value = servoParamsLines.at(row).split(",")[3].split(":")[0].toDouble();
                           qDebug()<<"value ===========  "<<value;
                           servoParam.value = value;
                           servoParamsCSVHash.insert(servoParam.key,servoParam.value);
                   }
                }

            }

            int intSleepmesc = servoParamList.size()*300;
            qDebug()<<"intSleepmesc   "<<intSleepmesc;
            sleep(intSleepmesc);
            //TODO 获取相应的http数据
            DobotType::StructSettingsServoParamsResult getSettingsServoParams  = m_httpProtocol->getSettingsReadServoParams(m_currentIp);
            if(getSettingsServoParams.status)
            {
                QList<DobotType::StructServoParam> servoParamList = getSettingsServoParams.servoParams;
                for(DobotType::StructServoParam servoParam: servoParamList)
                {
                    int servoParamsCSVValue = dotDividedBitsValue(false,dotDividedBitsHash.value(servoParam.key),servoParamsCSVHash.value(servoParam.key))+0.0005;
                    int intServoParamValue = servoParam.value+0.0005;
                    qDebug()<<QString("read adddress=%1, asrcValue=%2, servoParamsCSVValue=%3 ").arg(servoParam.key).arg(servoParam.value).arg(servoParamsCSVHash.value(servoParam.key));
                    if(intServoParamValue != servoParamsCSVValue)
                    {
                        qDebug()<<"  intServoParamValue  --- "<<intServoParamValue <<" servoParamsCSVValue ---   "<<servoParamsCSVValue;
                        m_servoParamAllWriteTipWidget->setMessage(false,tr("覆盖伺服参数需保证机器下使能。\n升级成功后首次运行机器时，\n请远离机械臂确保自身安全。"));
                        m_servoParamAllWriteTipWidget->show();
                        return;
                    }
                }

            }
        }


        m_servoParamAllWriteTipWidget->setMessage(true,tr("伺服参数已为最新版本，无需升级。"));
        m_servoParamAllWriteTipWidget->show();
    });


    m_upgrade2Widget = new Upgrade2Widget();
    m_upgrade2WidgetShawdowPng = new ShadowWindowPng(m_upgrade2Widget);
    m_upgrade2WidgetShawdowPng->setEnableScale(false);
    m_upgrade2WidgetShawdowPng->setMinimumSize(m_upgrade2Widget->minimumSize().width()+m_upgrade2WidgetShawdowPng->getShadowSize()*2,m_upgrade2Widget->minimumSize().height()+m_upgrade2WidgetShawdowPng->getShadowSize()*3);
    connect(m_upgrade2Widget,&Upgrade2Widget::singal_upgradeSingle2Count,this,&MainWidget2::slot_upgradeWidgetUpgradeSingle2Count);
    connect(m_upgrade2Widget,&Upgrade2Widget::signal_interruptUpgradeUpgrade2Widget,this,[&]{
            m_isInterruptUpgrade = true;
            m_pFileTransfer->kill();
            m_mainControlProcess->terminate();
            m_mainControlProcess->kill();
            DobotType::UpdateFirmware updateFirmware;
            updateFirmware.operation = "stop";
            m_httpProtocol->postUpdateCRFwAndXml(m_currentIp,updateFirmware);
            m_upgradeProgressTimer->stop();
    });
    connect(m_upgrade2Widget,&Upgrade2Widget::signal_upgrade2WidgetFinishUpgrade,this,[&](int status){
        m_upgradeProgressTimer->stop();
        //升级成功
        if(status == 2){
            if(!m_propertiesCabinetType.contains("标准")){
                m_controlAutoConnect->show();
                m_controlAutoConnect->initStatus();
                m_isUpgradeSuccessTimer = new QTimer(this);
                connect(m_isUpgradeSuccessTimer,&QTimer::timeout,this,&MainWidget2::slot_getHttpExchange);
                m_isUpgradeSuccessTimer->start(1000);
            }
            m_isUpgradeSuccessStatus = 2;
        }
        //升级失败
        if(status == -1){
            m_isUpgradeSuccessStatus = -1;

        }
        //重新升级
        if(status == 1){
            upgradeAllFirmwareAndXMLs();
        }

    });


    m_V4ControlBackProcess = new QProcess();
    QObject::connect(m_V4ControlBackProcess,&QProcess::errorOccurred,[&](QProcess::ProcessError error){
        qWarning()<<"slot_ errorOccurred!" <<error;
    });

    QObject::connect(m_V4ControlBackProcess,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<"m_V4ControlBackProcess finished! " <<exitStatus;
        QString cmdResult = m_V4ControlBackProcess->readAll();
        qDebug()<< "m_V4ControlBackProcess cmdResult "<<cmdResult;
        if(QProcess::NormalExit != exitStatus)
        {
            m_upgrade2Widget->setMainControlStatus(-1);
            return;
        }
        m_upgrade2Widget->setMainControlStatus(2);
        m_isControlBack = true;
        m_pFileTransfer->start(QCoreApplication::applicationDirPath()+"/tool/fileTransfer/fileTransfer.bat",QStringList()<<m_currentIp<<m_upgradeParentVersion+"/"+m_upgradeVersion);

    });


    m_widget2SystemControlBack = new Widget2SystemControlBack();
    connect(m_widget2SystemControlBack,&Widget2SystemControlBack::signal_systemControlBack,this,[&](QString filePath){
        qDebug()<<"filePath  "<<filePath;
        if(filePath.contains(CommonData::getUpgradeSettingsVersion().control)){
            m_V4ControlBackFilePath = filePath;
            qDebug()<<"m_V4ControlBackFilePath  "<<m_V4ControlBackFilePath;
            widget2UpgradeDevice();
        }
        else
        {
            m_messageWidget->setMessage("warn",tr("当前备份包与选择升级的控制器版本不符，\n请检查备份包开头的控制器版本\n与将要升级版本是否一致。"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
        }



    });

    ui->btnExpandMore->setProperty("isExpanded",false);
    connect(ui->btnExpandMore,&QPushButton::clicked,this,&MainWidget2::slot_expandMore);

    //初始化安装包下载功能
    m_upgradeFileZipDownload = new DownloadTool();
    connect(m_upgradeFileZipDownload,&DownloadTool::sigProgress,this,&MainWidget2::slot_downloadProgress);
    connect(m_upgradeFileZipDownload,&DownloadTool::sigDownloadFinished,this,&MainWidget2::slot_downloadFinished);
    ui->btnDownload->setCursor(QCursor(Qt::PointingHandCursor));
    connect(ui->btnDownload,&QPushButton::clicked,this,&MainWidget2::slot_download);

    //初始化全版本配置文件下载功能
    m_upgradeAllVersionIniDownload = new DownloadTool();
    connect(m_upgradeAllVersionIniDownload,&DownloadTool::sigDownloadFinished,this,&MainWidget2::slot_downloadAllVersionIniFinished);

    m_upgradeAllVersionIniDownload->startDownload("http://cdn.release.dobot.cc/dobotUpgradeToolFile/allVersion.ini"
                            ,QCoreApplication::applicationDirPath()+"/upgradeFiles");

    //初始化文件导入功能
    m_importTempVersionHandler = new CopyFolderAFiles();
    connect(m_importTempVersionHandler,&CopyFolderAFiles::signal_copyFolderFinished,this,&MainWidget2::slot_importFinishedTempVersion);
    connect(m_importTempVersionHandler,&CopyFolderAFiles::signal_copyFolderProgress,this,&MainWidget2::slot_importTempVersionProgress);
    ui->btnImport->setCursor(QCursor(Qt::PointingHandCursor));
    connect(ui->btnImport,&QPushButton::clicked,this,&MainWidget2::slot_importTempVersion);

    //初始化固件升级功能
    connect(ui->btnUpgrade,&QPushButton::clicked,this,&MainWidget2::slot_upgradeDevice);



    //初始化手动添加设备功能
    m_manualIpWidget = new ManualIpWidget();
    m_manualIPShadowWindow = new ShadowWindow(this);
    connect(m_manualIpWidget,&ManualIpWidget::signal_saveIpsToMainWindow,this,&MainWidget2::slot_saveIpsFromManual);
    connect(ui->btnManuallyAdd,&QPushButton::clicked,this,&MainWidget2::slot_openManuallyIPWidget);
    m_ipList.append("192.168.5.1");
    m_ipList.append("192.168.1.6");
    m_ipList.append("192.168.9.1");
    ui->boxDevice->setDisabled(true);
    ui->boxDevice->addItem(tr("搜索设备中..."),QVariant("SearchDevices"));

    ui->btnConnect->setProperty("isConnected",-1);
    connect(ui->btnConnect,&QPushButton::clicked,this,&MainWidget2::slot_connectClicked);

    m_controlMenu = new QMenu();
    connect(m_controlMenu,&QMenu::triggered,this,&MainWidget2::slot_menuClicked);
    QString m_controlMenu_qss = "QMenu {"
        "background-color: #FAFDFF;"
        "color: rgb(100,100,100);"
        "border: 0.5px solid #DFE3E5;"
        "border-radius: 3px; }"
        "QMenu::item {"
        "background-color: transparent;"
        "border-bottom: 0.5px solid #DFE3E5;"
        "padding:8px 65px;"
        "margin:0px 0px; }"
        "QMenu::item:selected {"
        "background-color: #FAFDFF; "
        "color: #0172e5;"
        "}";
    m_controlMenu->setWindowFlag(Qt::NoDropShadowWindowHint);
    m_controlMenu->setStyleSheet(m_controlMenu_qss);
    ui->btnMenu->setMenu(m_controlMenu);
    searchMenuItems();
    m_searchDeviceAndIsOnlineTimer = new QTimer(this);
    connect(m_searchDeviceAndIsOnlineTimer,&QTimer::timeout,this,&MainWidget2::slot_searchDevice);
    connect(m_searchDeviceAndIsOnlineTimer,&QTimer::timeout,this,&MainWidget2::slot_isOnline);
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
        if(m_httpProtocol->postFaultRepair(m_currentIp,"dns") == "success"){
            m_firmwareCheckItemDns->setLabelStatus(2);
        }else{
            m_firmwareCheckItemDns->setLabelStatus(-1);
        }
    });
    m_itemDns->setSizeHint(QSize(100,50));

    m_itemMacAddress = new QListWidgetItem();
    m_firmwareCheckItemMacAddress = new WidgetFirmwareCheckItem();
    connect(m_firmwareCheckItemMacAddress,&WidgetFirmwareCheckItem::signal_firmwareCheckRepair,this,[&]{
        if(m_httpProtocol->postFaultRepair(m_currentIp,"mac",getCurrentUseAddress()) == g_strSuccess){
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

    m_widgetSystemToolTip = new WidgetSystemToolTip();
    m_widgetSystemToolTip->resize(420,250);
    ShadowWindow* widgetSystemToolTipShadow = new ShadowWindow(this);
    widgetSystemToolTipShadow->setTopWidget(this);
    widgetSystemToolTipShadow->installWidget(m_widgetSystemToolTip);


    ui->widgetSystemLaserCab->hide();

    connect(ui->btnSystemSynTime,&QPushButton::clicked,this,[&]{
        if(m_isPasswordConfirm)
        {
            systemToolSyncTime();
        }
        else
        {
            m_widgetSystemSyncTimePassword->move(this->pos().x()+width()/2-m_widgetSystemSyncTimePassword->width()/2,this->pos().y()+height()/2-m_widgetSystemSyncTimePassword->height()/2);
            m_widgetSystemSyncTimePassword->initStatus();
            m_widgetSystemSyncTimePassword->show();
        }
    });

    //文件完整性检测进程
    m_pFileCheck = new QProcess();
    connect(m_pFileCheck,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),this,[&]{
        QString result = m_pFileCheck->readAll();
        qDebug()<<result;

        //error_file.txt
        QFile file(QCoreApplication::applicationDirPath()+"/tool/fileCheck/src/error_file.txt");
        if(!file.exists())
        {
            m_widgetSystemFileCheckTip->setMessage(SystemFileCheckTipStatus::FileCheck_SUCCESS);
            qDebug()<<"error_file.txt not exist";
            return;
        }
        if(file.open(QIODevice::ReadOnly))
        {
            QString error_files = file.readAll();
            m_widgetSystemFileCheckTip->setMessage(SystemFileCheckTipStatus::FileCheck_FAIL,error_files);
            qDebug()<<"error_files **********";
            qDebug()<<error_files;
        }
        else
        {

            qDebug()<<"error_file.txt read fail";
        }

    });
    connect(ui->btnSystemFileCheck,&QPushButton::clicked,this,[&]{
        QString strFileCheck = QCoreApplication::applicationDirPath()+"/tool/fileCheck/check_file.exe";

        DobotType::ControllerType controllerType = m_httpProtocol->getControllerType(m_currentIp);
        QString produce_type;
        if(controllerType.name.contains("M1Pro")&&CommonData::getCurrentSettingsVersion().control.contains("1.5.8"))
        {
            produce_type = "m1pro";
        }
        if(controllerType.name.contains("MagicianPro")&&CommonData::getCurrentSettingsVersion().control.contains("1.5.8"))
        {
            produce_type = "1580stable";

        }

        if(controllerType.name.contains("CR")&&CommonData::getCurrentSettingsVersion().control.contains("3.5.3.0"))
        {
            produce_type = "3530stable";
        }

        if(controllerType.name.contains("CR")&&CommonData::getCurrentSettingsVersion().control.contains("3.5.2"))
        {
            produce_type = "3528stable";
        }

        if(controllerType.name.contains("CR")&&CommonData::getCurrentSettingsVersion().control.contains("3.5.4.0"))
        {
            produce_type = "3540stable";
        }

        if(controllerType.name.contains("CR")&&CommonData::getCurrentSettingsVersion().control.contains("3.5.4.1"))
        {
            produce_type = "3541stable";
        }

        if(controllerType.name.contains("CR")&&CommonData::getCurrentSettingsVersion().control.contains("3.5.4.2"))
        {
            produce_type = "3542stable";
        }

        if(controllerType.name.contains("CR")&&CommonData::getCurrentSettingsVersion().control.contains("3.5.5.0"))
        {
            produce_type = "3550stable";
        }

        if(controllerType.name.contains("CR")&&CommonData::getCurrentSettingsVersion().control.contains("3.5.6.0"))
        {
            produce_type = "3560stable";
        }

        if(controllerType.name.contains("CR")&&CommonData::getCurrentSettingsVersion().control.contains("3.5.4.1-weld-stable"))
        {
            produce_type = "3541stable_weld";
        }

        qDebug()<<"produce_type "<<produce_type;
        if(produce_type.isEmpty()){

            m_widgetSystemFileCheckTip->setMessage(SystemFileCheckTipStatus::FileCheck_VEVRSION_FAIL);
            m_widgetSystemFileCheckTip->show();
        }else{
            m_widgetSystemFileCheckTip->setMessage(SystemFileCheckTipStatus::FileCheck_DOING);
            m_widgetSystemFileCheckTip->show();
            m_pFileCheck->start(QCoreApplication::applicationDirPath()+"/tool/fileCheck/check_file.exe",QStringList()<<m_currentIp<<produce_type);
        }



    });



    m_pPluginLaserCalibrationProcess = new QProcess();
    connect(ui->btnSystemLaserCab,&QPushButton::clicked,this,[&]{
        if(m_isPasswordConfirm)
        {
            m_pPluginLaserCalibrationProcess->start(QCoreApplication::applicationDirPath()+"/tool/LaserCalibration/PluginLaserCalibration.exe");
        }
        else
        {
            m_widgetSystemLaserCabPassword->move(this->pos().x()+width()/2-m_widgetSystemLaserCabPassword->width()/2,this->pos().y()+height()/2-m_widgetSystemLaserCabPassword->height()/2);
            m_widgetSystemLaserCabPassword->initStatus();
            m_widgetSystemLaserCabPassword->show();
        }


    });



    connect(m_widgetSystemToolTip,&WidgetSystemToolTip::signal_widgetSystemToolWifiIpRevovery,this,[&]{
        sleep(1000);
        m_widgetSystemToolTip->move(this->pos().x()+width()/2-m_widgetSystemToolTip->width()/2,this->pos().y()+height()/2-m_widgetSystemToolTip->height()/2);
        if(m_httpProtocol->postFaultRepair(m_currentIp,"defaultIp") == "success"){
            m_widgetSystemToolTip->setRepairStatus(4);
        }else{
            m_widgetSystemToolTip->setRepairStatus(-1);
        }

    });
    connect(ui->btnSystemDNSRepair,&QPushButton::clicked,this,[&]{
        m_widgetSystemToolTip->move(this->pos().x()+width()/2-m_widgetSystemToolTip->width()/2,this->pos().y()+height()/2-m_widgetSystemToolTip->height()/2);
        m_widgetSystemToolTip->show();
        m_widgetSystemToolTip->setRepairStatus(1);
        sleep(1000);
        if(m_httpProtocol->postFaultRepair(m_currentIp,"dns")=="success"){
            m_widgetSystemToolTip->setRepairStatus(2);
        }else{
            m_widgetSystemToolTip->setRepairStatus(-1);
        }


    });

    connect(ui->btnSystemMacRepair,&QPushButton::clicked,this,[&]{
        m_widgetSystemToolTip->move(this->pos().x()+width()/2-m_widgetSystemToolTip->width()/2,this->pos().y()+height()/2-m_widgetSystemToolTip->height()/2);
        m_widgetSystemToolTip->show();
        m_widgetSystemToolTip->setRepairStatus(1);
        QString currentUseAddress = getCurrentUseAddress();
        if(m_httpProtocol->postFaultRepair(m_currentIp,"mac",currentUseAddress)=="success"){
            m_widgetSystemToolTip->setMacAddress("000EC69"+currentUseAddress);
            m_widgetSystemToolTip->setRepairStatus(22);
        }else{
            m_widgetSystemToolTip->setRepairStatus(-1);
        }
    });


    connect(ui->btnSystemOverTurnRepair,&QPushButton::clicked,this,[&]{
        m_widgetSystemToolTip->move(this->pos().x()+width()/2-m_widgetSystemToolTip->width()/2,this->pos().y()+height()/2-m_widgetSystemToolTip->height()/2);
        m_widgetSystemToolTip->show();
        m_widgetSystemToolTip->setRepairStatus(1);
        if(m_httpProtocol->postFaultRepair(m_currentIp,"overTurn")=="success"){
            m_widgetSystemToolTip->setRepairStatus(21);
        }else{
            m_widgetSystemToolTip->setRepairStatus(-1);
        }
    });

    connect(ui->btnSystemDefaultIpRepair,&QPushButton::clicked,this,[&]{
        m_widgetSystemToolTip->move(this->pos().x()+width()/2-m_widgetSystemToolTip->width()/2,this->pos().y()+height()/2-m_widgetSystemToolTip->height()/2);
        m_widgetSystemToolTip->show();
        m_widgetSystemToolTip->setRepairStatus(3);
    });

    m_widgetSystemLogExport = new WidgetSystemLogExport();
    ShadowWindow* widgetSystemLogExportShadow = new ShadowWindow(this);
    widgetSystemLogExportShadow->setTopWidget(this);
    widgetSystemLogExportShadow->installWidget(m_widgetSystemLogExport);

    connect(m_widgetSystemLogExport,&WidgetSystemLogExport::signal_systemLogExport,this,[&](QString projectName,QString dstExportDir){
        m_systemLogExportDialog->setTitle(tr("日志导出"));
        m_systemLogExportDialog->setHeadLabel("export");
        m_systemLogExportDialog->move(this->pos().x()+width()/2-m_systemLogExportDialog->width()/2,this->pos().y()+height()/2-m_systemLogExportDialog->height()/2);
        m_systemLogExportDialog->setProgressRange(300);
        ShadowWindow* s4 = new ShadowWindow(this);
        s4->setTopWidget(this);
        s4->installWidget(m_systemLogExportDialog);

        m_systemLogExportDialog->show();
        m_systemLogExportDialog->setProgressValue(1);
        QProcess* p = new QProcess();
        QObject::connect(p,&QProcess::errorOccurred,[&](QProcess::ProcessError error){
            qWarning()<<"slot_ errorOccurred!" <<error;
        });
        static int progressValue = 1;
        QObject::connect(p,&QProcess::readyRead,[&]{
            progressValue +=1;
            qDebug()<<"progressValue+++++++++++  "<<progressValue;
            m_systemLogExportDialog->setProgressValue(progressValue);

        });


        QObject::connect(p,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
            qDebug()<<"m_widgetSystemLogExport finished!  " <<exitCode <<" exitStatus "<<exitStatus;
            QString cmdResult = p->readAll();
            qDebug()<< "m_widgetSystemLogExport cmdResult "<<cmdResult;
            if(cmdResult.contains("logExport successfully")){

                //机器型号_控制柜型号_控制器版本_导出日期时间_IL
                DobotType::ControllerType controllerType = m_httpProtocol->getControllerType(m_currentIp);

                //获取当前时间
                QDateTime current_time = QDateTime::currentDateTime();
                QString currentTime = current_time.toString("yyyyMMdd");
                QFile file(QCoreApplication::applicationDirPath()+"/tool/logExport/cache" + "/"+controllerType.name+"_"+currentTime+"_VN.txt");
                   if(file.open(QIODevice::ReadWrite|QIODevice::Text)){
                       QString temp; // 写入内容

                       if(m_propertiesCabinetType.contains("标准")){
                              temp+= "大型控制柜:\n";
                              temp+= "控制器版本: "+CommonData::getCurrentSettingsVersion().control+"\n";
                              temp+= "系统版本:"+CommonData::getCurrentSettingsVersion().system+"\n";
                              temp+= "馈能板:"+CommonData::getCurrentSettingsVersion().feedback+"\n";
                              temp+= "安全IO:"+CommonData::getCurrentSettingsVersion().safeio+"\n";
                              temp+= "通用IO:"+CommonData::getCurrentSettingsVersion().unio+"\n";
                              temp+= "伺服:"+CommonData::getCurrentSettingsVersion().servo1+"\n";
                              temp+= "末端IO:"+CommonData::getCurrentSettingsVersion().terminal+"\n";
                       }else{
                           temp+= "小型控制柜:\n";
                           temp+= "控制器版本: "+CommonData::getCurrentSettingsVersion().control+"\n";
                           temp+= "系统版本:"+CommonData::getCurrentSettingsVersion().system+"\n";
                           temp+= "伺服:"+CommonData::getCurrentSettingsVersion().servo1+"\n";
                           temp+= "末端IO:"+CommonData::getCurrentSettingsVersion().terminal+"\n";
                           temp+= "CCBOX:"+CommonData::getCurrentSettingsVersion().unio+"\n";
                       }

                       // 将内容写入文件
                       QTextStream out(&file);
                       out << temp;

                       file.close();
                       qDebug()<<"completed.";
                   }else{
                       qDebug()<<"failed to create a new file!";
                   }


                qDebug()<<"srcDir "<<QCoreApplication::applicationDirPath()+"/tool/logExport/cache";
                qDebug()<<"dstZip "<<dstExportDir+"/"+controllerType.name+"_"+CommonData::getStrPropertiesCabinetType()+"_"+CommonData::getCurrentSettingsVersion().control+"_"+currentTime+"_IL.rar";
                bool compress = m_zipDiyManager->zipDiyCompress(QCoreApplication::applicationDirPath()+"/tool/logExport/cache",dstExportDir+"/"+controllerType.name+"_"+CommonData::getStrPropertiesCabinetType()+"_"+CommonData::getCurrentSettingsVersion().control+"_"+currentTime+"_IL.rar",m_zipLogExportPassword);
                if(compress){
                    m_systemLogExportDialog->setProgressValue(m_systemLogExportDialog->progressRange());
                    m_systemLogExportDialog->setHeadLabel("success");
                }
            }else{
                qDebug()<<"m_widgetSystemLogExport 日志导出 失败";
            }
            progressValue = 0;

            p->deleteLater();//new code

        });
        p->start(QCoreApplication::applicationDirPath()+"/tool/logExport/logExport.bat",QStringList()<<m_currentIp<<projectName);
    });
    connect(ui->btnSystemLogExport,&QPushButton::clicked,this,[&]{
        SFtpOptFile opt;
        opt.id = SystemLogExport_OPT_LISTDIR;
        opt.strRemotePath = "/dobot/userdata/project/project";
        opt.strLocalPath = QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/files";
        opt.opt = SFtpOption::OPT_LISTDIR;
        m_csFtpClient->ssh(opt);

    });

    m_systemLogExportDialog =new Widget2ProgressDialog();
    m_systemLogExportDialog->setWarnMessage("");


}

void MainWidget2::initSingleUpgradeWidget()
{



    m_widgetSingleUpgrading = new WidgetSingleUpgrading();
    ShadowWindow* s15 = new ShadowWindow(this);
    s15->setTopWidget(this);
    s15->installWidget(m_widgetSingleUpgrading);

    connect(ui->btnSingleServoUpgradeAllFW,&QPushButton::clicked,this,[&]{
        if(CommonData::getUpgrade2WidgetServoJ1FOEStatus() == 2
                &&CommonData::getUpgrade2WidgetServoJ2FOEStatus() == 2
                &&CommonData::getUpgrade2WidgetServoJ3FOEStatus() == 2
                &&CommonData::getUpgrade2WidgetServoJ4FOEStatus() == 2
                &&CommonData::getUpgrade2WidgetServoJ5FOEStatus() == 2
                &&CommonData::getUpgrade2WidgetServoJ6FOEStatus() == 2)
        {
            setSingleFuncUpgradeStatus(2);
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJALL");
        m_strLabelSingleFwAndXML = "ServoJALL";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });

    //btnSingleServoUpgradeAllXML
    connect(ui->btnSingleServoUpgradeAllXML,&QPushButton::clicked,this,[&]{
        if(CommonData::getUpgrade2WidgetServoJ1XMLStatus() == 2
                &&CommonData::getUpgrade2WidgetServoJ2XMLStatus() == 2
                &&CommonData::getUpgrade2WidgetServoJ3XMLStatus() == 2
                &&CommonData::getUpgrade2WidgetServoJ4XMLStatus() == 2
                &&CommonData::getUpgrade2WidgetServoJ5XMLStatus() == 2
                &&CommonData::getUpgrade2WidgetServoJ6XMLStatus() == 2)
        {
            setSingleFuncUpgradeStatus(2);
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJALLXML");
        m_strLabelSingleFwAndXML = "ServoJALLXML";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });

    m_singleUpgradeTimer = new QTimer();
    connect(m_singleUpgradeTimer,&QTimer::timeout,this,[&]{
        int status = m_httpProtocol->getUpdateSingleStatus(m_currentIp);
        qDebug()<<"m_singleUpgradeTimer  status "<<status;
        static int servoCount = 1;
        if(status == -1)
        {
            m_widgetSingleUpgrading->setSingleUpgradeStatus(status);
            m_singleUpgradeTimer->stop();
            return;
        }
        //m_servoSlaveId
        if(status == 2&&m_strLabelSingleFwAndXML.contains("ALL")&&servoCount<6)
        {
            qDebug()<<"servoCount conitue ************  "<<servoCount;
            m_singleCRFwAndXml.slaveId+=1;
            QString result = m_httpProtocol->postUpdateCRSingleFwAndXml(m_currentIp,m_singleCRFwAndXml);
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
            sleep(20000);
        }
        if(servoCount == 6 && (status == -1||status == 2))
        {
            servoCount = 1;
            qDebug()<<"servoCount = 1  "<<servoCount;
            m_widgetSingleUpgrading->setSingleUpgradeStatus(status);
            m_singleUpgradeTimer->stop();
            return;
        }


        qDebug()<<"servoCount  end $$$$$$$$$$$  "<<servoCount;
        if(status != 1 && status != 0 && !m_strLabelSingleFwAndXML.contains("ALL"))
        {
            m_widgetSingleUpgrading->setSingleUpgradeStatus(status);
            m_singleUpgradeTimer->stop();
            return;
        }
    });

    connect(ui->btnSingleIOFW,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetCCBOXFOEStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("CCBOX");
        m_strLabelSingleFwAndXML = "CCBOX";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleIOFWXML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetCCBOXXMLStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("CCBOXXML");
        m_strLabelSingleFwAndXML = "CCBOXXML";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleSafeIO,&QPushButton::clicked,this,[&]{
        if(!CommonData::getStrPropertiesCabinetType().contains(g_strPropertiesCabinetTypeCC26X))
        {
            if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetSafeIOFOEStatus()) == 2)
            {
                return;
            }
        }
        else
        {
            if(CommonData::getUpgrade2WidgetSafeIOFOEStatus() == 2 && CommonData::getUpgrade2WidgetSafeBFOEStatus() == 2)
            {
                qDebug()<<" safeA --> "<<CommonData::getUpgrade2WidgetSafeIOFOEStatus()<<"  safeB--> "<<CommonData::getUpgrade2WidgetSafeBFOEStatus();
                setSingleFuncUpgradeStatus(2);
                return;
            }
        }

        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("SafeIO");
        m_strLabelSingleFwAndXML = "SafeIO";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleSafeIOXML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetSafeIOXMLStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("SafeIOXML");
        m_strLabelSingleFwAndXML = "SafeIOXML";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleFeedback,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetFeedbackStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("FeedBack");
        m_strLabelSingleFwAndXML = "FeedBack";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleUniIO,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetUniIOFOEStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("UniIO");
        m_strLabelSingleFwAndXML = "UniIO";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleUniIOXML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetUniIOXMLStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("UniIOXML");
        m_strLabelSingleFwAndXML = "UniIOXML";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ1,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ1FOEStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ1");
        m_strLabelSingleFwAndXML = "ServoJ1";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ1XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ1XMLStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ1XML");
        m_strLabelSingleFwAndXML = "ServoJ1XML";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ2,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ2FOEStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ2");
        m_strLabelSingleFwAndXML = "ServoJ2";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ2XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ2XMLStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ2XML");
        m_strLabelSingleFwAndXML = "ServoJ2XML";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ3,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ3FOEStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ3");
        m_strLabelSingleFwAndXML = "ServoJ3";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ3XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ3XMLStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ3XML");
        m_strLabelSingleFwAndXML = "ServoJ3XML";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ4,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ4FOEStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ4");
        m_strLabelSingleFwAndXML = "ServoJ4";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ4XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ4XMLStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ4XML");
        m_strLabelSingleFwAndXML = "ServoJ4XML";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ5,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ5FOEStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ5");
        m_strLabelSingleFwAndXML = "ServoJ5";
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ5XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ5XMLStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ5XML");
        m_strLabelSingleFwAndXML = "ServoJ5XML";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ6,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ6FOEStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ6");
        m_strLabelSingleFwAndXML = "ServoJ6";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleServoJ6XML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetServoJ6XMLStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("ServoJ6XML");
        m_strLabelSingleFwAndXML = "ServoJ6XML";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleTerminal,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetTerminalFOEStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("Terminal");
        m_strLabelSingleFwAndXML = "Terminal";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });
    connect(ui->btnSingleTerminalXML,&QPushButton::clicked,this,[&]{
        if(setSingleFuncUpgradeStatus(CommonData::getUpgrade2WidgetTerminalXMLStatus()) == 2)
        {
            return;
        }
        m_widgetSingleUpgrade->setCabinetType(CommonData::getStrPropertiesCabinetType());
        m_widgetSingleUpgrade->setSingleUpgradeFwOrXML("TerminalXML");
        m_strLabelSingleFwAndXML = "TerminalXML";
        m_widgetSingleUpgrade->move(this->pos().x()+width()/2-m_widgetSingleUpgrade->width()/2,this->pos().y()+height()/2-m_widgetSingleUpgrade->height()/2);
        m_widgetSingleUpgrade->show();
    });

}

void MainWidget2::initFileBackupWidget(bool reset)
{
    ui->lineBackupFileName->clear();
    ui->lineSelectedBackupDir->clear();
    if(!reset){
        DobotType::ControllerType controllerType = m_httpProtocol->getControllerType(m_currentIp);
        //获取当前时间
        QDateTime current_time = QDateTime::currentDateTime();
        QString currentTime = current_time.toString("yyyyMMdd");
        m_backupFileName = controllerType.name+"_"+CommonData::getStrPropertiesCabinetType()+"_"+CommonData::getCurrentSettingsVersion().control+"_"+currentTime+"_Backup";
        ui->lineBackupFileName->setText(m_backupFileName);
    }

}

void MainWidget2::initServoParamsWidget()
{
    ShadowWindow* widget2ServoParamsWaitShawdow = new ShadowWindow(this);
    m_widget2ServoParamsWait = new Widget2ServoParamsWait();
    widget2ServoParamsWaitShawdow->setTopWidget(this);
    widget2ServoParamsWaitShawdow->installWidget(m_widget2ServoParamsWait);

    ui->comboServo->setView(new QListView());
    m_servoReadParamsTimer = new QTimer();
    connect(m_servoReadParamsTimer,&QTimer::timeout,this,[&]{
        sleep(15000);
        DobotType::StructSettingsServoParamsResult servoParamsResult = m_httpProtocol->getSettingsReadServoParams(m_currentIp);
        if(servoParamsResult.status == true)
        {
            m_servoReadParamsTimer->stop();
            m_widget2ServoParamsWait->setStatus(2,tr("读取所有成功"));

            QHash<QString,DobotType::StructServoParam> servoParamHash;
            for(DobotType::StructServoParam servoParam:servoParamsResult.servoParams)
            {
                qDebug()<<"servoParam.key.toUpper() "<<servoParam.key.toUpper();

                servoParamHash.insert(servoParam.key.toUpper(),servoParam);

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
                qDebug()<<"m_singleReadDelegate paramIndex  "<<paramIndex;
                QString address = "addr"+paramIndexToAddress(paramIndex);
                qDebug()<<"m_singleReadDelegate  address  "<<address;
                DobotType::StructServoParam servoParam = servoParamHash.value(address);
                QModelIndex dotDividedBitsIndex = mServoParamsTableModel->index(i, colDotDividedBits, QModelIndex());
                int dotDividedBits = mServoParamsTableModel->data(dotDividedBitsIndex).toInt();
                double viewValue = dotDividedBitsValue(true,dotDividedBits,servoParam.value);

                QModelIndex index = mServoParamsTableModel->index(i, colValue, QModelIndex());
                mServoParamsTableModel->setData(index,viewValue, Qt::EditRole);


            }

        }
        else
        {
            m_servoReadParamsTimer->stop();
            m_widget2ServoParamsWait->setStatus(-1,tr("读取所有失败"));
        }
    });

    m_servoWriteParamsTimer = new QTimer();

    ui->comboServo->setDisabled(true);
    ui->btnReadAllServoParams->setDisabled(true);
    ui->btnWriteAllServoParams->setDisabled(true);
    ui->tableServoParamView->setDisabled(true);
    connect(ui->btnImportExcel,&QPushButton::clicked,this,[&]{
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                         "",
                                                         tr("Excel (*.xlsx *.csv)"));
        ui->comboServo->setDisabled(false);
        ui->btnReadAllServoParams->setDisabled(false);
        ui->btnWriteAllServoParams->setDisabled(false);
        ui->tableServoParamView->setDisabled(false);
        importServoParamsExcel(fileName);

    });

    //MVD模式管理表格
    mServoParamsTableModel = new TableModel();
    ui->tableServoParamView->setModel(mServoParamsTableModel);
    ui->tableServoParamView->setMouseTracking(true);
    ui->tableServoParamView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableServoParamView->setContextMenuPolicy(Qt::CustomContextMenu);

    delegateServoParamsTable();
    mServoParamsTableModel->setRowCount(modelRowMax);
    mServoParamsTableModel->setColumnCount(modelColMax);

    connect(ui->btnReadAllServoParams,&QPushButton::clicked,this,[&]{

        QStandardItem* item = mServoParamsTableModel->item(0,colParaIndex);
        if(item == nullptr || item->text().isEmpty())
        {
            m_messageWidget->setMessage("warn",tr("未导入Excel文件"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
            return;
        }

        QString servoJ = ui->comboServo->currentText();
        DobotType::StructSettingsServoParams settingsServoParams;


        int readCount = 0;
        for(int i = 0; i < mServoParamsTableModel->rowCount();i++)
        {
            QStandardItem* isCheckItem = mServoParamsTableModel->item(i,colIsCheck);
            if(isCheckItem == nullptr || isCheckItem->text().isEmpty())
            {
                qDebug()<<"m_singleReadDelegate item->text().isEmpty() ";
                continue;
            }
            if(isCheckItem->text() == g_strServoParamsColValueTrue)
            {
                readCount++;
            }
            else
            {
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
            qDebug()<<"exportServoParamsFile paramIndex  "<<paramIndex;
            QString address = "addr"+paramIndexToAddress(paramIndex);
            qDebug()<<"exportServoParamsFile  address  "<<address;

            servoParam.servoNum = servoJ;
            servoParam.key = address;
            servoParam.value = writeValue;
            settingsServoParams.servoParams.append(servoParam);

        }

        qDebug()<<"readCount +++++"<<readCount;
        if(readCount == 0)
        {
            m_messageWidget->setMessage("warn",tr("没有勾选任何选项！"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
            return;
        }

        bool isSuccess = m_httpProtocol->postSettingsReadServoParams(m_currentIp,settingsServoParams);
        if(!isSuccess)
        {
            m_messageWidget->setMessage("warn",tr("读取所有失败！"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
            qDebug()<<"btnReadAllServoParams postSettingsReadServoParams failed ";
        }
        else
        {
            m_widget2ServoParamsWait->initStatus();
            m_widget2ServoParamsWait->show();
            int sleepTime = readCount*300;
            qDebug()<<" sleepTime "<<sleepTime;
            sleep(sleepTime);
            DobotType::StructSettingsServoParamsResult servoParamsResult = m_httpProtocol->getSettingsReadServoParams(m_currentIp);
            if(servoParamsResult.status == true)
            {
                m_servoReadParamsTimer->stop();
                m_widget2ServoParamsWait->setStatus(2,tr("读取所有成功"));

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
                    QString address = "addr"+paramIndexToAddress(paramIndex);
                    qDebug()<<"m_singleReadDelegate  address.toLower()  "<<address.toLower();
                    DobotType::StructServoParam servoParam = servoParamHash.value(address.toLower());
                    if(servoParam.status != true)
                    {
                        qDebug()<<"servoParam.status false  "<<servoParam.status;
                        continue;
                    }
                    QModelIndex dotDividedBitsIndex = mServoParamsTableModel->index(i, colDotDividedBits, QModelIndex());
                    int dotDividedBits = mServoParamsTableModel->data(dotDividedBitsIndex).toInt();
                    double viewValue = dotDividedBitsValue(true,dotDividedBits,servoParam.value);

                    QModelIndex index = mServoParamsTableModel->index(i, colValue, QModelIndex());
                    mServoParamsTableModel->setData(index,viewValue, Qt::EditRole);

                }
                servoParamHash.clear();

            }
            else
            {
                m_servoReadParamsTimer->stop();
                m_widget2ServoParamsWait->setStatus(-1,tr("读取所有失败"));
            }

        }
    });

    connect(ui->btnWriteAllServoParams,&QPushButton::clicked,this,[&]{

        QString servoJ = ui->comboServo->currentText();
        DobotType::StructSettingsServoParams settingsServoParams;

        settingsServoParams.src = "httpClient";

        int writeCount = 0;
        for(int i = 0; i < mServoParamsTableModel->rowCount();i++)
        {
            QStandardItem* isCheckItem = mServoParamsTableModel->item(i,colIsCheck);
            if(isCheckItem == nullptr || isCheckItem->text().isEmpty())
            {
                qDebug()<<"m_singleReadDelegate item->text().isEmpty() ";
                continue;
            }

            QStandardItem* isReadOnlyItem = mServoParamsTableModel->item(i,colIsReadOnly);
            if(isReadOnlyItem == nullptr || isReadOnlyItem->text().isEmpty())
            {
                qDebug()<<"m_singleReadDelegate item->text().isEmpty() ";
                continue;
            }

            if(isCheckItem->text() == g_strServoParamsColValueTrue && isReadOnlyItem->text() != g_strServoParamsColValueTrue)
            {
                writeCount++;
            }
            else
            {
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
            QString address = "addr"+paramIndexToAddress(paramIndex);


            servoParam.servoNum = servoJ;
            servoParam.key = address;
            servoParam.value = writeValue;
            settingsServoParams.servoParams.append(servoParam);

        }

        qDebug()<<"writeCount +++++"<<writeCount;
        if(writeCount == 0)
        {
            m_messageWidget->setMessage("warn",tr("没有勾选任何选项！"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
            return;
        }

        m_widget2ServoParamsWait->initStatus();
        m_widget2ServoParamsWait->show();
        bool isSuccess = m_httpProtocol->postSettingsModifyServoParams(m_currentIp,settingsServoParams);
        if(!isSuccess)
        {
            m_messageWidget->setMessage("warn",tr("写入所有失败！"));
            m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
            m_messageWidget->show();
            qDebug()<<"btnReadAllServoParams postSettingsReadServoParams failed ";
        }
        else
        {
            m_widget2ServoParamsWait->initStatus();
            m_widget2ServoParamsWait->show();
            int sleepTime = writeCount*300;
            qDebug()<<" sleepTime "<<sleepTime;
            sleep(sleepTime);
            DobotType::StructSettingsServoParamsResult result = m_httpProtocol->getSettingsModifyServoParams(m_currentIp);
            if(result.status == true){
                QString errorParamAddList;
                for(DobotType::StructServoParam structServoParam : result.servoParams)
                {
                    if(structServoParam.status != true)
                    {
                        errorParamAddList.append(structServoParam.key+"\n");

                    }
                }
                if(errorParamAddList.isEmpty()){
                    m_widget2ServoParamsWait->setStatus(2,tr("写入所有成功"));
                }else{
                    m_widget2ServoParamsWait->setStatus(-2,tr("写入所有失败"),errorParamAddList);
                }

            }else{
                m_widget2ServoParamsWait->setStatus(-1,tr("写入所有失败"));
            }

        }
    });



}

void MainWidget2::upgradeAllFirmwareAndXMLs()
{
    sleep(500);
    DobotType::UpdateFirmware updateFirmware;
    if(m_propertiesCabinetType.contains("标准")){
        updateFirmware.cabType = "big";
    }else{
        updateFirmware.cabType = "small";
    }
    updateFirmware.operation = "start";
    if(CommonData::getStrPropertiesCabinetType().contains("CC26"))
    {
        updateFirmware.cabVersion = "V2";
    }
    else
    {
        updateFirmware.cabVersion = "V1";
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


    if(!m_isInterruptUpgrade){
       g_isRestartServer = true;
       upgradeFirmwareResult = m_httpProtocol->postUpdateCRFwAndXml(m_currentIp,updateFirmware);
       sleep(500);

    }else{
        return;
    }
    connect(m_upgradeProgressTimer,&QTimer::timeout,this,&MainWidget2::slot_upgradeProgress);
    m_upgradeProgressTimer->start(1000);

}

void MainWidget2::readUpgradeVersion(QString parentVersion,QString version)
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

    if(CommonData::getCurrentSettingsVersion().servo1.contains("0.")||CommonData::getCurrentSettingsVersion().terminal.contains("0.")){
        ui->btnUpgrade->setDisabled(true);
    }else{
        ui->btnUpgrade->setDisabled(false);
    }
    if(mainControl.isEmpty()&&g_strUpgradeParentVersion.contains("temporary"))
    {
        ui->btnUpgrade->setDisabled(true);
    }else{
        ui->btnUpgrade->setDisabled(false);
    }



    if(m_propertiesCabinetType.contains("小型")){
        CommonData::getUpgradeSettingsVersion().unio = CCBOXText;
    }else{
        CommonData::getUpgradeSettingsVersion().unio = uniIO;
    }
    QString upgradeSettingServo;
    qDebug()<<" CommonData::getSettingsVersion().servo1 "<<CommonData::getCurrentSettingsVersion().servo1;
    if(CommonData::getCurrentSettingsVersion().servo1.at(0)=='3'){
        upgradeSettingServo = servoV3;
    }else if(CommonData::getCurrentSettingsVersion().servo1.at(0)=='4'){
        upgradeSettingServo = servoV4;
    }

    DobotType::ControllerType controllerType = m_httpProtocol->getControllerType(m_currentIp);
    if(controllerType.name.contains("NC")||ui->labelCurrentJ1Version->text().at(0)=='6'){
        upgradeSettingServo = upgradeInfoSetting.value("NCServo").toString();
        terminalIO = upgradeInfoSetting.value("NCTerminal").toString();
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


    if(!CommonData::getStrPropertiesCabinetType().contains(g_strPropertiesCabinetTypeCCBOX))
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
    QJsonDocument jsonDoc;
    QJsonArray jsonArray;
    QJsonObject jsonObject;

    QString servoJ = ui->comboServo->currentText();


    QStandardItem* item = mServoParamsTableModel->item(0,colParaIndex);
    if(item == nullptr || item->text().isEmpty())
    {
        m_messageWidget->setMessage("warn",tr("未导入Excel文件"));
        m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
        m_messageWidget->show();
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
        QString address = "addr"+paramIndexToAddress(paramIndex);
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

    m_messageWidget->setMessage("",tr("导出所有伺服参数成功\n文件名称为servoParams.json"));
    m_messageWidget->move(this->pos().x()+width()/2-m_messageWidget->width()/2,this->pos().y()+height()/2-m_messageWidget->height()/2);
    m_messageWidget->show();
}

void MainWidget2::initLanguage()
{
    QSettings setting("config/language.ini",QSettings::IniFormat);
    int languageIndex = setting.value("language/style").toInt();
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

void MainWidget2::initPlink()
{
    if(m_currentIp.isEmpty()){
        return;
    }
    QEventLoop eventloop;
    QProcess* p = new QProcess();

    QString* allResult = new QString();

    QObject::connect(p,&QProcess::readyRead,[&eventloop,this,p,allResult]{
        QString result = p->readAll();
        allResult->append(result);

        qDebug().noquote().nospace()<<"****  "<<result;


        if (allResult->contains("Store key in cache")||allResult->contains("Update cached key"))
        {
            qDebug().noquote().nospace()<<p->write(QString("y\n").toUtf8());
            allResult->clear();
            return ;
        }

        if (!allResult->contains("[root@dobot ~]# "))
        {
            return ;
        }

        p->terminate();
        p->kill();
        p->deleteLater();
    });


    QObject::connect(p,&QProcess::errorOccurred,[](QProcess::ProcessError error){
        qDebug()<<"errorOccurred!" <<error;
    });

    connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),[&eventloop,p,allResult](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<"finished! " <<exitStatus;
        eventloop.quit();
        delete allResult;

    });

    p->setProcessChannelMode(QProcess::MergedChannels);
    p->start(QCoreApplication::applicationDirPath()+"/tool/startServer/plink.exe",QStringList() <<"-ssh"<<"-l" << "root" <<"-pw" <<"dobot"<<m_currentIp);
    eventloop.exec();

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

        QString updateDiskResult = m_httpProtocol->postUpdateDiskCheck(m_currentIp);
        if(updateDiskResult.isEmpty() || updateDiskResult != "success"){
            m_upgrade2Widget->setDiskCheckStatus(-1);
            qDebug()<<"postUpdateDisk  disk "+updateDiskResult;
            return;
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
            if(CommonData::getStrPropertiesCabinetType().contains(g_strPropertiesCabinetTypeCC26X))
            {
                cabVersion = "V2";
            }
            else
            {
                cabVersion = "V1";
            }

            QString result = m_httpProtocol->postUpdateCheckKernelVersion(g_strCurrentIP,cabVersion);
            if(result.isEmpty()||result!="same")
            {
                widget2UpdateDiskKernel();
                return;
            }

        }
//        if(CommonData::getCurrentSettingsVersion().control.at(0)=='3'&&CommonData::getUpgradeSettingsVersion().control.at(0)=='3')
//        {
//            widget2UpdateDiskKernel();
//            return;
//        }
//        if(CommonData::getCurrentSettingsVersion().control.contains("V3")&&CommonData::getUpgradeSettingsVersion().control.contains("V3"))
//        {
//            widget2UpdateDiskKernel();
//            return;
//        }

        Widget2MainControlUpgrade();

    }
}

void MainWidget2::Widget2MainControlUpgrade()
{

    if(CommonData::getCurrentSettingsVersion().control.contains("V4")||CommonData::getCurrentSettingsVersion().control.at(0) == '4' )
    {

        if(CommonData::getUpgradeSettingsVersion().control.contains("V3")||CommonData::getUpgradeSettingsVersion().control.at(0)=='3')
        {
            if(!m_isInterruptUpgrade){
                qDebug()<<"m_V4ControlBackProcess V4回退V3  ";
                m_V4ControlBackProcess->start(QCoreApplication::applicationDirPath()+"/tool/V4ControlBack/restore_backup.bat",QStringList()<<m_V4ControlBackFilePath<<m_currentIp);
                m_upgrade2Widget->setMainControlStatus(1);
            }else{
                qDebug()<<"m_V4ControlBackProcess  m_isInterruptUpgrade true";
            }
            return;
        }
    }

    QObject::connect(m_mainControlProcess,&QProcess::errorOccurred,[&](QProcess::ProcessError error){
        qWarning()<<"slot_ errorOccurred!" <<error;
    });

    QObject::connect(m_mainControlProcess,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode,QProcess::ExitStatus exitStatus){
        qDebug()<<"finished! " <<exitStatus;
        QString cmdResult = m_mainControlProcess->readAll();
        qDebug()<< "m_mainControlProcess cmdResult "<<cmdResult;
        if(cmdResult.contains("The upgrade has been completed")){
            m_upgrade2Widget->setMainControlStatus(2);
            if(CommonData::getUpgradeSettingsVersion().control.contains("V4"))
            {
                if(!m_A9UpdatePath.isEmpty())
                {
                    QDir dir(m_A9UpdatePath);
                    QFileInfoList fileList =  dir.entryInfoList();
                    for(QFileInfo fileInfo:fileList){
                        if(fileInfo.fileName().contains("_bak")){
                            //TODO 要记得删除当前目录的压缩包
                            qDebug()<<"fileInfo.absoluteFilePath() "<<fileInfo.absoluteFilePath();
                            qDebug()<<m_V4ExportDir+"/"+m_V4ControlAndSN+".tar.gz";
                            if(!QFile::copy(fileInfo.absoluteFilePath(),m_V4ExportDir+"/"+m_V4ControlAndSN+".tar.gz"))
                            {
                                qDebug()<<"目标目录已存在 改文件 或者其他错误原因";

                            }
                            else
                            {
                                qDebug()<<"删除主控升级脚本下的压缩包";
                                QFile::remove(fileInfo.absoluteFilePath());
                            }


                            upgradeAllFirmwareAndXMLs();
                            return;

                        }
                    }
                }
                else
                {
                    qDebug()<<"m_A9UpdatePath is Empty  ";
                }

            }else{
                upgradeAllFirmwareAndXMLs();
            }

        }else{
            m_upgrade2Widget->setMainControlStatus(-1);
            return;
        }
    });
    QDir dir(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion);

    if(!dir.exists()){
       return;
    }
    QFileInfoList fileList =  dir.entryInfoList();
    QString A9UpdatePath;
    QString A9FileName;
    for(QFileInfo fileInfo:fileList){
        if(fileInfo.fileName().contains("CR_update")){
            A9UpdatePath = fileInfo.absoluteFilePath();
            A9FileName = fileInfo.fileName();
        }
    }
    if(A9UpdatePath.isEmpty()){
        return;
    }
    m_A9UpdatePath = A9UpdatePath;
    qDebug()<<"A9UpdatePath "<<A9UpdatePath <<"  m_currentIp  "<<m_currentIp;
    qDebug()<<"A9FileName "<<"upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/"+A9FileName;
    if(ui->labelCurrentControl->text().contains(ui->labelUpgradeMainControlVersion->text())){
//        qDebug()<<"版本一样 跳过升级";
        upgradeAllFirmwareAndXMLs();
        m_upgrade2Widget->setMainControlStatus(2);
        return;
    }
    if(!m_isInterruptUpgrade){
        m_mainControlProcess->start(A9UpdatePath+"/tool_update.bat",QStringList()<<m_currentIp<<"upgradeFiles/"+m_upgradeParentVersion+"/"+m_upgradeVersion+"/"+A9FileName);
        m_upgrade2Widget->setMainControlStatus(1);
    }else{
        qDebug()<<"widget2UpgradeFileCopyToControlFinish  m_isInterruptUpgrade true";
    }
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
        ui->retranslateUi(this);

        ui->lineBackupFileName->setText(m_backupFileName);
        if(!ui->btnLanguage->isChecked()){
//            qDebug()<<"g_isEnglish  yingwen "<<g_isEnglish;
            if(!CommonData::getCurrentSettingsVersion().system.isEmpty())
            {
                ui->labelCurrentSystem->setText(tr("Current system version:")+CommonData::getCurrentSettingsVersion().system.simplified());
            }


            if(ui->boxDevice->currentText().contains("搜索设备中")){
                ui->boxDevice->setCurrentText("Devices searching...");
            }
            if(m_propertiesCabinetType.contains("小")){
                ui->labelTopCabinetType->setText(tr("CCBOX"));
            }
            if(m_propertiesCabinetType.contains("标准控制柜")){
                ui->labelTopCabinetType->setText(tr("Standard controller")+"("+CommonData::getStrPropertiesCabinetType()+")");
            }


            if(ui->btnConnect->property("isConnected").toInt()==1){
                ui->btnConnect->setText(tr("Connecting"));
            }else if(ui->btnConnect->property("isConnected").toInt()==2){
                 ui->btnConnect->setText(tr("Disconnect"));

            }
            if(ui->btnExpandMore->property("isExpanded").toBool()){
                ui->btnExpandMore->setText(tr("Retract"));
            }
            if(isOnline==true){
                ui->btnUpgrade->setText(tr("Download and Upgrade"));
            }
        }else{
//            qDebug()<<"g_isEnglish  zhongwen "<<g_isEnglish;
            if(ui->boxDevice->currentText().contains("searching")){
                ui->boxDevice->setCurrentText(tr("搜索设备中..."));
            }

            if(!CommonData::getCurrentSettingsVersion().system.isEmpty())
            {
                ui->labelCurrentSystem->setText(tr("当前系统版本：")+CommonData::getCurrentSettingsVersion().system.simplified());
            }
            ui->labelTopCabinetType->setText(m_propertiesCabinetType);


            if(ui->btnConnect->property("isConnected").toInt()==1){
                ui->btnConnect->setText(tr("连接中"));
            }else if(ui->btnConnect->property("isConnected").toInt()==2){

                ui->btnConnect->setText(tr("断开连接"));
            }
            if(ui->btnExpandMore->property("isExpanded").toBool()){
                ui->btnExpandMore->setText(tr("收回"));
            }
            if(isOnline==true){
                ui->btnUpgrade->setText(tr("下载并升级"));
            }
        }


        if(CommonData::getStrPropertiesCabinetType() == g_strPropertiesCabinetTypeCC262)
        {
            ui->labelCurrentSafeIOTheme->setText(tr("安全IO板A："));
        }
        if(ui->btnConnect->property("isConnected").toInt()==2){
            if(!ui->btnMenu->text().isEmpty()){
                readUpgradeVersion(m_upgradeParentVersion,m_upgradeVersion);
            }


            ui->labelCurrentMainControlVersion->setAttribute(Qt::WA_AlwaysShowToolTips);


            setLabelToolTip(ui->labelCurrentMainControlVersion,CommonData::getCurrentSettingsVersion().control);


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
            ui->textSingleServoJ2->setText(CommonData::getCurrentSettingsVersion().servo2);
            ui->textSingleServoJ3->setText(CommonData::getCurrentSettingsVersion().servo3);
            ui->textSingleServoJ4->setText(CommonData::getCurrentSettingsVersion().servo4);
            ui->textSingleServoJ5->setText(CommonData::getCurrentSettingsVersion().servo5);
            ui->textSingleServoJ6->setText(CommonData::getCurrentSettingsVersion().servo6);
            ui->textSingleTerminal->setText(CommonData::getCurrentSettingsVersion().terminal);
        }


        ui->btnConnect->repaint();
        QString upgradeDetail =readUpgradeDetail(m_upgradeParentVersion,m_upgradeVersion);
        if(!ui->btnMenu->text().isEmpty()&&upgradeDetail!=NULL){
            ui->textUpgradeDetail->setText(upgradeDetail);
        }

        return true;
    }
    return QWidget::event(event);
}


