#include "Upgrade2Widget.h"
#include "ui_Upgrade2Widget.h"

Upgrade2Widget::Upgrade2Widget(QWidget *parent) :
    QWidget(parent),
    mouse_press(false),
    m_posDiyX(QApplication::desktop()->geometry().width()/2-this->width()/2),
    m_posDiyY(QApplication::desktop()->geometry().height()/2-this->height()/2),
    ui(new Ui::Upgrade2Widget)
{
    ui->setupUi(this);
//    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowModality(Qt::ApplicationModal); //禁用主窗口

    m_isUprgadeSuccess =false;
    ui->labelUpdateDiskStatus->hide();
    ui->btnOK->hide();
    ui->btnRestart->hide();
    ui->btnDownClose->hide();
    ui->btnInterrupt->hide();
    connect(ui->btnClose,&QPushButton::clicked,this,&Upgrade2Widget::slot_closeWidget);
    connect(ui->btnDownClose,&QPushButton::clicked,this,[&]{
        parentWidget()->close();
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
        emit signal_interruptUpgradeUpgrade2Widget(isInterrupt);
        qDebug()<<"isInterrupt "<<isInterrupt;
        parentWidget()->hide();
    },Qt::DirectConnection);

    connect(ui->btnInterrupt,&QPushButton::clicked,this,[&]{
        emit signal_interruptUpgradeUpgrade2Widget(true);
        qDebug()<<"isInterrupt "<<true;
         parentWidget()->hide();
    });
    connect(ui->btnRestart,&QPushButton::clicked,this,[&]{
        ui->labelWarningStatus->hide();
        ui->labelWarningIcon->hide();
        CommonData::setAllUpgradeToSingleUpgradeCount(0);
        ui->btnRestart->hide();
        ui->btnInterrupt->hide();

        qDebug()<<"btnRestart "<<true;



        if(m_intUpgradeStatusList[8]!=2
                &&m_intUpgradeStatusList[7]!=2
                &&m_intUpgradeStatusList[6]!=2
                &&m_intUpgradeStatusList[5]!=2
                &&m_intUpgradeStatusList[4]!=2
                &&m_intUpgradeStatusList[3]!=2){
            setServoStatus(1);
        }

//        if(handleServoStatus(m_intUpgradeStatusList)!=2){
//            setServoStatus(1);
//        }

        if(m_intUpgradeStatusList[9]<=-1){
            setTerminalStatus(1);
        }
        if(ui->labelUnIO->isVisible()){
            if(m_intUpgradeStatusList[2]<=-1){
                setUnIOStatus(1);
            }
        }
        if(ui->labelIOFirmware->isVisible()){
            if(m_intUpgradeStatusList[2]<=-1){
                setIOFirmwareStatus(1);
            }
        }

        if(m_intUpgradeStatusList[1]<=-1){
            setFeedbackStatus(1);
        }
        if(m_intUpgradeStatusList[0]<=-1){
            setSafeIOStatus(1);

        }


        setRoundBarStatus(0);
        this->repaint();
        emit signal_upgrade2WidgetFinishUpgrade(1);

    });
    connect(ui->btnOK,&QPushButton::clicked,this,[&]{
        emit signal_upgrade2WidgetFinishUpgrade(2);
        parentWidget()->close();
    });


}



//void Upgrade2Widget::mousePressEvent(QMouseEvent *e)
//{
//    if(e->button()==Qt::LeftButton
//      && e->x() < this->width()
//      && e->y() < this->height())
//    {
//        this->setCursor(Qt::ClosedHandCursor);
//        mouse_press = true;
//    }
//    move_point=e->globalPos()-this->pos();
//}

//void Upgrade2Widget::mouseMoveEvent(QMouseEvent *e)
//{
//    if(mouse_press)
//    {
//        QPoint move_pos=e->globalPos();
//        this->move(move_pos-move_point);
//        QPoint posA = ui->btnClose->mapToGlobal(QPoint(0, 0));
//        m_tips->move(posA.x()-120,posA.y()+40);

