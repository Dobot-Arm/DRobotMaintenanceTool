#include "Upgrade2Widget.h"
#include "ui_Upgrade2Widget.h"
#include "LuaApi.h"

static const int gTerminalIndex = 11;
static const int gServoJ6Index = 10;
static const int gServoJ5Index = 9;
static const int gServoJ4Index = 8;
static const int gServoJ3Index = 7;
static const int gServoJ2Index = 6;
static const int gServoJ1Index = 5;
static const int gSafeIOIndex = 4;
static const int gSafeIOBIndex = 3;
static const int gFeedbackIndex = 2;
static const int gMainControlIndex = 1;
static const int gUniIOIndex = 0;

Upgrade2Widget::Upgrade2Widget(QWidget *parent) :
    UIBaseWidget(parent),
    mouse_press(false),
    m_posDiyX(QApplication::desktop()->geometry().width()/2-this->width()/2),
    m_posDiyY(QApplication::desktop()->geometry().height()/2-this->height()/2),
    ui(new Ui::Upgrade2Widget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    m_isUprgadeSuccess =false;
    ui->labelUpdateDiskStatus->hide();
    ui->btnOK->hide();
    ui->btnRestart->hide();
    ui->btnDownClose->hide();
    ui->btnInterrupt->hide();
    ui->btnExportLog->hide();
    connect(ui->btnDownClose,&QPushButton::clicked,this,[&]{
        close();
    });
    ui->labelWarningStatus->hide();
    ui->labelWarningIcon->hide();

    ui->labelMainControlIcon->show();
    ui->labelMainControl->show();

    m_roundProgressBar = new RoundProgressBar(ui->roundProgressBarWidget);
    m_roundProgressBar->resize(150,150);
    m_roundProgressBar->setValue(0);
    m_roundProgressBar->setOutterBarWidth(5);
    m_roundProgressBar->setInnerBarWidth(5);
    m_roundProgressBar->setOutterColor(QColor(220,220,220));
    //QColor(62,183,42)
    m_roundProgressBar->setDefaultTextColor(QColor(0,71,187));
    m_roundProgressBar->setInnerColor(QColor(0,71,187),QColor(0,71,187));
    m_roundProgressBar->setDiyFontAndSmallText(QFont("Microsoft YaHei",23,50),tr("完成进度"));
    m_roundProgressBar->setControlFlags(RoundProgressBar::all);

    m_tips = new BubbleTipsWidget(this);
    connect(m_tips,&BubbleTipsWidget::signal_interruptUpgrade,this,[this](bool isInterrupt){
        if (!isInterrupt) return;
        hide();
        qDebug()<<"一键升级:中断升级操作开始.....true";
        emit signal_interruptUpgradeUpgrade2Widget(true);
        qDebug()<<"一键升级:中断升级操作结束 isInterrupt.....true";
    },Qt::DirectConnection);

    connect(ui->btnExportLog,&QPushButton::clicked,this,&Upgrade2Widget::onExportLog);

    connect(ui->btnInterrupt,&QPushButton::clicked,this,[&]{
        hide();
        qDebug()<<"一键升级:中断升级操作开始.....true";
        emit signal_interruptUpgradeUpgrade2Widget(true);
        qDebug()<<"一键升级:中断升级操作结束 isInterrupt.....true";
    });
    connect(ui->btnRestart,&QPushButton::clicked,this,[&]{
        ui->labelWarningStatus->hide();
        ui->labelWarningIcon->hide();
        for (int i=0; i<20; ++i) {
            m_checkUpdateFailTimes[i] = 0; //gUniIOIndex~gTerminalIndex
        }
        m_retryUpdateTimes = 0;
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
        ui->btnExportLog->hide();
        qDebug()<<"btnRestart "<<true;

        if(m_intUpgradeStatusList[gServoJ6Index]!=2
                &&m_intUpgradeStatusList[gServoJ5Index]!=2
                &&m_intUpgradeStatusList[gServoJ4Index]!=2
                &&m_intUpgradeStatusList[gServoJ3Index]!=2
                &&m_intUpgradeStatusList[gServoJ2Index]!=2
                &&m_intUpgradeStatusList[gServoJ1Index]!=2){
            setServoStatus(1);
        }

//        if(handleServoStatus(m_intUpgradeStatusList)!=2){
//            setServoStatus(1);
//        }

        if(m_intUpgradeStatusList[gTerminalIndex]<=-1){
            setTerminalStatus(1);
        }
        if(ui->labelUnIO->isVisible()){
            if(m_intUpgradeStatusList[gUniIOIndex]<=-1){
                setUnIOStatus(1);
            }
        }
        if(ui->labelIOFirmware->isVisible()){
            if(m_intUpgradeStatusList[gUniIOIndex]<=-1){
                setIOFirmwareStatus(1);
            }
        }

        if(m_intUpgradeStatusList[gFeedbackIndex]<=-1){
            setFeedbackStatus(1);
        }
        if(m_intUpgradeStatusList[gSafeIOIndex]<=-1){
            setSafeIOStatus(1);

        }
        setRoundBarStatus(0);
        this->repaint();
        emit signal_upgrade2WidgetFinishUpgrade(1);

    });
    connect(ui->btnOK,&QPushButton::clicked,this,[&]{
        emit signal_upgrade2WidgetFinishUpgrade(2);
        close();
    });
}

void Upgrade2Widget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
Upgrade2Widget::~Upgrade2Widget()
{
    delete ui;
}

void Upgrade2Widget::setErrorCode(int errorCode)
{
    m_errorCode = errorCode;
}

