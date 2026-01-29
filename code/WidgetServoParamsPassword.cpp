#include "WidgetServoParamsPassword.h"
#include "ui_WidgetServoParamsPassword.h"

WidgetServoParamsPassword::WidgetServoParamsPassword(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::WidgetServoParamsPassword)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        int correctPassword = m_currentRandom|4353;
        bool ok;
        int password = ui->linePassword->text().toInt(&ok);
        if(!ok)
        {
            ui->labelConfirmStatus->setText(tr("密码错误，请重新输入"));
            ui->labelConfirmStatus->show();
        }
        if(password==correctPassword)
        {
            ui->linePassword->clear();
            emit signal_servoParamsPassword(true);
        }
        else
        {
            ui->labelConfirmStatus->setText(tr("密码错误，请重新输入"));
            ui->labelConfirmStatus->show();
        }
    });
    connect(ui->btnClose,&QPushButton::clicked,this,[&]{
        ui->linePassword->clear();
        close();
    });
    initStatus();
}

WidgetServoParamsPassword::~WidgetServoParamsPassword()
{
    delete ui;
}

void WidgetServoParamsPassword::initStatus()
{
    m_currentRandom = getRandom();
    ui->labelRandomNum->setText(tr("随机码：")+QString("%1").arg(m_currentRandom));
    ui->labelConfirmStatus->setText("");
}

int WidgetServoParamsPassword::getRandom()
{
    int randTime =  QTime::currentTime().msec();
    if(randTime<10)
    {
        return randTime*1000;
    }
    if(randTime<100)
    {
        return randTime*100;
    }
    if(randTime<1000)
    {
        return randTime*10;
    }
    return randTime;
}
