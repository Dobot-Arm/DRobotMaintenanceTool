#include "ServoParamsAllUpgradeWidget.h"
#include "ui_ServoParamsAllUpgradeWidget.h"

ServoParamsAllUpgradeWidget::ServoParamsAllUpgradeWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::ServoParamsAllUpgradeWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnClose,&QPushButton::clicked,this,&ServoParamsAllUpgradeWidget::close);
    initStatus();
}

ServoParamsAllUpgradeWidget::~ServoParamsAllUpgradeWidget()
{
    delete ui;
}

void ServoParamsAllUpgradeWidget::setStatus(int status,QString msg,QString errorParams)
{
    ui->labelDoing->hide();
    ui->labelMessage->hide();
    ui->labelIconStatus->show();
    ui->btnClose->show();
    if(status == 2){
        ui->labelIconStatus->setStyleSheet("QLabel{\n	image: url(:/image/images2/Icon_doingSuccessed_big.png);\n	border: none;\n}");
        ui->labelStatus->setStyleSheet(g_strFontSuccessStyleSheet);
    }
    else if(status == -2)
    {
        ui->labelIconStatus->setStyleSheet("QLabel{\n	image: url(:/image/images2/Icon_doingFailed_big.png);\n	border: none;\n}");
        ui->labelErrorAddr->show();
        ui->textErrorServoParams->setText(errorParams);
        ui->textErrorServoParams->show();
        ui->labelStatus->setStyleSheet(g_strFontFailedStyleSheet);
    }
    else if(status == -1){
        ui->labelIconStatus->setStyleSheet("QLabel{\n	image: url(:/image/images2/Icon_doingFailed_big.png);\n	border: none;\n}");
        ui->labelStatus->setStyleSheet(g_strFontFailedStyleSheet);
    }
    if(!msg.isEmpty())
    {
        ui->labelStatus->setText(msg);
    }
    ui->labelStatus->show();
}

void ServoParamsAllUpgradeWidget::initStatus()
{
    ui->labelErrorAddr->hide();
    ui->textErrorServoParams->hide();
    //加载gif图片
    QMovie *movie = new QMovie(":/image/images2/loading.gif");
    ui->labelDoing->setMovie(movie);
    movie->start();
    ui->labelDoing->show();
    ui->labelMessage->show();
    ui->btnClose->hide();
    ui->labelStatus->hide();
    ui->labelIconStatus->hide();
}

void ServoParamsAllUpgradeWidget::show()
{
    raise();
    QWidget::show();
}
