#include "SystemToolSNWriteWidget.h"
#include "ui_SystemToolSNWriteWidget.h"
#include "LuaApi.h"

#include "Define.h"
#include "CommonData.h"
SystemToolSNWriteWidget::SystemToolSNWriteWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::SystemToolSNWriteWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    connect(ui->btnClose,&QPushButton::clicked,this,&SystemToolSNWriteWidget::slotClose);
    connect(ui->btnWirteABind,&QPushButton::clicked,this,&SystemToolSNWriteWidget::slotWirteABind);
    initWidget();

    hide();
    setGeometry(0,0,parent->width(),parent->height());
}

SystemToolSNWriteWidget::~SystemToolSNWriteWidget()
{
    delete ui;
}

void SystemToolSNWriteWidget::exec()
{
    raise();
    show();
    m_loopExit.exec();
    hide();
}

void SystemToolSNWriteWidget::initWidget()
{
    ui->lineEditControllCabinetSN->setPlaceholderText(tr("请输入"));
    ui->lineEditRobotArmSN->setPlaceholderText(tr("请输入"));
    DobotType::SettingsProductInfoHardwareInfo info = CommonData::getSettingsProductInfoHardwareInfo();
    ui->lineEditControllCabinetSN->setText(info.ControllCabinetSNCode);
    ui->lineEditRobotArmSN->setText(info.RealArmSNCode);

    auto type = CLuaApi::getInstance()->getSNCodeWriteType();
    if (type.hasValue())
    {
        if (1 == type.value() || 2 == type.value())
        {
            ui->lineEditRobotArmSN->hide();
            ui->labelRobotArmSN->hide();
            ui->labelControllCabinetSN->setText(tr("机器序列号："));
        }
        else if (3 == type.value())
        {
            ui->labelControllCabinetSN->setText(tr("控制柜SN码："));
        }
    }
    else
    {
        if(CommonData::getControllerType().originName == g_strPropertiesCabinetTypeMagicianE6
                ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCC162)
                ||CommonData::getStrPropertiesCabinetType().name.contains(g_strPropertiesCabinetTypeCCBOX))
        {
            ui->lineEditRobotArmSN->hide();
            ui->labelRobotArmSN->hide();
            ui->labelControllCabinetSN->setText(tr("机器序列号："));
        }
        else
        {
            ui->labelControllCabinetSN->setText(tr("控制柜SN码："));
        }
    }
    ui->labelErrorMessage->hide();
}

void SystemToolSNWriteWidget::slotClose()
{
    m_isClose = true;
    m_loopExit.quit();
}

void SystemToolSNWriteWidget::slotWirteABind()
{
    if(ui->lineEditRobotArmSN->isVisible() &&ui->lineEditRobotArmSN->text().isEmpty())
    {
        ui->labelErrorMessage->show();
        ui->lineEditRobotArmSN->setProperty("isEmpty","true");
        updateStyleSheet(ui->lineEditRobotArmSN);
        return;
    }

    ui->lineEditRobotArmSN->setProperty("isEmpty","false");
    updateStyleSheet(ui->lineEditRobotArmSN);
    ui->labelErrorMessage->hide();
    if(ui->lineEditRobotArmSN->isVisible())
    {
        if(ui->lineEditRobotArmSN->text().contains(CommonData::getControllerType().originName))
        {
            m_isSNMacthSuccess = true;
        }
        else
        {
            m_isSNMacthSuccess = false;
        }
        m_robotArmSN = ui->lineEditRobotArmSN->text();
        m_controllCabinetSN = ui->lineEditControllCabinetSN->text();
    }
    else
    {
        m_isSNMacthSuccess = true;
        m_controllCabinetSN = ui->lineEditControllCabinetSN->text();
    }
    m_loopExit.quit();
}
