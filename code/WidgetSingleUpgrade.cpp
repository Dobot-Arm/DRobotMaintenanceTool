#include "WidgetSingleUpgrade.h"
#include "ui_WidgetSingleUpgrade.h"

WidgetSingleUpgrade::WidgetSingleUpgrade(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetSingleUpgrade)
    , m_singleAllVersionSetting(QCoreApplication::applicationDirPath()+"/upgradeFiles/allVersion.ini",QSettings::IniFormat)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowModality(Qt::ApplicationModal); //禁用主窗口
    connect(ui->btnClose,&QPushButton::clicked,this,[&]{
        ui->textSelectedUpgrade->clear();
        ui->btnSelectMenu->setText("");
        ui->lineImportedFile->clear();

        ui->textSelectedUpgrade2->clear();
        ui->lineImportedFile2->clear();
        setUpgradeStatus(0);
        close();
    });

    m_singleMenu = new QMenu();
    connect(m_singleMenu,&QMenu::triggered,this,&WidgetSingleUpgrade::slot_menuClicked);
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
    connect(ui->btnImportFile,&QPushButton::clicked,this,[&]{

        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                         "/home",
                                                         tr("BIN (*.bin *.efw)"));

        if(fileName.isEmpty()){
            return;
        }
        ui->lineImportedFile->setText(fileName);
    });


    connect(ui->btnImportFile2,&QPushButton::clicked,this,[&]{
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                         "/home",
                                                         tr("BIN (*.bin *.efw)"));

        if(fileName.isEmpty()){
            return;
        }
        ui->lineImportedFile2->setText(fileName);
    });

    connect(ui->btnSingleUpgrade,&QPushButton::clicked,this,[&]{
        if(ui->btnRadioSelect->isChecked()){
            ui->lineImportedFile2->clear();
            ui->lineImportedFile->clear();
            qDebug()<<" ui->textSelectedUpgrade  "<<ui->textSelectedUpgrade->text();
            qDebug()<<"  m_singleUpgradeFilePath  "<<m_singleUpgradeFilePath;

            if(ui->textSelectedUpgrade->text().isEmpty())
            {
                qDebug()<<"ui->textSelectedUpgrade 为空 ";
                return;
            }

            if(m_cabinetType.contains("CC26")&&m_singleUpgradeFwOrXML.contains("SafeIO")&&!m_singleUpgradeFwOrXML.contains("XML"))
            {
                emit signal_upgradeSingleFwOrXML(m_SCBAFilePath,ui->textSelectedUpgrade->text(),m_SCBBFilePath,ui->textSelectedUpgrade2->text());
                ui->textSelectedUpgrade2->clear();
            }
            else
            {
                emit signal_upgradeSingleFwOrXML(m_singleUpgradeFilePath,ui->textSelectedUpgrade->text());
            }

            ui->textSelectedUpgrade->clear();
            ui->btnSelectMenu->setText("");
        }
        else
        {

            ui->textSelectedUpgrade->clear();
            ui->btnSelectMenu->setText("");

            QString filePath = ui->lineImportedFile->text();
            qDebug()<<"filePath import "<<filePath;
            QStringList filePathList = filePath.split("/");
            QString fileName = filePathList.at(filePathList.size()-1);
            qDebug()<<"fileName import "<<fileName;

            if(m_cabinetType.contains("CC26")&&m_singleUpgradeFwOrXML.contains("SafeIO"))
            {
                QString filePath2 = ui->lineImportedFile2->text();
                qDebug()<<"filePath import "<<filePath2;
                QStringList filePathList = filePath2.split("/");
                QString fileName2 = filePathList.at(filePathList.size()-1);
                qDebug()<<"fileName import "<<fileName2;
                signal_upgradeSingleFwOrXML(filePath,fileName,filePath2,fileName2);
                ui->lineImportedFile2->clear();
                ui->lineImportedFile->clear();
            }else{
                signal_upgradeSingleFwOrXML(filePath,fileName);
                ui->lineImportedFile->clear();
            }

        }
        setUpgradeStatus(0);
        close();


    });
    setUpgradeStatus(0);



}

WidgetSingleUpgrade::~WidgetSingleUpgrade()
{
    delete ui;
}

void WidgetSingleUpgrade::setCabinetType(QString cabinetType)
{
    m_cabinetType = cabinetType;
}

void WidgetSingleUpgrade::setSingleUpgradeFwOrXML(QString fwOrXML)
{
    m_singleUpgradeFwOrXML = fwOrXML;
}

void WidgetSingleUpgrade::retranslate()
{
    ui->retranslateUi(this);
}

