#include "MessageWidget.h"
#include "ui_MessageWidget.h"
#include "Define.h"
#include "MainWidget2.h"

MessageWidget::MessageWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::MessageWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnConfirm,&QPushButton::clicked,this,&MessageWidget::close);

    setGeometry(0,0,parent->width(),parent->height());
}

MessageWidget::~MessageWidget()
{
    delete ui;
}

void MessageWidget::setMessage(QString type, QString message)
{
    if(type == "warn"){
        ui->labelMessage->setStyleSheet("border:none;color:rgb(255, 0, 0);font-size:16px;font-weight: normal;");
        ui->labelMessage->setText(message);
    }else{
        ui->labelMessage->setStyleSheet("border:none;color:rgb(0, 0, 255);font-size:16px;font-weight: normal;");
        ui->labelMessage->setText(message);
    }
    ui->btnConfirm->show();
}

void MessageWidget::setBtnText(QString txt)
{
    ui->btnConfirm->setText(txt);
}

void MessageWidget::show()
{
    int iMargin = 56;
    int iWidth = ui->widget->width();
    int iHeight = 0;

    int itxtwidth = iWidth-iMargin*2;
    QFontMetrics info(ui->labelMessage->font());
    QSize sz = info.size(Qt::TextSingleLine,ui->labelMessage->text());
    if (sz.width()>itxtwidth){
        int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
        ui->labelMessage->setFixedSize(itxtwidth, h+64);
    }else{
        ui->labelMessage->setFixedSize(itxtwidth,sz.height()+64);
    }
    iHeight += ui->labelMessage->height();
    iHeight += ui->btnConfirm->height();
    iHeight += 32;
    if (iHeight<184) iHeight=184;

    ui->widget->setFixedSize(iWidth, iHeight);
    raise();
    UIBaseWidget::show();
}

bool MessageWidget::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
