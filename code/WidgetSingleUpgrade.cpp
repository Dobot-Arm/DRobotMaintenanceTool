#include "WidgetSingleUpgrade.h"
#include "ui_WidgetSingleUpgrade.h"
#include "LuaApi.h"
#include <QTimer>

WidgetSingleUpgrade::WidgetSingleUpgrade(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::WidgetSingleUpgrade)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    m_allLabel = {ui->textSelectedUpgrade,ui->textSelectedUpgrade2,ui->textSelectedUpgrade3,
                  ui->textSelectedUpgrade4,ui->textSelectedUpgrade5,ui->textSelectedUpgrade6};
    m_allLine = {qMakePair<QLineEdit*,QPushButton*>(ui->lineImportedFile,ui->btnImportFile),
                 qMakePair<QLineEdit*,QPushButton*>(ui->lineImportedFile2,ui->btnImportFile2),
                 qMakePair<QLineEdit*,QPushButton*>(ui->lineImportedFile3,ui->btnImportFile3),
                 qMakePair<QLineEdit*,QPushButton*>(ui->lineImportedFile4,ui->btnImportFile4),
                 qMakePair<QLineEdit*,QPushButton*>(ui->lineImportedFile5,ui->btnImportFile5),
                 qMakePair<QLineEdit*,QPushButton*>(ui->lineImportedFile6,ui->btnImportFile6)};

    connect(ui->btnClose,&QPushButton::clicked,this,[this]{
        ui->btnSelectMenu->setText("");
        for (int i=0; i<m_allLabel.size(); ++i){
            m_allLabel.at(i)->clear();
        }
        for (int i=0; i<m_allLine.size(); ++i){
            m_allLine.at(i).first->clear();
        }
        setUpgradeStatus(0);
        close();
    });

    m_singleMenu = new QMenu();
    connect(m_singleMenu,&QMenu::triggered,this,&WidgetSingleUpgrade::slot_menuClicked);
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

    searchMenuItems();
    connect(ui->btnRadioSelect,&QPushButton::clicked,this,[&]{
        setUpgradeStatus(0);
    });
    connect(ui->btnRadioImport,&QPushButton::clicked,this,[&]{
        setUpgradeStatus(1);
    });
    for (int i=0; i<m_allLine.size(); ++i){
        auto pL = m_allLine.at(i).first;
        auto pB = m_allLine.at(i).second;
        connect(pB,&QPushButton::clicked,this,[this,pL]{
            slot_clickSelectFile(pL);
        });
    }
    connect(ui->btnSingleUpgrade,&QPushButton::clicked,this,[&]{
        QStringList allUpdateFilePath;
        QStringList allUpdateFile;
        if(ui->btnRadioSelect->isChecked()){
            for (int i=0; i<m_allLabel.size(); ++i){
                auto p = m_allLabel.at(i);
                if (p->isHidden() || p->text().isEmpty())
                {
                    continue;
                }
                QString strSelFileName = p->text();
                QString strSelFullFile = m_singleUpgradeFileDir+"/"+strSelFileName;
                qDebug()<<QString("ui->textSelectedUpgrade(%1):").arg(i)<<strSelFullFile;
                if(!strSelFileName.isEmpty())
                {
                    allUpdateFile.append(strSelFileName);
                    allUpdateFilePath.append(strSelFullFile);
                }
            }
            if (allUpdateFile.isEmpty()){
                qDebug()<<"no files selected.....";
                return;
            }
            for (int i=0; i<m_allLabel.size(); ++i){
                m_allLabel.at(i)->clear();
            }
        }
        else
        {
            for (int i=0; i<m_allLine.size(); ++i){
                auto p = m_allLine.at(i).first;
                if (p->isHidden() || p->text().isEmpty())
                {
                    continue;
                }
                QString strSelFullFile = p->text();
                QStringList filePathList = strSelFullFile.split("/");
                QString strSelFileName = filePathList.at(filePathList.size()-1);
                qDebug()<<QString("filePath import(%1):").arg(i)<<strSelFullFile;
                if(!strSelFileName.isEmpty())
                {
                    allUpdateFile.append(strSelFileName);
                    allUpdateFilePath.append(strSelFullFile);
                }
            }
            if (allUpdateFile.isEmpty()){
                qDebug()<<"no files import.....";
                return;
            }
            for (int i=0; i<m_allLine.size(); ++i){
                m_allLine.at(i).first->clear();
            }
        }
        ui->btnSelectMenu->setText("");
        emit signal_upgradeSingleFwOrXML(allUpdateFilePath, allUpdateFile);
        setUpgradeStatus(0);
        close();
    });
    ui->btnRadioSelect->setChecked(true);
    setUpgradeStatus(0);
}