int Upgrade2Widget::getIdxFWUniIO() const { return gUniIOIndex; }
int Upgrade2Widget::getIdxFWMainCtrl() const { return gMainControlIndex; }
int Upgrade2Widget::getIdxFWFeedback() const { return gFeedbackIndex; }
int Upgrade2Widget::getIdxFWSafeIOB() const { return gSafeIOBIndex; }
int Upgrade2Widget::getIdxFWSafeIO() const { return gSafeIOIndex; }
int Upgrade2Widget::getIdxFWJ1() const { return gServoJ1Index; }
int Upgrade2Widget::getIdxFWJ2() const { return gServoJ2Index; }
int Upgrade2Widget::getIdxFWJ3() const { return gServoJ3Index; }
int Upgrade2Widget::getIdxFWJ4() const { return gServoJ4Index; }
int Upgrade2Widget::getIdxFWJ5() const { return gServoJ5Index; }
int Upgrade2Widget::getIdxFWJ6() const { return gServoJ6Index; }
int Upgrade2Widget::getIdxFWTerminal() const { return gTerminalIndex; }

int Upgrade2Widget::getIdxXMLUniIO() const { return 0; }
int Upgrade2Widget::getIdxXMLSafeIO() const { return 1; }
int Upgrade2Widget::getIdxXMLJ1() const { return 2; }
int Upgrade2Widget::getIdxXMLJ2() const { return 3; }
int Upgrade2Widget::getIdxXMLJ3() const { return 4; }
int Upgrade2Widget::getIdxXMLJ4() const { return 5; }
int Upgrade2Widget::getIdxXMLJ5() const { return 6; }
int Upgrade2Widget::getIdxXMLJ6() const { return 7; }
int Upgrade2Widget::getIdxXMLTerminal() const { return 8; }

void Upgrade2Widget::initStatus()
{
    QList<QPair<QLabel*, //icon
            QLabel* //title
            >> lstUI;
    m_hashFW.clear();
    m_hashXML.clear();
    auto hasValue = CLuaApi::getInstance()->getUpdateSequerence();
    if (hasValue.hasValue())
    {
        auto value = hasValue.value();
        QStringList names;
        while (value.size()>0){
            auto it = value.begin();
            int iMin = value.begin().value();
            for (auto itr = value.begin(); itr!=value.end();++itr)
            {
                if (itr.value()<iMin){
                    it = itr;
                    iMin = itr.value();
                }
            }
            names<<it.key();
            value.erase(it);
        }
        //调整UI顺序，与fw的顺序一致
        lstUI.append(qMakePair(ui->labelDownloadFirmwareIcon, ui->labelDownloadFirmware));
        lstUI.append(qMakePair(ui->labelDiskCheckIcon, ui->labelDiskCheck));
        for (int i=0; i<names.size();++i)
        {
            if ("unio"==names[i]){
                lstUI.append(qMakePair(ui->labelIOFirmwareIcon, ui->labelIOFirmware));
                lstUI.append(qMakePair(ui->labelUnIOIcon, ui->labelUnIO));
            }
            else if ("control"==names[i]){
                lstUI.append(qMakePair(ui->labelMainControlIcon, ui->labelMainControl));
            }
            else if ("feedback"==names[i]){
                lstUI.append(qMakePair(ui->labelFeedbackIcon, ui->labelFeedback));
            }
            else if ("safeio"==names[i]){
                lstUI.append(qMakePair(ui->labelSafeIOIcon, ui->labelSafeIO));
            }
            else if ("servo"==names[i]){
                lstUI.append(qMakePair(ui->labelServoIcon, ui->labelServo));
            }
            else if ("terminal"==names[i]){
                lstUI.append(qMakePair(ui->labelTerminalIcon, ui->labelTerminal));
            }
        }
    }
    else
    {
        //调整UI顺序
        lstUI.append(qMakePair(ui->labelDownloadFirmwareIcon, ui->labelDownloadFirmware));
        lstUI.append(qMakePair(ui->labelDiskCheckIcon, ui->labelDiskCheck));

        lstUI.append(qMakePair(ui->labelTerminalIcon, ui->labelTerminal));
        lstUI.append(qMakePair(ui->labelServoIcon, ui->labelServo));
        lstUI.append(qMakePair(ui->labelSafeIOIcon, ui->labelSafeIO));
        lstUI.append(qMakePair(ui->labelFeedbackIcon, ui->labelFeedback));
        lstUI.append(qMakePair(ui->labelMainControlIcon, ui->labelMainControl));
        lstUI.append(qMakePair(ui->labelIOFirmwareIcon, ui->labelIOFirmware));
        lstUI.append(qMakePair(ui->labelUnIOIcon, ui->labelUnIO));
    }
    {//删除旧的排版、重新排序UI
        for (auto itr=lstUI.begin(); itr!=lstUI.end(); ++itr){
            ui->gridLayout->removeWidget(itr->first);
            ui->gridLayout->removeWidget(itr->second);
        }
        for (int row=0; row<lstUI.size();++row){
            ui->gridLayout->addWidget(lstUI[row].first,row,0,1,1);
            ui->gridLayout->addWidget(lstUI[row].second,row,1,1,1);
        }
    }

    for (int i=0; i<20; ++i) {
        m_checkUpdateFailTimes[i] = 0; //gUniIOIndex~gTerminalIndex
    }
    m_retryUpdateTimes = 0;
    m_errorCode = 0;
    setDiskCheckStatus(0);
    setDownloadFirmwareStatus(0);
    setFeedbackStatus(0);
    setIOFirmwareStatus(0);
    setMainControlStatus(0);
    setSafeIOStatus(0);
    setServoStatus(0);
    setTerminalStatus(0);
    setUnIOStatus(0);
    setRoundBarStatus(0);

    m_roundProgressBar->setValue(0);
    m_roundProgressBar->setRange(0,14);

    m_isUprgadeSuccess = false;
    ui->labelUpdateDiskStatus->hide();
    ui->btnOK->hide();
    ui->btnRestart->hide();
    ui->btnDownClose->hide();
    ui->btnInterrupt->hide();
    ui->btnExportLog->hide();

    ui->labelWarningStatus->hide();
    ui->labelWarningStatus->setText(QString());
    ui->labelWarningIcon->hide();

    ui->labelAllUpgradeStatus->setText(QString());

    setCabinetType();
}

