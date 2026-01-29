#include "FormServoUpdateComfirm.h"
#include "ui_FormServoUpdateComfirm.h"
#include "Define.h"

FormServoUpdateComfirm::FormServoUpdateComfirm(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FormServoUpdateComfirm)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    connect(ui->btnClose,&QPushButton::clicked,this,&FormServoUpdateComfirm::close);
    connect(ui->btnOk,&QPushButton::clicked,this,&FormServoUpdateComfirm::signalOk);

    const QString str = tr("覆盖伺服参数需保证机器下使能。升级成功后首次运行机器时，请远离机械臂确保自身安全");
    ui->labelInfo->setText("<p style='line-height:20px;width:100%;white-space:pre-wrap;'>"+str+"</p>");
}

FormServoUpdateComfirm::~FormServoUpdateComfirm()
{
    delete ui;
}

void FormServoUpdateComfirm::setText(QString str)
{
    ui->labelMessage->setText("<p style='line-height:20px;width:100%;white-space:pre-wrap;'>"+str+"</p>");
}

void FormServoUpdateComfirm::resizeEvent(QResizeEvent *event)
{
    if (!g_isEnglish)
    {
        ui->widget->setFixedHeight(ui->widget->height()*0.8);
    }
}