WidgetSingleUpgrade::~WidgetSingleUpgrade()
{
    delete ui;
}

void WidgetSingleUpgrade::setSingleUpgradeFwOrXML(QString fwOrXML)
{
    m_singleUpgradeFwOrXML = fwOrXML;
}

void WidgetSingleUpgrade::retranslate()
{
    ui->retranslateUi(this);
}

void WidgetSingleUpgrade::slot_clickSelectFile(QLineEdit* p)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/home",tr("BIN (*.bin *.efw)"));
    if(fileName.isEmpty()){
        return;
    }
    p->setText(fileName);
}

void WidgetSingleUpgrade::setUpgradeStatus(int status)
{

    for (int i=0; i<m_allLabel.size();++i)
    {
        if (i==0) m_allLabel.at(i)->show();
        else m_allLabel.at(i)->hide();
        m_allLabel.at(i)->clear();
    }
    for (int i=0; i<m_allLine.size();++i)
    {
        auto& pp = m_allLine.at(i);
        if (i==0) {
            pp.first->show();
            pp.second->show();
        }
        else{
            pp.first->hide();
            pp.second->hide();
        }
        pp.first->clear();
    }

    bool bNot2 = CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX)||
            CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMG400)||
            CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6)||
            CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162);
    //0 选择升级文件 1 导入升级文件
    if(status == 0)
    {
        ui->btnSelectMenu->setEnabled(true);
        if(!bNot2 && m_singleUpgradeFwOrXML.contains("SafeIO"))
        {
            ui->textSelectedUpgrade2->show();
        }
    }
    else
    {
        ui->btnSelectMenu->setEnabled(false);
        auto count = CLuaApi::getInstance()->getSingleUpdrageImportCount(m_singleUpgradeFwOrXML);
        if (count.hasValue())
        {
            for (int i=0; i<m_allLine.size() && i<count.value(); ++i)
            {
                auto& pp = m_allLine.at(i);
                pp.first->show();
                pp.second->show();
            }
        }
        else
        {
            if(!bNot2 &&m_singleUpgradeFwOrXML.contains("SafeIO") &&!m_singleUpgradeFwOrXML.contains("XML"))
            {
                ui->lineImportedFile2->show();
                ui->btnImportFile2->show();
            }
        }
    }
    ui->btnSelectMenu->setText("");
    adjustWidgetSize();
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
void WidgetSingleUpgrade::searchMenuItems()
{
    QSettings allVersionCfg(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+g_strAllVersion+".cfg",QSettings::IniFormat);
    m_singleMenu->clear();
    for (int i=0; i<m_allLabel.size(); ++i){
        auto p = m_allLabel.at(i);
        p->clear();
        if (i==0) p->show();
        else p->hide();
    }

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
                if(!CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)&&
                        !CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6)
                        &&(version.contains("CRA")||version.contains("ED6")))
                {
                    continue;
                }
            }
            currentBigVersion->addAction(version);
        }
    }
    ui->btnSelectMenu->setMenu(m_singleMenu);
    QTimer::singleShot(100, this, [this]{
        setAllSubMenuFixWidth(m_singleMenu,ui->btnSelectMenu->width());
    });
}

void WidgetSingleUpgrade::show()
{
    QWidget::show();
    ui->btnRadioSelect->click();
}