void Upgrade2Widget::setDiskCheckStatus(int ok,QString text)
{
    if(ok == -1){
        ui->labelAllUpgradeStatus->setText(tr("扫描磁盘失败！"));
        ui->labelDiskCheck->setText(tr("扫描磁盘失败！"));
        ui->btnDownClose->show();
        ui->btnExportLog->show();
        setRoundBarStatus(-1);
    }else if(ok == 0){
        ui->labelDiskCheck->setText(tr("扫描磁盘"));
    }else if(ok == 1){
        ui->labelDiskCheck->setText(tr("扫描磁盘中..."));
        ui->labelAllUpgradeStatus->setText(tr("扫描磁盘中..."));
    }else if(ok == 2){
        ui->labelDiskCheck->setText(tr("扫描磁盘成功！"));
        ui->labelAllUpgradeStatus->setText(tr("扫描磁盘成功！"));
        m_roundProgressBar->setValue(2);//m_roundProgressBar->setValue(20);
    }
    if(!text.isEmpty()){
        setLabelWarningText(text);
    }
    setIconStatus(ui->labelDiskCheckIcon,ok);
}

void Upgrade2Widget::setDiskUpdateStatus(int ok, QString text)
{
    ui->labelUpdateDiskStatus->setText(text);
    ui->labelUpdateDiskStatus->show();
//    if(ok == 2){
//        ui->btnDownClose->show();
//        setLabelWarningText(tr("请重启控制柜"));
//    }
}

void Upgrade2Widget::hideLabelUpdateDisk()
{
    ui->labelUpdateDiskStatus->hide();
}

void Upgrade2Widget::setDownloadFirmwareStatus(int ok,QString text)
{
    if(ok == -1){
        ui->labelDownloadFirmware->setText(tr("下载固件失败！"));
        ui->labelAllUpgradeStatus->setText(tr("下载固件失败！"));
        setLabelWarningText(tr("下载失败请检查机器和网络连接！"));
        ui->btnDownClose->show();
        ui->btnExportLog->show();
        setRoundBarStatus(-1);
    }else if(ok == 0){
        ui->labelDownloadFirmware->setText(tr("下载固件"));
    }else if(ok == 1){
        ui->labelDownloadFirmware->setText(tr("下载固件中..."));
        ui->labelAllUpgradeStatus->setText(tr("下载固件中..."));
    }else if(ok == 2){
        ui->labelDownloadFirmware->setText(tr("下载固件成功！"));
        ui->labelAllUpgradeStatus->setText(tr("下载固件成功！"));
        m_roundProgressBar->setValue(1);//m_roundProgressBar->setValue(10);
    }

    if(!text.isEmpty()){
        setLabelWarningText(text);
    }
    setIconStatus(ui->labelDownloadFirmwareIcon,ok);
}

