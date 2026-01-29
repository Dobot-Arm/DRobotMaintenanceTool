#include "WidgetSingleUpgradeMainControl.h"
#include "ui_WidgetSingleUpgradeMainControl.h"
#include "LuaApi.h"
#include "RobotStudio/QMenuIconStyle.h"

WidgetSingleUpgradeMainControl::WidgetSingleUpgradeMainControl(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::WidgetSingleUpgradeMainControl)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnClose,&QPushButton::clicked,this,[&]{
        ui->textSelectedUpgrade->clear();
        ui->btnSelectMenu->setText("");
        ui->lineImportedFile->clear();
        setUpgradeStatus(0);
        close();
    });

    connect(ui->btnRadioSelect,&QPushButton::clicked,this,[&]{
        setUpgradeStatus(0);
    });
    connect(ui->btnRadioImport,&QPushButton::clicked,this,[&]{
        setUpgradeStatus(1);
    });

    connect(ui->btnImportDir,&QPushButton::clicked,this,[&]{
        QString srcDir = QFileDialog::getExistingDirectory(nullptr,QString(),
                                                        "/home",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
        if(!srcDir.isEmpty())
        {
            ui->lineImportedFile->setText(srcDir);
        }
    });

    connect(ui->btnSingleUpgrade,&QPushButton::clicked,this,[&]{
        if(ui->btnRadioSelect->isChecked()){
            ui->lineImportedFile->clear();
            qDebug()<<" ui->textSelectedUpgrade mainControl  "<<ui->textSelectedUpgrade->text();
            if(ui->textSelectedUpgrade->text().isEmpty())
            {
                qDebug()<<"ui->textSelectedUpgrade 为空 ";
                return;
            }
            emit signalUpgradeSingleMainControl(m_mainControlFilePath);
            ui->textSelectedUpgrade->clear();
            ui->btnSelectMenu->setText("");
        }
        else
        {
            ui->textSelectedUpgrade->clear();
            ui->btnSelectMenu->setText("");

            QString filePath = ui->lineImportedFile->text();
            if(filePath.isEmpty())
            {
                qDebug()<<"ui->lineImportedFile 为空 ";
                return;
            }
            qDebug()<<"filePath import mainControl "<<filePath;
            QStringList filePathList = filePath.split("/");
            QString fileName = filePathList.at(filePathList.size()-1);
            qDebug()<<"fileName import  mainControl "<<fileName;

            emit signalUpgradeSingleMainControl(filePath);
            ui->lineImportedFile->clear();
        }
        setUpgradeStatus(0);
        close();
    });

    m_singleMenu = new QMenu();
    m_singleMenu->setStyle(new QMenuIconStyle());
    connect(m_singleMenu,&QMenu::triggered,this,&WidgetSingleUpgradeMainControl::slot_menuClicked);
    QString m_controlMenu_qss = "QMenu{"
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
    m_singleMenu->setWindowFlag(Qt::NoDropShadowWindowHint);
    m_singleMenu->setStyleSheet(m_controlMenu_qss);
    ui->btnSelectMenu->setMenu(m_singleMenu);

    setUpgradeStatus(0);
    ui->btnRadioSelect->setChecked(true);
}

WidgetSingleUpgradeMainControl::~WidgetSingleUpgradeMainControl()
{
    delete ui;
}


static void setAllSubMenuFixWidth(QMenu* pMenu, int iWidth)
{
    pMenu->setFixedWidth(iWidth);
    auto allAction = pMenu->actions();
    for (QAction* pAct : allAction)
    {
        QMenu* pSub = pAct->menu();
        if (pSub)
        {
            setAllSubMenuFixWidth(pSub, iWidth);
        }
    }
}
void WidgetSingleUpgradeMainControl::searchMenuItems()
{
    QSettings allVersionCfg(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+g_strAllVersion+".cfg",QSettings::IniFormat);
    m_singleMenu->clear();
    qDebug()<<"  searchMenuItems  ";
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

    for(QString group:allVersionCfg.childGroups()){
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

        QMenu* currentBigVersion = m_singleMenu->addMenu(group);
        currentBigVersion->setWindowFlag(Qt::NoDropShadowWindowHint);
        QStringList existVersions = versions.value(group);
        allVersionCfg.beginGroup(group);
        QStringList keyVersions = allVersionCfg.allKeys();
        allVersionCfg.endGroup();
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
                    if (CommonData::getCurrentSettingsVersion().servo1.at(0)==2 &&
                            version.contains("Nova10V",Qt::CaseInsensitive)&&version.contains("3.5.8.1"))
                    {
                        continue;
                    }
                    else if (CommonData::getCurrentSettingsVersion().servo1.at(0)==3 &&
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
                currentBigVersion->addAction(version);
            }
        }
    }
    if(versions.contains("temporary")){
        QMenu* currentBigVersion = m_singleMenu->addMenu("temporary");
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
            currentBigVersion->addAction(version);
        }
    }
    ui->btnSelectMenu->setMenu(m_singleMenu);
    setAllSubMenuFixWidth(m_singleMenu,ui->btnSelectMenu->width());
}

void WidgetSingleUpgradeMainControl::setUpgradeStatus(int status)
{
    //0 选择升级文件 1 导入升级文件
    if (0==status){
        ui->btnSelectMenu->setEnabled(true);
    }else if (1==status){
        ui->btnSelectMenu->setEnabled(false);
    }
    ui->btnSelectMenu->setText("");
    ui->textSelectedUpgrade->clear();
}

void WidgetSingleUpgradeMainControl::slot_menuClicked(QAction *action)
{
    ui->textSelectedUpgrade->clear();
    QMenu* menuParent = (QMenu*)action->parent();
    qDebug()<<"menuParent->title();  "<<menuParent->title();

    QString fileParentDir = menuParent->title();
    QString fileName = action->text();
    QString selectedControlText = "";
    QDir upgradeDir(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+fileParentDir+"/"+fileName);
    QFileInfoList fileInfoList = upgradeDir.entryInfoList();
    for(QFileInfo fileInfo : fileInfoList)
    {
        const QString strFileName = fileInfo.fileName();
        auto info = getMatchCRUpdateInfo(strFileName);
        if(info.valid && fileInfo.isDir())
        {
            m_mainControlFilePath = fileInfo.filePath();
            qDebug()<<" m_mainControlFilePath  "<<m_mainControlFilePath;
            selectedControlText = strFileName.mid(info.index+info.length);
            ui->textSelectedUpgrade->setText(selectedControlText);
            break;
        }
    }
    ui->btnSelectMenu->setText(action->text());
}

bool WidgetSingleUpgradeMainControl::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