void WidgetSingleUpgrade::slot_menuClicked(QAction *action)
{
    for (int i=0; i<m_allLabel.size(); ++i){
        auto p = m_allLabel.at(i);
        p->clear();
        if (i==0) p->show();
        else p->hide();
    }
    QMenu* menuParent = (QMenu*)action->parent();
    qDebug()<<"menuParent->title();  "<<menuParent->title();

    QString fileParentDir = menuParent->title();
    QString fileName = action->text();

    QDir upgradeDir(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+fileParentDir+"/"+fileName+"/files");
    m_singleUpgradeFileDir = upgradeDir.absolutePath();
    QFileInfoList fileInfoList = upgradeDir.entryInfoList();

    const QString strCurrentServo = CommonData::getCurrentSettingsVersion().servo1;
    bool isXML = false;
    QString upgradeFW = m_singleUpgradeFwOrXML;
    if(m_singleUpgradeFwOrXML.contains("XML"))
    {
        isXML = true;
        upgradeFW = m_singleUpgradeFwOrXML.mid(0,m_singleUpgradeFwOrXML.size()-3);
        qDebug()<<"XML "<<upgradeFW;
    }
    if(m_singleUpgradeFwOrXML.contains("Servo"))
    {
        upgradeFW = upgradeFW.mid(0,5);
        qDebug()<<"Servo "<<upgradeFW;
    }
    QHash<QString,QHash<QString,QString>> allLuaVal;
    {
        QString upgradeInfoPath = QString("%1/upgradeFiles/%2/%3/%3upgrade.ini").arg(QCoreApplication::applicationDirPath()).arg(fileParentDir).arg(fileName);
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
    }
    auto allFile = CLuaApi::getInstance()->getSingleUpdrageFiles(action->text(),
                                                                 upgradeDir.entryList(QDir::Files),
                                                                 m_singleUpgradeFwOrXML,allLuaVal);
    if (allFile.hasValue())
    {
        const auto files = allFile.value();
        for (int i=0; i<files.size() && i<m_allLabel.size(); ++i){
            if (!files.at(i).isEmpty()){
                m_allLabel.at(i)->show();
                m_allLabel.at(i)->setText(files.at(i));
            }
        }
    }
    else
    {
        if(CommonData::getStrPropertiesCabinetType().name.contains("CC26"))
        {
            showCC262(fileInfoList,upgradeFW,isXML,fileParentDir);
        }
        else if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)){
            showCC162(fileInfoList,upgradeFW,isXML);
        }
        else if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX)){
            showCCBox(fileInfoList,upgradeFW,isXML);
        }
        else if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
        {
            showE6(fileInfoList,upgradeFW,isXML);
        }
    }
    ui->btnSelectMenu->setText(action->text());
    adjustWidgetSize();
}

bool WidgetSingleUpgrade::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