//    }
//}

//void Upgrade2Widget::mouseReleaseEvent(QMouseEvent *e)
//{
//    mouse_press = false;
//    this->setCursor(Qt::ArrowCursor);
//}

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

void Upgrade2Widget::initStatus()
{
    m_bIsOutage = false;
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

    m_isUprgadeSuccess = false;
    ui->labelUpdateDiskStatus->hide();
    ui->btnOK->hide();
    ui->btnRestart->hide();
    ui->btnDownClose->hide();
    ui->btnInterrupt->hide();

    ui->labelWarningStatus->hide();
    ui->labelWarningStatus->setText(QString());
    ui->labelWarningIcon->hide();

    ui->labelAllUpgradeStatus->setText(QString());

}

void Upgrade2Widget::setDiskCheckStatus(int ok,QString text)
{
    if(ok == -1){
        ui->labelAllUpgradeStatus->setText(tr("扫描磁盘失败！"));
        ui->labelDiskCheck->setText(tr("扫描磁盘失败！"));
        setRoundBarStatus(-1);
    }else if(ok == 0){
        ui->labelDiskCheck->setText(tr("扫描磁盘"));
    }else if(ok == 1){
        ui->labelDiskCheck->setText(tr("扫描磁盘中..."));
        ui->labelAllUpgradeStatus->setText(tr("扫描磁盘中..."));
    }else if(ok == 2){
        ui->labelDiskCheck->setText(tr("扫描磁盘成功！"));
        ui->labelAllUpgradeStatus->setText(tr("扫描磁盘成功！"));
        m_roundProgressBar->setValue(20);
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
        setRoundBarStatus(-1);
    }else if(ok == 0){
        ui->labelDownloadFirmware->setText(tr("下载固件"));
    }else if(ok == 1){
        ui->labelDownloadFirmware->setText(tr("下载固件中..."));
        ui->labelAllUpgradeStatus->setText(tr("下载固件中..."));
    }else if(ok == 2){
        ui->labelDownloadFirmware->setText(tr("下载固件成功！"));
        ui->labelAllUpgradeStatus->setText(tr("下载固件成功！"));
        m_roundProgressBar->setValue(10);
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
        if(intVersion < 2400)
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

void Upgrade2Widget::setMainControlStatus(int ok,QString text)
{
    if(ok == -1){
        ui->labelMainControl->setText(tr("升级主控板失败！"));
        setLabelWarningText(tr("升级主控板失败，请联系技术支持"));
        ui->labelAllUpgradeStatus->setText(tr("升级主控板失败！"));
        ui->btnDownClose->show();
        setRoundBarStatus(-1);
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
        if(CommonData::getStrPropertiesCabinetType() == g_strPropertiesCabinetTypeCC262&&ok == -1)
        {
            ui->labelSafeIO->setText(tr("升级安全IO板A失败!"));
        }
        if(CommonData::getStrPropertiesCabinetType() == g_strPropertiesCabinetTypeCC262&&ok == -3)
        {
            ui->labelSafeIO->setText(tr("升级安全IO板B失败!"));
        }
        ui->btnRestart->show();
        ui->btnInterrupt->show();
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
    for(int i = 8; i > 2; i--)
    {
//        if(upgradeStatusList[i] == -2&&CommonData::getAllUpgradeToSingleUpgradeCount()==2)
//        {
//            setServoStatus(-2);
//            return -2;
//        }

//        if(upgradeStatusList[i] == -1&&CommonData::getAllUpgradeToSingleUpgradeCount()==2)
//        {
//            setServoStatus(-1);
//            return -1;
//        }

//        if(upgradeStatusList[i] == -2&&CommonData::getAllUpgradeToSingleUpgradeCount()<2)
//        {
//            CommonData::setAllUpgradeToSingleUpgradeCount(CommonData::getAllUpgradeToSingleUpgradeCount()+1);
//            //TODO
//            emit singal_upgradeSingle2Count(i,-2);

//        }

        if(upgradeStatusList[i] <= -1&&!isCanSingleUpgradeSlave(i))
        {
            setServoStatus(-1);
            return -1;
        }
        if(upgradeStatusList[i] == 2)
        {
            initSingleUpgradeSlaveId(i);
        }


    }
//    if(upgradeStatusList[8]==-2||upgradeStatusList[7]==-2||upgradeStatusList[6]==-2||upgradeStatusList[5]==-2
//            ||upgradeStatusList[4]==-2||upgradeStatusList[3]==-2){
//        setServoStatus(-2);
//        return -2;
//    }

//    if(upgradeStatusList[8]==-1||upgradeStatusList[7]==-1||upgradeStatusList[6]==-1||upgradeStatusList[5]==-1
//            ||upgradeStatusList[4]==-1||upgradeStatusList[3]==-1){
//        setServoStatus(-1);
//        return -1;
//    }

    if(upgradeStatusList[8]==2&&upgradeStatusList[7]==2&&upgradeStatusList[6]==2&&upgradeStatusList[5]==2&&
            upgradeStatusList[4]==2&&upgradeStatusList[3]==2){
        setServoStatus(2);
        return 2;
    }

    if(upgradeStatusList[8]==1||upgradeStatusList[7]==1||upgradeStatusList[6]==1||upgradeStatusList[5]==1
            ||upgradeStatusList[4]==1||upgradeStatusList[3]==1){
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
            if(intVersion<2310)
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

    if(ok <= -1){
        iconLabel->setStyleSheet("border-image: url(:/image/images2/Icon_doFailed.png);");
    }else if(ok == 0){
        iconLabel->setStyleSheet("border-image: url(:/image/images2/Icon_do_disable.png);");
    }else if(ok == 1){
        iconLabel->setStyleSheet("border-image: url(:/image/images2/Icon_doing.png);");
    }else if(ok == 2){
        iconLabel->setStyleSheet("border-image: url(:/image/images2/Icon_doSuccessed.png);");
    }
    iconLabel->repaint();
}

void Upgrade2Widget::setLabelWarningText(QString text)
{
    ui->labelWarningStatus->setText(text);
    ui->labelWarningStatus->show();
    ui->labelWarningIcon->show();
}

void Upgrade2Widget::setCabinetType()
{

    if(CommonData::getStrPropertiesCabinetType() == g_strPropertiesCabinetTypeCC162
            || CommonData::getStrPropertiesCabinetType() == g_strPropertiesCabinetTypeCC262
            || CommonData::getStrPropertiesCabinetType() == g_strPropertiesCabinetTypeCC263){
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
        ui->labelSafeIO->hide();
        ui->labelSafeIOIcon->hide();
        ui->labelUnIO->hide();
        ui->labelUnIOIcon->hide();
        ui->labelFeedback->hide();
        ui->labelFeedbackIcon->hide();
        ui->labelIOFirmware->show();
        ui->labelIOFirmwareIcon->show();

        m_roundProgressBar->setDiyFontAndSmallText(QFont("Microsoft YaHei",20,50),tr("完成进度"));
    }
}

void Upgrade2Widget::setRoundBarStatus(int status)
{
    if(status == -1){
        qDebug()<<" -11111111111111111111111 ";
            m_roundProgressBar->setDefaultTextColor(QColor(206,73,73));
            m_roundProgressBar->setInnerColor(QColor(206,73,73),QColor(206,73,73));
            parentWidget()->resize(parentWidget()->width(),parentWidget()->minimumHeight()+150);
        }
        if(status == 0){
             qDebug()<<" 00000000000000000000000000 ";
            parentWidget()->resize(parentWidget()->width(),parentWidget()->minimumHeight());
            m_roundProgressBar->setDefaultTextColor(QColor(0,71,187));
            m_roundProgressBar->setInnerColor(QColor(0,71,187),QColor(0,71,187));
        }
        if(status == 2){
            qDebug()<<" 22222222222222222 ";
            m_roundProgressBar->setDefaultTextColor(QColor(62,183,42));
            m_roundProgressBar->setInnerColor(QColor(62,183,42),QColor(62,183,42));
            parentWidget()->resize(parentWidget()->width(),parentWidget()->minimumHeight()+150);
        }
        m_roundProgressBar->repaint();
}

void Upgrade2Widget::setUpgradeStatus(QList<int> intUpgradeStatusList)
{

    qDebug()<<"CommonData::getStrPropertiesCabinetType()  "<<CommonData::getStrPropertiesCabinetType() <<"m_bIsOutage "<<m_bIsOutage;
    if(CommonData::getStrPropertiesCabinetType().contains(g_strPropertiesCabinetTypeCCBOX))
    {

        if(m_bIsOutage)
        {
            emit signal_upgrade2WidgetFinishUpgrade(0);
            parentWidget()->resize(parentWidget()->width(),parentWidget()->minimumHeight()+150);
            setLabelWarningText(tr("IO板固件升级完成会自动断电，需重启控制柜确认固件是否升级成功"));
            ui->btnDownClose->show();
            return;
        }
    }


    if(intUpgradeStatusList.size()<11){
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

    if(intUpgradeStatusList[9]<=-1&&!isCanSingleUpgradeSlave(9))
    {
        if(setTerminalStatus(intUpgradeStatusList[9])<=-1){
            emit signal_upgrade2WidgetFinishUpgrade(-1);
            qDebug()<<"setTerminalStatus failed";
            return;
        }

        if(intUpgradeStatusList[9] == 2)
        {
            initSingleUpgradeSlaveId(9);
        }

    }
    else if(intUpgradeStatusList[9]>-1)
    {
        setTerminalStatus(intUpgradeStatusList[9]);
        if(intUpgradeStatusList[9] == 2)
        {
            initSingleUpgradeSlaveId(9);
        }
    }



    if(handleServoStatus(intUpgradeStatusList)<=-1){
        emit signal_upgrade2WidgetFinishUpgrade(-1);
        qDebug()<<"handleServoStatus failed";
        return;
    }



    if(intUpgradeStatusList[2]<=-1&&!isCanSingleUpgradeSlave(2))
    {
        if(ui->labelUnIO->isVisible()){
            if(setUnIOStatus(intUpgradeStatusList[2])<=-1){
                emit signal_upgrade2WidgetFinishUpgrade(-1);
                qDebug()<<" setUnIOStatus failed";
                return;
            }
        }
        if(ui->labelIOFirmware->isVisible()){
            if(setIOFirmwareStatus(intUpgradeStatusList[2])<=-1){
                emit signal_upgrade2WidgetFinishUpgrade(-1);
                qDebug()<<" setUnIOStatus failed";
                return;
            }
        }

    }
    else if(intUpgradeStatusList[2]>-1)
    {
        if(ui->labelUnIO->isVisible()){
            setUnIOStatus(intUpgradeStatusList[2]);
        }
        if(ui->labelIOFirmware->isVisible()){
            setIOFirmwareStatus(intUpgradeStatusList[2]);
        }
        if(intUpgradeStatusList[2] == 2)
        {
            initSingleUpgradeSlaveId(2);
        }
    }




    if(intUpgradeStatusList[1]<=-1&&!isCanSingleUpgradeSlave(1))
    {
        if(setFeedbackStatus(intUpgradeStatusList[1])<=-1){
            emit signal_upgrade2WidgetFinishUpgrade(-1);
            qDebug()<<" setFeedbackStatus failed";
            return;
        }

    }
    else if(intUpgradeStatusList[1]>-1)
    {
        setFeedbackStatus(intUpgradeStatusList[1]);
        if(intUpgradeStatusList[1] == 2)
        {
            initSingleUpgradeSlaveId(1);
        }

    }


    if(intUpgradeStatusList[0]<=-1&&!isCanSingleUpgradeSlave(0))
    {
        if(setSafeIOStatus(intUpgradeStatusList[0])<=-1){
            emit signal_upgrade2WidgetFinishUpgrade(-1);
            qDebug()<<" setSafeIOStatus failed";
            return;
        }

    }
    else if(intUpgradeStatusList[0]>-1)
    {
        setSafeIOStatus(intUpgradeStatusList[0]);
        if(intUpgradeStatusList[0] == 2)
        {
            initSingleUpgradeSlaveId(0);
        }
    }



    if(intUpgradeStatusList[10]<=-1&&!isCanSingleUpgradeSlave(10))
    {
        if(intUpgradeStatusList[10]==-1){
            setSafeIOStatus(-3);
            emit signal_upgrade2WidgetFinishUpgrade(-1);
            qDebug()<<" setSafeIOStatus BBB failed";
            return;
        }


    }
    else if(intUpgradeStatusList[10]==2)
    {
        initSingleUpgradeSlaveId(10);
    }




    if(CommonData::getStrPropertiesCabinetType() != g_strPropertiesCabinetTypeCCBOX && intUpgradeStatusList[2]==2){
        sleep(10000);
    }


    int successIndex = 0;
    for(int status : intUpgradeStatusList){
        if(status == 2){
            successIndex++;
        }
    }
    if(CommonData::getStrPropertiesCabinetType() == g_strPropertiesCabinetTypeCCBOX){
        successIndex+=3;
    }

    if(CommonData::getStrPropertiesCabinetType() == g_strPropertiesCabinetTypeCC162){
        successIndex+=1;
    }
    if(intUpgradeStatusList.size()==successIndex||successIndex == 11){
        emit signal_upgrade2WidgetFinishUpgrade(0);
        if(CommonData::getStrPropertiesCabinetType() == g_strPropertiesCabinetTypeCCBOX){
            m_isUprgadeSuccess = true;
            sleep(10000);
            m_roundProgressBar->setValue(100);
            setRoundBarStatus(2);
            setIOFirmwareStatus(2);
            setLabelWarningText(tr("请点击 好的 按钮之后,重启或开启小型柜子的开关\n,并在弹窗等待设备自动连接。"));
        }
        else if(CommonData::getStrPropertiesCabinetType() == g_strPropertiesCabinetTypeCC262){
            m_roundProgressBar->setValue(100);
            m_isUprgadeSuccess = true;
            setRoundBarStatus(2);
            setLabelWarningText(tr("升级完成会断电，请按柜子船型开关重启两次。"));
        }
        else {
            m_roundProgressBar->setValue(100);
            m_isUprgadeSuccess = true;
            setRoundBarStatus(2);
            setLabelWarningText(tr("升级完成，请按柜子后面的船型开关重启。"));
        }

        ui->btnOK->show();

    }else{
        m_roundProgressBar->setValue(34+successIndex*6);
        qDebug()<<"intUpgradeStatusList.size() "<<intUpgradeStatusList.size()<<"successIndex "<<successIndex;
    }
    this->repaint();



}

void Upgrade2Widget::initSingleUpgradeSlaveId(int slaveId)
{
    if(m_singleUpgradeSlaveId == slaveId)
    {
        m_singleUpgradeSlaveId = -1;
        CommonData::setAllUpgradeToSingleUpgradeCount(0);
    }
}

bool Upgrade2Widget::isCanSingleUpgradeSlave(int slaveId)
{
    if(CommonData::getAllUpgradeToSingleUpgradeCount()==2)
    {
        return false;
    }
    CommonData::setAllUpgradeToSingleUpgradeCount(CommonData::getAllUpgradeToSingleUpgradeCount()+1);
    m_singleUpgradeSlaveId = slaveId;
    emit singal_upgradeSingle2Count(slaveId);
    return true;
}




void Upgrade2Widget::slot_closeWidget()
{
    if(m_isUprgadeSuccess){
        parentWidget()->close();
        return;
    }
    QPoint posA = ui->btnClose->mapToGlobal(QPoint(0, 0));
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
            parentWidget()->resize(700,500);
        }
        else
        {
            parentWidget()->resize(500,500);
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