int Upgrade2Widget::setFeedbackStatus(int ok,QString text)
{
    if(ok == -1){
        ui->labelFeedback->setText(tr("升级馈能板固件失败！"));
        QString strSafeIO = CommonData::getCurrentSettingsVersion().safeio;
        if(strSafeIO.isEmpty())
        {
            qDebug()<<" strSafeIO "<<strSafeIO;
            ui->labelAllUpgradeStatus->setText(tr("升级固件失败！"));
            ui->btnRestart->show();
            ui->btnInterrupt->show();
            ui->btnExportLog->show();
            setRoundBarStatus(-1);
            return ok;
        }
        QStringList strSafeIOList = strSafeIO.split('.');
        int V1 = strSafeIOList[0].toInt();
        int V2 = strSafeIOList[1].toInt();
        int V3 = strSafeIOList[2].toInt();
        int intVersion;
        if(strSafeIOList.size()<4)
        {
            intVersion = V1*1000+V2*100+V3*10+0;
        }
        else
        {
            int V4 = strSafeIOList[3].toInt();
            intVersion = V1*1000+V2*100+V3*10+V4;
        }
        qDebug()<<" intVersion  2400  ----> "<<intVersion;

        if(intVersion < 2400 && CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162))
        {
            setLabelWarningText(tr("请先重启控制柜，并将安全io固件单独升级为V2.4.0.0，再重启控制柜，重新进行一键升级"));
        }
        else
        {
            setLabelWarningText(tr("升级馈能板固件失败，请联系技术支持"));
        }

        ui->labelAllUpgradeStatus->setText(tr("升级固件失败！"));
        ui->btnRestart->show();
        ui->btnInterrupt->show();
        ui->btnExportLog->show();
        setRoundBarStatus(-1);
    }else if(ok == 0){
        ui->labelFeedback->setText(tr("升级馈能板固件"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 1){
        ui->labelFeedback->setText(tr("馈能板固件升级中..."));
        ui->labelAllUpgradeStatus->setText(tr("固件升级中..."));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 2){
        ui->labelFeedback->setText(tr("升级馈能板固件成功！"));
        ui->labelAllUpgradeStatus->setText(tr("升级固件成功！"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }

    if(!text.isEmpty()){
        setLabelWarningText(text);
    }
    setIconStatus(ui->labelFeedbackIcon,ok);
    return ok;
}

int Upgrade2Widget::setIOFirmwareStatus(int ok,QString text)
{

    if(ok <= -1){
        ui->labelIOFirmware->setText(tr("升级IO板固件失败！"));
        setLabelWarningText(tr("请重新升级IO板卡或联系技术支持"));
        ui->labelAllUpgradeStatus->setText(tr("升级固件失败！"));
        if(ok == -2){
            ui->labelIOFirmware->setText(tr("升级IO板卡XML失败！"));
            ui->labelAllUpgradeStatus->setText(tr("升级XML失败！"));
        }
        ui->btnRestart->show();
        ui->btnInterrupt->show();
        ui->btnExportLog->show();
        setRoundBarStatus(-1);
    }else if(ok == 0){
        ui->labelIOFirmware->setText(tr("升级IO板固件"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 1){
        ui->labelIOFirmware->setText(tr("IO板固件升级中..."));
        ui->labelAllUpgradeStatus->setText(tr("固件升级中..."));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 2){
        ui->labelIOFirmware->setText(tr("升级IO板固件成功！"));
        ui->labelAllUpgradeStatus->setText(tr("升级固件成功！"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }

    if(!text.isEmpty()){
        setLabelWarningText(text);
    }
    setIconStatus(ui->labelIOFirmwareIcon,ok);
    return ok;
}

int Upgrade2Widget::setMainControlStatus(int ok,QString text)
{
    if(ok == -1){
        ui->labelTerminal->setText(tr("升级主控板失败！"));
        setLabelWarningText(tr("升级主控板失败，请联系技术支持"));
        ui->labelAllUpgradeStatus->setText(tr("升级主控板失败！"));
        ui->btnRestart->show();
        ui->btnInterrupt->show();
        ui->btnExportLog->show();
        setRoundBarStatus(-1);
        ui->labelMainControl->setText(tr("升级主控板失败！"));
    }else if(ok == 0){
        ui->labelMainControl->setText(tr("升级主控板固件"));
    }else if(ok == 1){
        ui->labelMainControl->setText(tr("主控板升级中..."));
        ui->labelAllUpgradeStatus->setText(tr("主控板升级中..."));
    }else if(ok == 2){
        ui->labelMainControl->setText(tr("升级主控板成功！"));
        ui->labelAllUpgradeStatus->setText(tr(" 升级主控板成功！"));
    }
    if(!text.isEmpty()){
        setLabelWarningText(text);
    }
    setIconStatus(ui->labelMainControlIcon,ok);
    return ok;
}

int Upgrade2Widget::setSafeIOStatus(int ok,QString text)
{

    if(ok <= -1){
        ui->labelSafeIO->setText(tr("升级安全IO固件失败！"));
        setLabelWarningText(tr("请重新升级安全IO或联系技术支持"));
        ui->labelAllUpgradeStatus->setText(tr("升级固件失败！"));
        if(ok == -2){
            ui->labelSafeIO->setText(tr("升级安全XML失败！"));
            ui->labelAllUpgradeStatus->setText(tr("升级XML失败！"));
        }
        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)&&ok == -1)
        {
            ui->labelSafeIO->setText(tr("升级安全IO板A失败!"));
        }
        if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC26X)&&ok == -3)
        {
            ui->labelSafeIO->setText(tr("升级安全IO板B失败!"));
        }
        ui->btnRestart->show();
        ui->btnInterrupt->show();
        ui->btnExportLog->show();
        setRoundBarStatus(-1);
    }else if(ok == 0){
        ui->labelSafeIO->setText(tr("升级安全IO固件"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 1){
        ui->labelSafeIO->setText(tr("安全IO固件升级中..."));
        ui->labelAllUpgradeStatus->setText(tr("固件升级中..."));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 2){
        ui->labelSafeIO->setText(tr("升级安全IO固件成功！"));
        ui->labelAllUpgradeStatus->setText(tr("升级固件成功！"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }

    if(!text.isEmpty()){
        setLabelWarningText(text);
    }
    setIconStatus(ui->labelSafeIOIcon,ok);
    return ok;
}

int Upgrade2Widget::handleServoStatus(QList<int> upgradeStatusList)
{
    int slavidxs[]={gServoJ6Index,gServoJ5Index,gServoJ4Index,
                    gServoJ3Index,gServoJ2Index,gServoJ1Index};
    for (int i=0; i<6;i++)
    {
        if(upgradeStatusList[slavidxs[i]] <= -1&&!isCanSingleUpgradeIdx(slavidxs[i]))
        {
            setServoStatus(-1);
            return -1;
        }
        if(upgradeStatusList[slavidxs[i]] == 2)
        {
            initSingleUpgradeIdx(slavidxs[i]);
        }
    }

    if(upgradeStatusList[gServoJ6Index]==2
            &&upgradeStatusList[gServoJ5Index]==2
            &&upgradeStatusList[gServoJ4Index]==2
            &&upgradeStatusList[gServoJ3Index]==2
            &&upgradeStatusList[gServoJ2Index]==2
            &&upgradeStatusList[gServoJ1Index]==2){
        setServoStatus(2);
        return 2;
    }

    if(upgradeStatusList[gServoJ6Index]==1
            ||upgradeStatusList[gServoJ5Index]==1
            ||upgradeStatusList[gServoJ4Index]==1
            ||upgradeStatusList[gServoJ3Index]==1
            ||upgradeStatusList[gServoJ2Index]==1
            ||upgradeStatusList[gServoJ1Index]==1){
        setServoStatus(1);
        return 1;
    }
    return 1;
}

int Upgrade2Widget::setServoStatus(int ok,QString text)
{

    if(ok <= -1){
        ui->labelServo->setText(tr("升级伺服固件失败！"));
        setLabelWarningText(tr("请重新升级伺服固件或联系技术支持"));
        ui->labelAllUpgradeStatus->setText(tr("升级固件失败！"));
        if(ok == -2){
            ui->labelServo->setText(tr("升级伺服XML失败！"));
            ui->labelAllUpgradeStatus->setText(tr("升级XML失败！"));
        }
        ui->btnRestart->show();
        ui->btnInterrupt->show();
        ui->btnExportLog->show();
        setRoundBarStatus(-1);
    }else if(ok == 0){
        ui->labelServo->setText(tr("升级伺服固件"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 1){
        ui->labelServo->setText(tr("伺服固件升级中..."));
        ui->labelAllUpgradeStatus->setText(tr("固件升级中..."));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 2){
        ui->labelServo->setText(tr("升级伺服固件成功！"));
        ui->labelAllUpgradeStatus->setText(tr("升级固件成功！"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }

    if(!text.isEmpty()){
        setLabelWarningText(text);
    }
    setIconStatus(ui->labelServoIcon,ok);
    return ok;
}

int Upgrade2Widget::setTerminalStatus(int ok,QString text)
{

    if(ok <= -1){
        ui->labelTerminal->setText(tr("升级末端IO固件失败！"));
        setLabelWarningText(tr("请重新升级末端IO或联系技术支持"));
        ui->labelAllUpgradeStatus->setText(tr("升级固件失败！"));
        if(ok == -2){
            ui->labelTerminal->setText(tr("升级末端XML失败！"));
            ui->labelAllUpgradeStatus->setText(tr("升级XML失败！"));
        }
        ui->btnRestart->show();
        ui->btnInterrupt->show();
        ui->btnExportLog->show();
        setRoundBarStatus(-1);
    }else if(ok == 0){
        ui->labelTerminal->setText(tr("升级末端IO固件"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 1){
        ui->labelTerminal->setText(tr("末端IO固件升级中..."));
        ui->labelAllUpgradeStatus->setText(tr("固件升级中..."));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 2){
        ui->labelTerminal->setText(tr("升级末端IO固件成功！"));
        ui->labelAllUpgradeStatus->setText(tr("升级固件成功！"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }
    if(!text.isEmpty()){
        setLabelWarningText(text);
    }
    setIconStatus(ui->labelTerminalIcon,ok);
    return ok;
}

int Upgrade2Widget::setUnIOStatus(int ok,QString text)
{

    if(ok <= -1){
        ui->labelUnIO->setText(tr("升级通用IO固件失败！"));
        setLabelWarningText(tr("请重新升级通用IO或请联系技术支持"));
        ui->labelAllUpgradeStatus->setText(tr("升级固件失败！"));
        if(ok == -2){
            ui->labelUnIO->setText(tr("升级通用XML失败！"));
            ui->labelAllUpgradeStatus->setText(tr("升级XML失败！"));
        }
        if(ok == -1)
        {
            QString strUnIO = CommonData::getCurrentSettingsVersion().unio;
            QStringList strUnIOList = strUnIO.split('.');
            int V1 = strUnIOList[0].toInt();
            int V2 = strUnIOList[1].toInt();
            int V3 = strUnIOList[2].toInt();
            int intVersion;
            if(strUnIOList.size()<4)
            {
                intVersion = V1*1000+V2*100+V3*10+0;
            }
            else
            {
                int V4 = strUnIOList[3].toInt();
                intVersion = V1*1000+V2*100+V3*10+V4;
            }
            qDebug()<<"intVersion 2310  --->  "<<intVersion;
            if(intVersion < 2310 && CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162))
            {
                setLabelWarningText(tr("请先重启控制柜，并将通用io固件单独升级为V2.3.1，再重启控制柜，重新进行一键升级"));
            }
            else
            {
                setLabelWarningText(tr("升级馈能板固件失败，请联系技术支持"));
            }
        }
        ui->btnRestart->show();
        ui->btnInterrupt->show();
        ui->btnExportLog->show();
        setRoundBarStatus(-1);
    }else if(ok == 0){
        ui->labelUnIO->setText(tr("升级通用IO固件"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 1){
        ui->labelUnIO->setText(tr("通用IO固件升级中..."));
        ui->labelAllUpgradeStatus->setText(tr("固件升级中..."));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }else if(ok == 2){
        ui->labelUnIO->setText(tr("升级通用固件成功！"));
        ui->labelAllUpgradeStatus->setText(tr("升级固件成功！"));
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();
    }
    if(!text.isEmpty()){
        setLabelWarningText(text);
    }
    setIconStatus(ui->labelUnIOIcon,ok);
    return ok;
}


void Upgrade2Widget::setIconStatus(QLabel *iconLabel, int ok)
{
    QString strOld = iconLabel->styleSheet();
    QString strNew;
    if(ok <= -1){
        strNew = "border-image: url(:/image/images2/Icon_doFailed.png);";
        iconLabel->setStyleSheet(strNew);
    }else if(ok == 0){
        strNew = "border-image: url(:/image/images2/Icon_do_disable.png);";
        iconLabel->setStyleSheet(strNew);
    }else if(ok == 1){
        strNew = "border-image: url(:/image/images2/Icon_doing.png);";
        iconLabel->setStyleSheet(strNew);
    }else if(ok == 2){
        strNew = "border-image: url(:/image/images2/Icon_doSuccessed.png);";
        iconLabel->setStyleSheet(strNew);
    }
    if (strOld!=strNew)
        iconLabel->repaint();
}

void Upgrade2Widget::setLabelWarningText(QString text)
{
    DobotType::StructErrorCode structErrorCode;
    if(m_errorCode != 0)
    {
        structErrorCode = CommonData::getErrorCodes().value(m_errorCode);
        QString description;
        QString solution;
        if(g_isEnglish)
        {
            description = QString("%1 %2").arg(structErrorCode.errorCode).arg(structErrorCode.enDescription);
            solution = "solution: "+structErrorCode.enSolution;
        }
        else
        {
            description = QString("%1 %2").arg(structErrorCode.errorCode).arg(structErrorCode.zhDescription);
            solution = "解决方案: "+structErrorCode.zhSolution;
        }
        QString strTxt = QString("<font color=\"#CE4949\">%1</font><br/><font color=\"#2D3440\">%2</font>")
                .arg(description).arg(solution);
        ui->labelWarningStatus->setText(strTxt);
    }
    else
    {
        ui->labelWarningStatus->setText(text);
    }

    ui->labelWarningStatus->show();
    ui->labelWarningIcon->show();
}

void Upgrade2Widget::setCabinetType()
{
    COptional<QHash<QString,bool>> ret = CLuaApi::getInstance()->getShowUpdateProcessUI();
    if (ret.hasValue())
    {
        QHash<QString,QPair<QLabel*,QLabel*>> lstUI = {
            {"unio",{ui->labelUnIOIcon, ui->labelUnIO}},
            {"ccboxio",{ui->labelIOFirmwareIcon, ui->labelIOFirmware}},
            {"control",{ui->labelMainControlIcon, ui->labelMainControl}},
            {"feedback",{ui->labelFeedbackIcon, ui->labelFeedback}},
            {"safeio",{ui->labelSafeIOIcon, ui->labelSafeIO}},
            {"servo",{ui->labelServoIcon, ui->labelServo}},
            {"terminal",{ui->labelTerminalIcon, ui->labelTerminal}}
        };
        for (auto itr=lstUI.begin(); itr!=lstUI.end(); ++itr)
        {
            auto p = itr.value();
            p.first->hide();
            p.second->hide();
        }
        auto value = ret.value();
        for(auto itr=value.begin(); itr!=value.end(); ++itr)
        {
            auto find = lstUI.find(itr.key());
            if (find != lstUI.end())
            {
                auto p = find.value();
                p.first->setVisible(itr.value());
                p.second->setVisible(itr.value());
            }
        }
        m_roundProgressBar->setDiyFontAndSmallText(QFont("Microsoft YaHei",20,50),tr("完成进度"));
    }
    else
    {
        if(CommonData::getStrPropertiesCabinetType().name == g_strPropertiesCabinetTypeCC162
                || CommonData::getStrPropertiesCabinetType().name == g_strPropertiesCabinetTypeCC262
                || CommonData::getStrPropertiesCabinetType().name == g_strPropertiesCabinetTypeCC263){
            ui->labelIOFirmware->hide();
            ui->labelIOFirmwareIcon->hide();

            ui->labelSafeIO->show();
            ui->labelSafeIOIcon->show();
            ui->labelUnIO->show();
            ui->labelUnIOIcon->show();
            ui->labelFeedback->show();
            ui->labelFeedbackIcon->show();
            m_roundProgressBar->setDiyFontAndSmallText(QFont("Microsoft YaHei",23,50),tr("完成进度"));

        }else{
            ui->labelIOFirmware->show();
            ui->labelIOFirmwareIcon->show();

            ui->labelSafeIO->hide();
            ui->labelSafeIOIcon->hide();
            ui->labelUnIO->hide();
            ui->labelUnIOIcon->hide();
            ui->labelFeedback->hide();
            ui->labelFeedbackIcon->hide();

            m_roundProgressBar->setDiyFontAndSmallText(QFont("Microsoft YaHei",20,50),tr("完成进度"));
        }
    }

    /*固定显示的升级项有：
        labelDownloadFirmwareIcon     labelDownloadFirmware     下载固件
        labelDiskCheckIcon            labelDiskCheck            扫描磁盘
        labelTerminalIcon             labelTerminal             升级末端IO固件
        labelServoIcon                labelServo                升级伺服固件
        labelMainControlIcon          labelMainControl          升级主控板固件
      互斥显示项有：
        【
        labelSafeIOIcon     labelSafeIO     升级安全IO固件
        labelFeedbackIcon   labelFeedback   升级馈能板固件
        labelUnIOIcon       labelUnIO       升级通用IO固件
        】
        【
        labelIOFirmwareIcon labelIOFirmware 升级IO板卡固件
        】
    */
}

void Upgrade2Widget::setRoundBarStatus(int status)
{
    if(status == -1){
        qDebug()<<" -11111111111111111111111 ";
            m_roundProgressBar->setDefaultTextColor(QColor(206,73,73));
            m_roundProgressBar->setInnerColor(QColor(206,73,73),QColor(206,73,73));
            ui->widgetBox->resize(ui->widgetBox->width(),ui->widgetBox->minimumHeight()+200);
        }
        if(status == 0){
             qDebug()<<" 00000000000000000000000000 ";
            ui->widgetBox->resize(ui->widgetBox->width(),ui->widgetBox->minimumHeight());
            m_roundProgressBar->setDefaultTextColor(QColor(0,71,187));
            m_roundProgressBar->setInnerColor(QColor(0,71,187),QColor(0,71,187));
        }
        if(status == 2){
            qDebug()<<" 22222222222222222 ";
            m_roundProgressBar->setDefaultTextColor(QColor(62,183,42));
            m_roundProgressBar->setInnerColor(QColor(62,183,42),QColor(62,183,42));
            ui->widgetBox->resize(ui->widgetBox->width(),ui->widgetBox->minimumHeight()+150);
        }
        m_roundProgressBar->repaint();
}

void Upgrade2Widget::setUpgradeStatus(QList<int> intUpgradeStatusList, bool bIsOutage)
{

    qDebug()<<"CommonData::getStrPropertiesCabinetType().name  "<<CommonData::getStrPropertiesCabinetType().name <<"bIsOutage "<<bIsOutage;
    if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
    {
        if(bIsOutage)
        {
            emit signal_upgrade2WidgetFinishUpgrade(0);
            ui->widgetBox->resize(ui->widgetBox->width(),ui->widgetBox->minimumHeight()+150);
            setLabelWarningText(tr("IO板固件升级完成会自动断电，需重启控制柜确认固件是否升级成功"));
            if(CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6))
            {
                setLabelWarningText(tr("通用IO固件升级完成会自动断电，需重启控制柜确认固件是否升级成功"));
            }
            ui->btnDownClose->show();
            return;
        }
    }

    if(intUpgradeStatusList.size()<12){
        qDebug()<<"==============intUpgradeStatusList.size="<<intUpgradeStatusList.size();
        return;
    }

    if(m_isUprgadeSuccess){
//        emit signal_upgrade2WidgetFinishUpgrade(0);
//        setRoundBarStatus(2);
//        ui->btnOK->show();
        qDebug()<<" m_isUprgadeSuccess finish"<<m_isUprgadeSuccess;
        return;
    }
    m_roundProgressBar->repaint();

    qDebug()<<" begin status";
    for(int status: intUpgradeStatusList){
        qDebug()<<"  "<<status;
    }

    m_intUpgradeStatusList = intUpgradeStatusList;

    if(intUpgradeStatusList[gTerminalIndex]<=-1&&!isCanSingleUpgradeIdx(gTerminalIndex))
    {
        if(setTerminalStatus(intUpgradeStatusList[gTerminalIndex])<=-1){
            emit signal_upgrade2WidgetFinishUpgrade(-1);
            qDebug()<<"setTerminalStatus failed";
            return;
        }
    }
    else if(intUpgradeStatusList[gTerminalIndex]>-1)
    {
        setTerminalStatus(intUpgradeStatusList[gTerminalIndex]);
        if(intUpgradeStatusList[gTerminalIndex] == 2)
        {
            initSingleUpgradeIdx(gTerminalIndex);
        }
    }

    if(handleServoStatus(intUpgradeStatusList)<=-1){
        emit signal_upgrade2WidgetFinishUpgrade(-1);
        qDebug()<<"handleServoStatus failed";
        return;
    }

    if(intUpgradeStatusList[gFeedbackIndex]<=-1&&!isCanSingleUpgradeIdx(gFeedbackIndex))
    {
        if(setFeedbackStatus(intUpgradeStatusList[gFeedbackIndex])<=-1){
            emit signal_upgrade2WidgetFinishUpgrade(-1);
            qDebug()<<" setFeedbackStatus failed";
            return;
        }

    }
    else if(intUpgradeStatusList[gFeedbackIndex]>-1)
    {
        setFeedbackStatus(intUpgradeStatusList[gFeedbackIndex]);
        if(intUpgradeStatusList[gFeedbackIndex] == 2)
        {
            initSingleUpgradeIdx(gFeedbackIndex);
        }

    }

    if(intUpgradeStatusList[gSafeIOIndex]<=-1&&!isCanSingleUpgradeIdx(gSafeIOIndex))
    {
        if(setSafeIOStatus(intUpgradeStatusList[gSafeIOIndex])<=-1){
            emit signal_upgrade2WidgetFinishUpgrade(-1);
            qDebug()<<" setSafeIOStatus failed";
            return;
        }

    }
    else if(intUpgradeStatusList[gSafeIOIndex]>-1)
    {
        setSafeIOStatus(intUpgradeStatusList[gSafeIOIndex]);
        if(intUpgradeStatusList[gSafeIOIndex] == 2)
        {
            initSingleUpgradeIdx(gSafeIOIndex);
        }
    }

    if(intUpgradeStatusList[gSafeIOBIndex]<=-1&&!isCanSingleUpgradeIdx(gSafeIOBIndex))
    {
        if(intUpgradeStatusList[gSafeIOBIndex]==-1){
            setSafeIOStatus(-3);
            emit signal_upgrade2WidgetFinishUpgrade(-1);
            qDebug()<<" setSafeIOStatus BBB failed";
            return;
        }
    }
    else if(intUpgradeStatusList[gSafeIOBIndex]==2)
    {
        initSingleUpgradeIdx(gSafeIOBIndex);
    }

    if(intUpgradeStatusList[gMainControlIndex]<=-1&&!isCanSingleUpgradeIdx(gMainControlIndex))
    {
        if(setMainControlStatus(intUpgradeStatusList[gMainControlIndex])<=-1){
            emit signal_upgrade2WidgetFinishUpgrade(-1);
            qDebug()<<" setFeedbackStatus failed";
            return;
        }

    }
    else if(intUpgradeStatusList[gMainControlIndex]>-1)
    {
        setMainControlStatus(intUpgradeStatusList[gMainControlIndex]);
        if(intUpgradeStatusList[gMainControlIndex] == 2)
        {
            initSingleUpgradeIdx(gMainControlIndex);
        }

    }

    if(intUpgradeStatusList[gUniIOIndex]<=-1&&!isCanSingleUpgradeIdx(gUniIOIndex))
    {
        if(ui->labelUnIO->isVisible()){
            if(setUnIOStatus(intUpgradeStatusList[gUniIOIndex])<=-1){
                emit signal_upgrade2WidgetFinishUpgrade(-1);
                qDebug()<<" setUnIOStatus failed";
                return;
            }
        }
        if(ui->labelIOFirmware->isVisible()){
            if(setIOFirmwareStatus(intUpgradeStatusList[gUniIOIndex])<=-1){
                emit signal_upgrade2WidgetFinishUpgrade(-1);
                qDebug()<<" setUnIOStatus failed";
                return;
            }
        }

    }
    else if(intUpgradeStatusList[gUniIOIndex]>-1)
    {
        if(ui->labelUnIO->isVisible()){
            setUnIOStatus(intUpgradeStatusList[gUniIOIndex]);
        }
        if(ui->labelIOFirmware->isVisible()){
            setIOFirmwareStatus(intUpgradeStatusList[gUniIOIndex]);
        }
        if(intUpgradeStatusList[gUniIOIndex] == 2)
        {
            initSingleUpgradeIdx(gUniIOIndex);
        }
    }


    if(CommonData::getStrPropertiesCabinetType().name != g_strPropertiesCabinetTypeCCBOX
            && intUpgradeStatusList[gUniIOIndex]==2){
        setUnIOStatus(intUpgradeStatusList[gUniIOIndex]);
        sleep(10000);
    }


    int successIndex = 0;
    for(int status : intUpgradeStatusList){
        if(status == 2){
            successIndex++;
        }
    }
    m_roundProgressBar->setRange(0, 2+intUpgradeStatusList.size());//固件下载和扫描磁盘也算进去
    /*
    if(CommonData::getStrPropertiesCabinetType().name == g_strPropertiesCabinetTypeCCBOX
            ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeMagicianE6)){
        successIndex+=3;
    }
    if(CommonData::getStrPropertiesCabinetType().name == g_strPropertiesCabinetTypeCC162){
        successIndex+=1;
    }*/
    //||successIndex == 12
    if(intUpgradeStatusList.size()==successIndex){
        emit signal_upgrade2WidgetFinishUpgrade(0);
        if(CommonData::getStrPropertiesCabinetType().name == g_strPropertiesCabinetTypeCCBOX){
            m_isUprgadeSuccess = true;
            setIOFirmwareStatus(2);
            sleep(10000);
            m_roundProgressBar->setValue(m_roundProgressBar->getMax());
            setRoundBarStatus(2);
            setLabelWarningText(tr("请点击 好的 按钮之后,重启或开启小型柜子的开关,并在弹窗等待设备自动连接。"));
        }
        else if(CommonData::getStrPropertiesCabinetType().name == g_strPropertiesCabinetTypeCC262){
            m_roundProgressBar->setValue(m_roundProgressBar->getMax());
            m_isUprgadeSuccess = true;
            setRoundBarStatus(2);
            setLabelWarningText(tr("升级完成会断电，请按柜子船型开关重启一次。"));
        }
        else {
            m_roundProgressBar->setValue(m_roundProgressBar->getMax());
            m_isUprgadeSuccess = true;
            setRoundBarStatus(2);
            setLabelWarningText(tr("升级完成，请按柜子后面的船型开关重启。"));
        }

        ui->btnOK->show();

    }else{
        m_roundProgressBar->setValue(2+successIndex);//m_roundProgressBar->setValue(28+successIndex*6);
        qDebug()<<"intUpgradeStatusList.size() "<<intUpgradeStatusList.size()<<"successIndex "<<successIndex;
    }
    this->repaint();
}

void Upgrade2Widget::initSingleUpgradeIdx(int idx)
{//从上面的逻辑看，升级成功时，才会调用这个
    Q_UNUSED(idx);
    if (m_checkUpdateFailTimes.contains(idx)){
        m_checkUpdateFailTimes[idx] = 0;
    }
}

bool Upgrade2Widget::isCanSingleUpgradeIdx(int idx)
{//升级失败时这个才会被调用,返回false，则界面提示升级失败，true则界面什么都不提示
    Q_UNUSED(idx);
    if (m_checkUpdateFailTimes.contains(idx))
    {
        if(m_checkUpdateFailTimes[idx] < 6)
        {//升级检测到失败的次数少于这么多，则不提示失败
            m_checkUpdateFailTimes[idx]++;
            return true;
        }
    }

    if (m_retryUpdateTimes < 3)
    {//升级失败超过次数后，重新升级
        m_retryUpdateTimes++;
        emit singal_upgradeSingle2Count();
        return true;
    }
    return false;
}

void Upgrade2Widget::slot_closeWidget()
{
    if(m_isUprgadeSuccess){
        close();
        return;
    }
    QPoint posA = ui->btnInterrupt->mapToGlobal(QPoint(0, 0));
    m_tips->setPos(posA.x()-120,posA.y()+40);

    QFont f;
    f.setFamily("Microsoft YaHei UI");
    f.setPixelSize(14);
    m_tips->setContentFont(f);
    m_tips->setDirect(DIRECT::DIRECT_TOP, 0.45);
    m_tips->setContent(tr("确认是否中断升级？"));
    m_tips->show();
}

void Upgrade2Widget::closeEvent(QCloseEvent *e)
{
    setDiskCheckStatus(0);
    setDownloadFirmwareStatus(0);
    setFeedbackStatus(0);
    setIOFirmwareStatus(0);
    setMainControlStatus(0);
    setSafeIOStatus(0);
    setServoStatus(0);
    setTerminalStatus(0);
    setUnIOStatus(0);
    setRoundBarStatus(0);
    ui->labelWarningStatus->setText(QString());
    ui->labelWarningIcon->hide();

    ui->labelUpdateDiskStatus->hide();
    ui->labelAllUpgradeStatus->setText(QString());

    e->accept();
}

bool Upgrade2Widget::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        qDebug()<<"g_isEnglish  "<<g_isEnglish;
        if(g_isEnglish)
        {
            ui->widgetBox->resize(700,500);
        }
        else
        {
            ui->widgetBox->resize(500,500);
        }
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

void Upgrade2Widget::sleep(int milliseconds)
{
    QTime dieTime = QTime::currentTime().addMSecs(milliseconds);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Upgrade2Widget::onExportLog()
{
    emit signal_ExportLogWhenError(1);
}