void WidgetSingleUpgrade::setUpgradeStatus(int status)
{
    //0 选择升级文件 1 导入升级文件
    if(status == 0)
    {
        ui->btnRadioSelect->setChecked(true);
        ui->btnSelectMenu->show();
        ui->labelSelectedUpgrade->show();
        if(m_cabinetType.contains("CC26")&&m_singleUpgradeFwOrXML.contains("SafeIO"))
        {

            ui->textSelectedUpgrade2->show();
        }


        ui->textSelectedUpgrade->show();

        ui->lineImportedFile->hide();
        ui->btnImportFile->hide();

        ui->lineImportedFile2->hide();
        ui->btnImportFile2->hide();

    }
    else
    {
        ui->btnRadioImport->setChecked(true);

        ui->btnSelectMenu->hide();

        ui->textSelectedUpgrade2->hide();
        ui->labelSelectedUpgrade->hide();
        ui->textSelectedUpgrade->hide();

        ui->lineImportedFile->show();
        ui->btnImportFile->show();


        if(m_cabinetType.contains("CC26")&&m_singleUpgradeFwOrXML.contains("SafeIO")&&!m_singleUpgradeFwOrXML.contains("XML"))
        {
            ui->lineImportedFile2->show();
            ui->btnImportFile2->show();
        }


    }
}

void WidgetSingleUpgrade::searchMenuItems()
{
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

    for(QString group:m_singleAllVersionSetting.childGroups()){
        if(m_cabinetType.contains("CC26")&&!group.contains("CRV2"))
        {
            continue;
        }
        QMenu* currentBigVersion = m_singleMenu->addMenu(group);
        currentBigVersion->setWindowFlag(Qt::NoDropShadowWindowHint);
        if(versions.contains(group)){
            QStringList existVersions = versions.value(group);
            m_singleAllVersionSetting.beginGroup(group);
            QStringList keyVersions = m_singleAllVersionSetting.allKeys();
            m_singleAllVersionSetting.endGroup();
            for(QString version:keyVersions){
                if(existVersions.contains(version)){
                    currentBigVersion->addAction(version);
                }
                /**
                 *
                 * else{
                 *   currentBigVersion->addAction(QIcon(":/image/images/icon_undownload.png"),version);
                 * }
                 *
                 **/
            }
        }else{
//            m_singleAllVersionSetting.beginGroup(group);
//            QStringList keyVersions = m_singleAllVersionSetting.allKeys();
//            m_singleAllVersionSetting.endGroup();
//            for(QString version:keyVersions){
//                currentBigVersion->addAction(QIcon(":/image/images/icon_undownload.png"),version);
//            }
        }


    }
    if(versions.contains("temporary")){
        QMenu* currentBigVersion = m_singleMenu->addMenu("temporary");
        currentBigVersion->setWindowFlag(Qt::NoDropShadowWindowHint);
        QStringList smallVersions = versions.value("temporary");
        for(QString version:smallVersions){
            if(m_cabinetType.contains("CC26")&&!version.contains("CRV2"))
            {
                continue;
            }
            currentBigVersion->addAction(version);
        }
    }


    ui->btnSelectMenu->setMenu(m_singleMenu);
}