void WidgetSingleUpgrade::showCC262(const QFileInfoList& fileInfoList,
                                const QString upgradeFW, const bool isXML,
                                const QString fileParentDir)
{
    /* upgradeFW 只可能是以下几种值：Servo、CCBOX、SafeIO、FeedBack、UniIO、Terminal
     **/
    const QString strCurrentServo = CommonData::getCurrentSettingsVersion().servo1;
    for(QFileInfo fileInfo : fileInfoList)
    {
        QString strFileName = fileInfo.fileName();
        if(m_singleUpgradeFwOrXML.contains("SafeIO"))
        {
            if(!m_singleUpgradeFwOrXML.contains("XML"))
            {
                if(fileInfo.fileName().contains("SCBA"))
                {
                        qDebug()<<"SCBA XML "<<fileInfo.filePath();
                        ui->textSelectedUpgrade->setText(fileInfo.fileName());
                }
                if(fileInfo.fileName().contains("SCBB"))
                {
                        qDebug()<<"SCBB XML "<<fileInfo.filePath();
                        ui->textSelectedUpgrade2->setText(fileInfo.fileName());
                        ui->textSelectedUpgrade2->show();
                }
            }
            else
            {
                if(fileInfo.fileName().contains("SCB_"))
                {
                    qDebug()<<"SCB_ XML "<<fileInfo.filePath();
                    ui->textSelectedUpgrade->setText(fileInfo.fileName());
                }
            }
        }
        else if(fileInfo.fileName().contains(upgradeFW))
        {
            if(isXML)
            {
                if(!fileInfo.fileName().contains("XML")) continue;
                if(upgradeFW=="Terminal")
                {
                    if(CommonData::getControllerType().name.contains("CR20A"))
                    {
                        if(CompareVersion("4.5.0",fileParentDir)>=0)
                        {
                            if(fileInfo.fileName().contains("CR"))
                            {
                                qDebug()<<"fileInfo.filePath() CR20 RX XML "<<fileInfo.filePath();
                                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                            }
                        }
                        else if(fileInfo.fileName().contains("CR20"))
                        {
                            qDebug()<<"fileInfo.filePath() CR20 RX XML "<<fileInfo.filePath();
                            ui->textSelectedUpgrade->setText(fileInfo.fileName());
                        }
                    }
                    else
                    {
                        if(!fileInfo.fileName().contains("CR20")&&fileInfo.fileName().contains("RX")&&strCurrentServo.at(0)=='6'){
                            qDebug()<<"fileInfo.filePath() Terminal RX XML "<<fileInfo.filePath();
                            ui->textSelectedUpgrade->setText(fileInfo.fileName());
                        }
                        if(!fileInfo.fileName().contains("CR20")&&!fileInfo.fileName().contains("RX")&&strCurrentServo.at(0)=='4'){
                            qDebug()<<"fileInfo.filePath() Terminal !!RX XML "<<fileInfo.filePath();
                            ui->textSelectedUpgrade->setText(fileInfo.fileName());
                        }
                    }
                }
                else
                {
                    qDebug()<<"fileInfo.filePath() XML "<<fileInfo.filePath();
                    ui->textSelectedUpgrade->setText(fileInfo.fileName());
                }
            }
            else
            {
                if(fileInfo.fileName().contains("XML")) continue;
                if(upgradeFW.contains("Servo"))
                {
                    if(fileInfo.fileName().contains("V4")&&strCurrentServo.at(0)=='4')
                    {
                        qDebug()<<"CC262 fileInfo.filePath() !!XML Servo V4  "<<fileInfo.filePath();
                        ui->textSelectedUpgrade->setText(fileInfo.fileName());
                    }
                    else if(fileInfo.fileName().contains("V6")&&strCurrentServo.at(0)=='6')
                    {
                        qDebug()<<"CC262 fileInfo.filePath() !!XML Servo V6  "<<fileInfo.filePath();
                        ui->textSelectedUpgrade->setText(fileInfo.fileName());
                    }
                    else if ((fileInfo.fileName().contains("V4") || fileInfo.fileName().contains("V6"))&&strCurrentServo.at(0)=='0')
                    {//当读不到版本号时，就选择这个m_currentServo
                        qDebug()<<"CC262 fileInfo.filePath() !!XML Servo  "<<fileInfo.filePath();
                        ui->textSelectedUpgrade->setText(fileInfo.fileName());
                    }
                }
                else if(upgradeFW=="Terminal")
                {
                    if(CommonData::getControllerType().name.contains("CR20A"))
                    {
                        if(CompareVersion("4.5.0",fileParentDir)>=0)
                        {
                            if (fileInfo.fileName().contains("CR"))
                            {
                                qDebug()<<"fileInfo.filePath() CR20 RX !!XML "<<fileInfo.filePath();
                                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                            }
                        }
                        else if (fileInfo.fileName().contains("CR20"))
                        {
                            qDebug()<<"fileInfo.filePath() CR20 RX !!XML "<<fileInfo.filePath();
                            ui->textSelectedUpgrade->setText(fileInfo.fileName());
                        }
                    }
                    else
                    {
                        if(!fileInfo.fileName().contains("CR20")&&fileInfo.fileName().contains("V4")&&strCurrentServo.at(0)=='4')
                        {
                            qDebug()<<"CC262 fileInfo.filePath() !!XML Terminal V4 "<<fileInfo.filePath();
                            ui->textSelectedUpgrade->setText(fileInfo.fileName());
                        }

                        if(!fileInfo.fileName().contains("CR20")&&fileInfo.fileName().contains("V6")&&strCurrentServo.at(0)=='6')
                        {
                            qDebug()<<"CC262 fileInfo.filePath() !!XML Terminal V6 "<<fileInfo.filePath();
                            ui->textSelectedUpgrade->setText(fileInfo.fileName());
                        }
                    }
                }
                else
                {
                    qDebug()<<"CC262 fileInfo.filePath() !!XML !!Servo "<<fileInfo.filePath();
                    ui->textSelectedUpgrade->setText(fileInfo.fileName());
                }
            }
        }
    }
}

