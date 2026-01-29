#include "FormServoParamButton.h"
#include "ui_FormServoParamButton.h"

FormServoParamButton::FormServoParamButton(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FormServoParamButton)
{
    ui->setupUi(this);
    ui->labelDoing->setText("");

    m_pMovie = new QMovie(":/image/images/loading.gif");
    ui->labelDoing->setMovie(m_pMovie);

    connect(ui->btnCheckServoParam, &QPushButton::clicked, this, &FormServoParamButton::signalCheckServoParam);
    connect(ui->btnUpdateServoParam, &QPushButton::clicked, this, &FormServoParamButton::signalUpdateServoParam);
}

FormServoParamButton::~FormServoParamButton()
{
    delete ui;
}

void FormServoParamButton::showServoParamLabel(bool bChecking, QVariant bNeedUpdate, bool bFail/*=false*/,QString strMsgTxt/*=""*/)
{
    m_bChecking = bChecking;
    m_bNeedUdate = bNeedUpdate;
    m_bFail = bFail;
    if (bChecking)
    {
        ui->labelServoState->hide();
        ui->labelServoStateInfo->hide();

        ui->labelDoing->setText("");
        ui->labelDoing->setMovie(m_pMovie);
        ui->labelDoing->show();
        ui->labelCheck->show();
        m_pMovie->start();
    }
    else
    {
        m_pMovie->stop();
        ui->labelDoing->hide();
        ui->labelCheck->hide();
        if (bFail)
        {
            ui->labelServoState->hide();
            ui->labelServoStateInfo->show();
            if (strMsgTxt.isEmpty())
                ui->labelServoStateInfo->setText(tr("读伺服参数数据失败"));
            else
                ui->labelServoStateInfo->setText(strMsgTxt);
            return;
        }
        if (bNeedUpdate.type() == QVariant::Bool)
        {
            auto bUpdate = bNeedUpdate.toBool();
            ui->labelServoState->show();
            ui->labelServoStateInfo->show();
            if (bUpdate)
            {
                ui->labelServoState->setText(tr("需要升级"));
                if (strMsgTxt.isEmpty())
                    ui->labelServoStateInfo->setText(tr("请先一键升级完固件后，重启控制柜，再次连接成功后，手动点击覆盖按钮"));
                else
                    ui->labelServoStateInfo->setText(strMsgTxt);
            }
            else
            {
                ui->labelServoState->setText(tr("无需要升级"));
                if (strMsgTxt.isEmpty())
                    ui->labelServoStateInfo->setText(tr("您只需完成升级操作，无需操作伺服参数按钮"));
                else
                    ui->labelServoStateInfo->setText(strMsgTxt);
            }
        }
        else
        {
            ui->labelServoState->show();
            ui->labelServoState->setText("");
            ui->labelServoStateInfo->hide();
        }
    }
}

bool FormServoParamButton::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        showServoParamLabel(m_bChecking, m_bNeedUdate,m_bFail);
        return true;
    }
    return QWidget::event(event);
}

void FormServoParamButton::showEvent(QShowEvent *event)
{
    /*
    ui->labelServoState->setText("");
    ui->labelServoStateInfo->hide();
    ui->labelDoing->hide();
    ui->labelCheck->hide();
    */
    showServoParamLabel(m_bChecking, m_bNeedUdate,m_bFail);
}