void WidgetSingleUpgrade::slot_menuClicked(QAction *action)
{
    ui->textSelectedUpgrade->clear();
    QMenu* menuParent = (QMenu*)action->parent();
    qDebug()<<"menuParent->title();  "<<menuParent->title();

    QString fileParentDir = menuParent->title();
    QString fileName = action->text();

    QDir upgradeDir(QCoreApplication::applicationDirPath()+"/upgradeFiles/"+fileParentDir+"/"+fileName+"/files");
    QFileInfoList fileInfoList = upgradeDir.entryInfoList();

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



    if(m_cabinetType.contains("CC26"))
    {
        for(QFileInfo fileInfo : fileInfoList)
        {

            if(m_singleUpgradeFwOrXML.contains("SafeIO"))
            {
                if(!m_singleUpgradeFwOrXML.contains("XML"))
                {
                    if(fileInfo.fileName().contains("SCBA"))
                    {
                            m_SCBAFilePath = fileInfo.filePath();
                            ui->textSelectedUpgrade->setText(fileInfo.fileName());

                    }
                    if(fileInfo.fileName().contains("SCBB"))
                    {
                            m_SCBBFilePath = fileInfo.filePath();
                            ui->textSelectedUpgrade2->setText(fileInfo.fileName());
                    }
                }
                else
                {

                    if(fileInfo.fileName().contains("SCB_"))
                    {
                        m_singleUpgradeFilePath = fileInfo.filePath();
                        qDebug()<<"SCB_ XML "<<m_singleUpgradeFilePath;
                        ui->textSelectedUpgrade->setText(fileInfo.fileName());
                    }

                }

            }
            else if(fileInfo.fileName().contains(upgradeFW))
            {

                if(isXML)
                {

                    if(fileInfo.fileName().contains("XML"))
                    {
                        if(upgradeFW=="Terminal")
                        {
                            if(fileInfo.fileName().contains("RX")&&m_currentServo.at(0)=='6'){
                                m_singleUpgradeFilePath = fileInfo.filePath();
                                qDebug()<<"m_singleUpgradeFilePath Terminal RX XML "<<m_singleUpgradeFilePath;
                                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                            }
                            if(!fileInfo.fileName().contains("RX")&&m_currentServo.at(0)=='4'){
                                m_singleUpgradeFilePath = fileInfo.filePath();
                                qDebug()<<"m_singleUpgradeFilePath Terminal !!RX XML "<<m_singleUpgradeFilePath;
                                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                            }

                        }
                        else
                        {
                            m_singleUpgradeFilePath = fileInfo.filePath();
                            qDebug()<<"m_singleUpgradeFilePath XML "<<m_singleUpgradeFilePath;
                            ui->textSelectedUpgrade->setText(fileInfo.fileName());
                        }

                    }
                }
                else
                {
                    if(!fileInfo.fileName().contains("XML"))
                    {
                        if(upgradeFW.contains("Servo"))
                        {
                            if(fileInfo.fileName().contains("V4")&&m_currentServo.at(0)=='4')
                            {
                                m_singleUpgradeFilePath = fileInfo.filePath();
                                qDebug()<<"CC262 m_singleUpgradeFilePath !!XML Servo V4  "<<m_singleUpgradeFilePath;
                                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                            }
                            if(fileInfo.fileName().contains("V6")&&m_currentServo.at(0)=='6')
                            {
                                m_singleUpgradeFilePath = fileInfo.filePath();
                                qDebug()<<"CC262 m_singleUpgradeFilePath !!XML Servo V6  "<<m_singleUpgradeFilePath;
                                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                            }

                        }
                        else if(upgradeFW=="Terminal")
                        {
                            if(fileInfo.fileName().contains("V4")&&m_currentServo.at(0)=='4')
                            {
                                m_singleUpgradeFilePath = fileInfo.filePath();
                                qDebug()<<"CC262 m_singleUpgradeFilePath !!XML Terminal V4 "<<m_singleUpgradeFilePath;
                                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                            }

                            if(fileInfo.fileName().contains("V6")&&m_currentServo.at(0)=='6')
                            {
                                m_singleUpgradeFilePath = fileInfo.filePath();
                                qDebug()<<"CC262 m_singleUpgradeFilePath !!XML Terminal V6 "<<m_singleUpgradeFilePath;
                                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                            }
                        }
                        else
                        {

                            m_singleUpgradeFilePath = fileInfo.filePath();
                            qDebug()<<"CC262 m_singleUpgradeFilePath !!XML !!Servo "<<m_singleUpgradeFilePath;
                            ui->textSelectedUpgrade->setText(fileInfo.fileName());
                        }

                    }
                }
            }

        }
    }else{
        for(QFileInfo fileInfo : fileInfoList)
        {
            if(fileInfo.fileName().contains(upgradeFW))
            {
                if(isXML)
                {
                    if(fileInfo.fileName().contains("XML"))
                    {
                        m_singleUpgradeFilePath = fileInfo.filePath();
                        qDebug()<<"m_singleUpgradeFilePath XML "<<m_singleUpgradeFilePath;
                        ui->textSelectedUpgrade->setText(fileInfo.fileName());
                    }
                }else{
                    if(!fileInfo.fileName().contains("XML"))
                    {
                        if(m_currentServo.at(0)=='4'&&upgradeFW=="Servo")
                        {
                            if(fileInfo.fileName().contains("V4"))
                            {
                                m_singleUpgradeFilePath = fileInfo.filePath();
                                qDebug()<<"m_singleUpgradeFilePath !!XML Servo V4  "<<m_singleUpgradeFilePath;
                                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                            }
                        }
                        else if(m_currentServo.at(0)!='4'&&upgradeFW=="Servo")
                        {
                            if(fileInfo.fileName().contains("V3"))
                            {
                                m_singleUpgradeFilePath = fileInfo.filePath();
                                qDebug()<<"m_singleUpgradeFilePath !!XML Servo V3 "<<m_singleUpgradeFilePath;
                                ui->textSelectedUpgrade->setText(fileInfo.fileName());
                            }
                        }
                        else
                        {
                            m_singleUpgradeFilePath = fileInfo.filePath();
                            qDebug()<<"m_singleUpgradeFilePath !!XML !!Servo "<<m_singleUpgradeFilePath;
                            ui->textSelectedUpgrade->setText(fileInfo.fileName());
                        }

                    }

                }


            }
        }
    }




    ui->btnSelectMenu->setText(action->text());



}

bool WidgetSingleUpgrade::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        if(g_isEnglish)
        {
            this->resize(this->width()+100,this->height());
        }
        else
        {
           this->resize(this->width()-100,this->height());
        }
        return true;
    }
    return QWidget::event(event);

}

