#include "SystemToolSNRepairWidget.h"
#include "ui_SystemToolSNRepairWidget.h"
#include "Define.h"
#include "CommonData.h"
SystemToolSNRepairWidget::SystemToolSNRepairWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::SystemToolSNRepairWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    connect(ui->btnCancel,&QPushButton::clicked,this,&SystemToolSNRepairWidget::slotClose);
    connect(ui->btnWirteABind,&QPushButton::clicked,this,&SystemToolSNRepairWidget::slotWirteABind);
    initWidget();

    hide();
    setGeometry(0,0,parent->width(),parent->height());
}

SystemToolSNRepairWidget::~SystemToolSNRepairWidget()
{
    delete ui;
}

void SystemToolSNRepairWidget::exec()
{
    raise();
    show();
    m_loopExit.exec();
    hide();
}

bool SystemToolSNRepairWidget::isSNMacthSuccess()
{
    return m_isSNMacthSuccess;
}

QString SystemToolSNRepairWidget::getRobotArmSN()
{
    return m_robotArmSN;
}

QString SystemToolSNRepairWidget::getControllCabinetSN()
{
    return m_controllCabinetSN;
}

void SystemToolSNRepairWidget::initWidget()
{
    DobotType::SettingsProductInfoHardwareInfo info = CommonData::getSettingsProductInfoHardwareInfo();
    ui->lineControlSN->setPlaceholderText(tr("请输入"));
    ui->lineRealRobotSN->setPlaceholderText(tr("请输入"));
    ui->lineControlSN->setText(info.ControllCabinetSNCode);
    ui->lineRealRobotSN->setText(info.RealArmSNCode);
    ui->labelControlRobotSN->setText(info.RobotArmSNCode);
    ui->labelErrorMessage->hide();
    m_isClose = false;
}

void SystemToolSNRepairWidget::slotClose()
{
    m_isClose = true;
    m_loopExit.quit();
}

void SystemToolSNRepairWidget::slotWirteABind()
{
    if(ui->lineRealRobotSN->text().isEmpty())
    {
        ui->labelErrorMessage->show();
        ui->lineRealRobotSN->setProperty("isEmpty","true");
        updateStyleSheet(ui->lineRealRobotSN);
    }
    else
    {
        ui->labelErrorMessage->hide();
        ui->lineRealRobotSN->setProperty("isEmpty","false");
        updateStyleSheet(ui->lineRealRobotSN);
        if(!ui->lineRealRobotSN->text().contains(CommonData::getControllerType().originName))
        {
            m_isSNMacthSuccess = false;
        }
        else
        {
            m_isSNMacthSuccess = true;
            m_robotArmSN = ui->lineRealRobotSN->text();
            m_controllCabinetSN = ui->lineControlSN->text();
        }
        m_loopExit.quit();
    }
}
