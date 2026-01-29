#include "WidgetSystemToolTip.h"
#include "ui_WidgetSystemToolTip.h"

WidgetSystemToolTip::WidgetSystemToolTip(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::WidgetSystemToolTip)
{
    ui->setupUi(this);
    setGeometry(0,0,parent->width(),parent->height());
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        setRepairStatus(0);
        close();
    });
    connect(ui->btnWifiConfirm,&QPushButton::clicked,this,[&]{
        setRepairStatus(1);
        emit signal_widgetSystemToolWifiIpRevovery();
    });
    connect(ui->btnWifiCancel,&QPushButton::clicked,this,[&]{
        setRepairStatus(0);
        close();
    });
    connect(ui->btnOk,&QPushButton::clicked,this,[&]{
        setRepairStatus(0);
        close();
    });
    connect(ui->btnFail,&QPushButton::clicked,this,[&]{
        setRepairStatus(0);
        close();
    });
    setRepairStatus(0);
}

WidgetSystemToolTip::~WidgetSystemToolTip()
{
    delete ui;
}

void WidgetSystemToolTip::show()
{
    raise();
    QWidget::show();
}

bool WidgetSystemToolTip::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

void WidgetSystemToolTip::setMacAddress(QString address)
{
    m_strMac = "MAC:"+address;
}
void WidgetSystemToolTip::setRepairStatus(int status)
{
    ui->widgetFail->hide();
    ui->widgetProgress->hide();
    ui->widgetOk->hide();
    ui->widgetWifi->hide();
    ui->widgetSuccess->hide();
    if(status == -1){//修复失败
        ui->widgetFail->show();
    }else if(status == 0){//初始化
    }else if(status == 1){//修复中
        ui->widgetProgress->show();
    }else if(status == 2){//成功
        ui->labelOkTxt->setText(tr("修复完成"));
        int iMargin = 56;
        int iWidth = ui->widgetOk->width();
        int iHeight = 0;
        int itxtwidth = iWidth-iMargin*2;
        QFontMetrics info(ui->labelOkTxt->font());
        QSize sz = info.size(Qt::TextSingleLine,ui->labelOkTxt->text());
        if (sz.width()>itxtwidth){
            int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
            ui->labelOkTxt->setFixedSize(itxtwidth, h+100);
        }else{
            ui->labelOkTxt->setFixedSize(itxtwidth,sz.height()+100);
        }
        iHeight += ui->labelOkTxt->height();
        iHeight += ui->btnOk->height();
        iHeight += 56;
        ui->widgetOk->setFixedHeight(iHeight);
        ui->widgetOk->show();
    }else if(status == 3){//默认ip确认是否wifi连接
        int iMargin = 56;
        int iWidth = ui->widgetWifi->width();
        int iHeight = 0;
        int itxtwidth = iWidth-iMargin*2;
        QFontMetrics info(ui->labelIsWifiConnect->font());
        QSize sz = info.size(Qt::TextSingleLine,ui->labelIsWifiConnect->text());
        if (sz.width()>itxtwidth){
            int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
            ui->labelIsWifiConnect->setFixedSize(itxtwidth, h+100);
        }else{
            ui->labelIsWifiConnect->setFixedSize(itxtwidth,sz.height()+100);
        }
        iHeight += ui->labelIsWifiConnect->height();
        iHeight += ui->btnWifiConfirm->height();
        iHeight += 56;
        ui->widgetWifi->setFixedHeight(iHeight);
        ui->widgetWifi->show();
    }else if(status == 4){//默认ip修复成功
         ui->labelOkTxt->setText(tr("设置完成请重启控制柜后检查网关和IP是否可用"));
         int iMargin = 56;
         int iWidth = ui->widgetOk->width();
         int iHeight = 0;
         int itxtwidth = iWidth-iMargin*2;
         QFontMetrics info(ui->labelOkTxt->font());
         QSize sz = info.size(Qt::TextSingleLine,ui->labelOkTxt->text());
         if (sz.width()>itxtwidth){
             int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
             ui->labelOkTxt->setFixedSize(itxtwidth, h+100);
         }else{
             ui->labelOkTxt->setFixedSize(itxtwidth,sz.height()+100);
         }
         iHeight += ui->labelOkTxt->height();
         iHeight += ui->btnOk->height();
         iHeight += 56;
         ui->widgetOk->setFixedHeight(iHeight);
         ui->widgetOk->show();
    }else if(status == 21){//成功并重启控制柜
        ui->labelMessage->setText(tr("请重启下控制柜"));
        int iHeight = ui->labelSuccess->height();
        iHeight += ui->labelMessage->height();
        iHeight += ui->btnConfirm->height();
        iHeight += 56;
        ui->widgetSuccess->setFixedHeight(iHeight);
        ui->widgetSuccess->show();
    }else if(status == 22){//成功并重启控制柜Mac显示地址
        ui->labelMessage->setText(tr("请重启下控制柜")+"\n"+m_strMac);
        int iHeight = ui->labelSuccess->height();
        iHeight += ui->labelMessage->height();
        iHeight += ui->btnConfirm->height();
        iHeight += 56;
        ui->widgetSuccess->setFixedHeight(iHeight);
        ui->widgetSuccess->show();
    }
}
