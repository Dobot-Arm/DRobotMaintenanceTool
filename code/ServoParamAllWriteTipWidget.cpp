#include "ServoParamAllWriteTipWidget.h"
#include "ui_ServoParamAllWriteTipWidget.h"

ServoParamAllWriteTipWidget::ServoParamAllWriteTipWidget(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::ServoParamAllWriteTipWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        emit signal_servoParamsAllWrite();
        close();
    });

    connect(ui->btnCancel,&QPushButton::clicked,this,[&]{
        close();
    });

    connect(ui->btnClose,&QPushButton::clicked,this,[&]{
        close();
    });
}

ServoParamAllWriteTipWidget::~ServoParamAllWriteTipWidget()
{
    delete ui;
}

void ServoParamAllWriteTipWidget::setMessage(bool isNew, QString text)
{

    ui->labelDoing->hide();
    ui->labelMessage->show();
    if(isNew)
    {
        ui->btnClose->show();
        ui->btnCancel->hide();
        ui->btnConfirm->hide();
    }
    else
    {
        ui->btnClose->hide();

        ui->btnCancel->show();
        ui->btnConfirm->show();
    }

    ui->labelMessage->setText(text);

}

void ServoParamAllWriteTipWidget::initStatus()
{
    ui->labelMessage->hide();
    ui->btnClose->hide();
    ui->btnCancel->hide();
    ui->btnConfirm->hide();

    //加载gif图片
    QMovie *movie = new QMovie(":/image/images2/loading.gif");
    ui->labelDoing->setMovie(movie);
    movie->start();
    ui->labelDoing->show();
}