void WidgetSingleUpgrade::showCC162(const QFileInfoList& fileInfoList,
                                       const QString upgradeFW, const bool isXML)
{
    for(QFileInfo fileInfo : fileInfoList)
    {
        if(!fileInfo.fileName().contains(upgradeFW)) continue;

        if(isXML)
        {
            if(!fileInfo.fileName().contains("XML")) continue;
            if(upgradeFW=="Servo"){
                if (!fileInfo.fileName().startsWith("CR",Qt::CaseInsensitive)) continue;
                qDebug()<<"fileInfo.filePath() XML Servo "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
            else if(upgradeFW=="Terminal"){
                if (!fileInfo.fileName().startsWith("CR",Qt::CaseInsensitive)) continue;
                qDebug()<<"fileInfo.filePath() XML Teminal "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
            else
            {
                qDebug()<<"fileInfo.filePath() XML "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
        }else{
            if(fileInfo.fileName().contains("XML")) continue;
            if(upgradeFW=="Servo")
            {
                if (!fileInfo.fileName().startsWith("CR",Qt::CaseInsensitive)) continue;

                if(fileInfo.fileName().contains("V4")&&CommonData::getCurrentSettingsVersion().servo1.at(0)=='4')
                {
                    qDebug()<<"fileInfo.filePath() !!XML Servo V4  "<<fileInfo.filePath();
                    ui->textSelectedUpgrade->setText(fileInfo.fileName());
                    break;
                }
                if(fileInfo.fileName().contains("V2")&&CommonData::getCurrentSettingsVersion().servo1.at(0)=='2')
                {
                    qDebug()<<"fileInfo.filePath() !!XML Servo V2 "<<fileInfo.filePath();
                    ui->textSelectedUpgrade->setText(fileInfo.fileName());
                    break;
                }

                if(fileInfo.fileName().contains("V3")&&(CommonData::getCurrentSettingsVersion().servo1.at(0)!='4'
                                                        &&CommonData::getCurrentSettingsVersion().servo1.at(0)!='2'))
                {
                    qDebug()<<"fileInfo.filePath() !!XML Servo V3 "<<fileInfo.filePath();
                    ui->textSelectedUpgrade->setText(fileInfo.fileName());
                    break;
                }
            }
            else if(upgradeFW=="Terminal")
            {
                if (!fileInfo.fileName().startsWith("CR",Qt::CaseInsensitive)) continue;
                qDebug()<<"fileInfo.filePath() !!XML Terminal "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
            else
            {
                qDebug()<<"fileInfo.filePath() !!XML !!Servo "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
        }
    }
}

void WidgetSingleUpgrade::showCCBox(const QFileInfoList& fileInfoList,
                                       const QString upgradeFW, const bool isXML)
{
    for(QFileInfo fileInfo : fileInfoList)
    {
        if(!fileInfo.fileName().contains(upgradeFW)) continue;

        if(isXML)
        {
            if(!fileInfo.fileName().contains("XML")) continue;
            if(upgradeFW=="Servo"){
                if (!fileInfo.fileName().startsWith("NC_",Qt::CaseInsensitive)) continue;
                qDebug()<<"fileInfo.filePath() XML Servo NC "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
            else if(upgradeFW=="Terminal"){
                if (!fileInfo.fileName().startsWith("NC_",Qt::CaseInsensitive)) continue;
                qDebug()<<"fileInfo.filePath() XML Teminal NC "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
            else
            {
                qDebug()<<"fileInfo.filePath() XML "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
        }else{
            if(fileInfo.fileName().contains("XML")) continue;
            if(upgradeFW=="Servo")
            {
                if (!fileInfo.fileName().startsWith("NC_",Qt::CaseInsensitive)) continue;
                if(fileInfo.fileName().contains("V4")&&CommonData::getCurrentSettingsVersion().servo1.at(0)=='4')
                {
                    qDebug()<<"fileInfo.filePath() !!XML Servo V4  "<<fileInfo.filePath();
                    ui->textSelectedUpgrade->setText(fileInfo.fileName());
                    break;
                }
                if(fileInfo.fileName().contains("V2")&&CommonData::getCurrentSettingsVersion().servo1.at(0)=='2')
                {
                    qDebug()<<"fileInfo.filePath() !!XML Servo V2 "<<fileInfo.filePath();
                    ui->textSelectedUpgrade->setText(fileInfo.fileName());
                    break;
                }

                if(fileInfo.fileName().contains("V3")&&(CommonData::getCurrentSettingsVersion().servo1.at(0)!='4'
                                                        &&CommonData::getCurrentSettingsVersion().servo1.at(0)!='2'))
                {
                    qDebug()<<"fileInfo.filePath() !!XML Servo V3 "<<fileInfo.filePath();
                    ui->textSelectedUpgrade->setText(fileInfo.fileName());
                    break;
                }
            }
            else if(upgradeFW=="Terminal")
            {
                if (!fileInfo.fileName().startsWith("NC_",Qt::CaseInsensitive)) continue;
                qDebug()<<"CC262 fileInfo.filePath() !!XML Terminal "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
            else
            {
                qDebug()<<"fileInfo.filePath() !!XML !!Servo "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
        }
    }
}

void WidgetSingleUpgrade::showE6(const QFileInfoList& fileInfoList,
                             const QString upgradeFW, const bool isXML)
{
    QString strLog;
    for(QFileInfo fileInfo : fileInfoList)
    {
        if(!fileInfo.fileName().contains(upgradeFW)) continue;
        if(isXML)
        {
            if(!fileInfo.fileName().contains("XML")) continue;
            strLog = "XML";
        }
        else
        {
            if(fileInfo.fileName().contains("XML")) continue;
            strLog = "!!!XML";
        }

        //区分E6 伺服V3 V2版本
        if(upgradeFW=="Servo")
        {
            if(fileInfo.fileName().contains("V3")
                    &&CommonData::getCurrentSettingsVersion().servo1.at(0)=='3')
            {
                qDebug()<<"fileInfo.filePath() "<<strLog<<" "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
            if(fileInfo.fileName().contains("V2")
                    &&CommonData::getCurrentSettingsVersion().servo1.at(0)=='2')
            {
                qDebug()<<"fileInfo.filePath() "<<strLog<<" "<<fileInfo.filePath();
                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                break;
            }
        }
        else
        {
            qDebug()<<"fileInfo.filePath() "<<strLog<<" "<<fileInfo.filePath();
            ui->textSelectedUpgrade->setText(fileInfo.fileName());
            break;
        }
    }
}

void WidgetSingleUpgrade::adjustWidgetSize()
{
    int widgetHeight = 0;
    {
        widgetHeight += ui->btnSelectMenu->height();
        widgetHeight += ui->labelSelectedUpgrade->height();
        for (int i=0; i<m_allLabel.size(); ++i){
            auto p = m_allLabel.at(i);
            if (p->isVisible())
            {
                widgetHeight += p->height();
            }
        }
    }
    {
        for (int i=0; i<m_allLine.size(); ++i){
            auto p = m_allLine.at(i).first;
            if (p->isVisible())
            {
                widgetHeight += p->height();
            }
        }
    }
    widgetHeight += ui->labelTitle->height();
    widgetHeight += ui->btnSingleUpgrade->height();
    widgetHeight += 100;

    ui->widget->setFixedHeight(widgetHeight);
    ui->widget->resize(ui->widget->width(), widgetHeight);
}
