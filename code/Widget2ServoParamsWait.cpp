#include "Widget2ServoParamsWait.h"
#include "ui_Widget2ServoParamsWait.h"
#include "MainWidget2.h"
#include <QStyle>

Widget2ServoParamsWait::Widget2ServoParamsWait(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::Widget2ServoParamsWait)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnClose,&QPushButton::clicked,this,[this]{
        ui->labelDoing->show();
        ui->labelMessage->show();
        ui->labelStatusIcon->hide();
        ui->textErrorServoParams->hide();
        ui->btnClose->hide();
        ui->labelStatus->hide();
        close();
    });
    initStatus();
    setGeometry(0,0,parent->width(),parent->height());
}

Widget2ServoParamsWait::~Widget2ServoParamsWait()
{
    delete ui;
}

void Widget2ServoParamsWait::setStatus(int status,QString msg,QStringList errorParams)
{
    ui->labelDoing->hide();
    ui->labelMessage->hide();
    ui->btnClose->show();
    ui->labelStatusIcon->show();
    if(status == 2){
        this->style()->unpolish(ui->labelStatusIcon); //清除旧的样式
        ui->labelStatusIcon->setProperty("status","success");
        this->style()->polish(ui->labelStatusIcon);   //更新为新的样式
//        ui->labelStatus->setStyleSheet(g_strFontSuccessStyleSheet);
    }
    if(status == -2)
    {
        this->style()->unpolish(ui->labelStatusIcon); //清除旧的样式
        ui->labelStatusIcon->setProperty("status","fail");
        this->style()->polish(ui->labelStatusIcon);   //更新为新的样式
        for(QString errorParam : errorParams)
        {
            ui->textErrorServoParams->append(errorParam);
        }
        if(50+errorParams.count()*20<237)
        {
            ui->textErrorServoParams->setMaximumHeight(20+errorParams.count()*20);
        }
        else
        {
            ui->textErrorServoParams->setMaximumHeight(237);
        }
        ui->textErrorServoParams->show();
//        ui->labelStatus->setStyleSheet(g_strFontFailedStyleSheet);
    }
    if(status == -1){
        this->style()->unpolish(ui->labelStatusIcon); //清除旧的样式
        ui->labelStatusIcon->setProperty("status","fail");
        this->style()->polish(ui->labelStatusIcon);   //更新为新的样式
//        ui->labelStatus->setStyleSheet(g_strFontFailedStyleSheet);
    }
    if(!msg.isEmpty())
    {
        ui->labelStatus->setText(msg);
    }
    ui->labelStatus->show();
}

void Widget2ServoParamsWait::show()
{
    this->raise();
    QWidget::show();
}

void Widget2ServoParamsWait::initStatus()
{
    ui->labelStatusIcon->hide();
    ui->textErrorServoParams->clear();
    ui->textErrorServoParams->hide();
    //加载gif图片
    QMovie *movie = new QMovie(":/image/images2/loading.gif");
    ui->labelDoing->setMovie(movie);
    movie->start();
    ui->labelDoing->show();
    ui->labelMessage->show();
    ui->btnClose->hide();
    ui->labelStatus->hide();
}

void Widget2ServoParamsWait::showEvent(QShowEvent *event)
{
    ui->widget->resize(477,300);
}
