#include "WidgetSystemFileCheckTip.h"
#include "ui_WidgetSystemFileCheckTip.h"
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

WidgetSystemFileCheckTip::WidgetSystemFileCheckTip(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::WidgetSystemFileCheckTip)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnOk,&QPushButton::clicked,this,&WidgetSystemFileCheckTip::close);
    connect(ui->btnCancel,&QPushButton::clicked,this,&WidgetSystemFileCheckTip::close);
    connect(ui->btnFail,&QPushButton::clicked,this,&WidgetSystemFileCheckTip::close);
}

WidgetSystemFileCheckTip::~WidgetSystemFileCheckTip()
{
    delete ui;
}

void WidgetSystemFileCheckTip::setMessage(int status,QStringList errorfiles)
{
    ui->widgetProgress->hide();
    ui->widgetOk->hide();
    ui->widgetFail->hide();
    ui->widgetFailMsg->hide();
    if(status == SystemFileCheckTipStatus::FileCheck_INIT)
    {
    }
    else if(status == SystemFileCheckTipStatus::FileCheck_DOING)
    {
        ui->widgetProgress->show();
    }
    else if(status == SystemFileCheckTipStatus::FileCheck_SUCCESS)
    {
        ui->widgetOk->show();
    }
    else if(status == SystemFileCheckTipStatus::FileCheck_VEVRSION_FAIL)
    {
        ui->widgetFail->show();
    }
    else if(status == SystemFileCheckTipStatus::FileCheck_FAIL)
    {
        ui->widgetFailMsg->show();

        int iMargin = 56;
        int iWidth = ui->widgetFailMsg->width();
        int iHeight = 40;

        int itxtwidth = iWidth-iMargin*2;
        QFontMetrics info(ui->labelErrorTitle->font());
        QSize sz = info.size(Qt::TextSingleLine,ui->labelErrorTitle->text());
        if (sz.width()>itxtwidth){
            int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
            ui->labelErrorTitle->setFixedSize(itxtwidth, h+20);
        }else{
            ui->labelErrorTitle->setFixedSize(itxtwidth,sz.height()+20);
        }
        iHeight += ui->labelErrorTitle->height();
        iHeight += ui->btnCancel->height();
        iHeight += 56;

        QString strErrMsg;
        for (int i=0; i<errorfiles.size(); ++i){
            strErrMsg += errorfiles[i];
            strErrMsg += '\n';
        }
        ui->txtErrMsg->setText(strErrMsg);
        info = QFontMetrics(ui->txtErrMsg->font());
        sz = info.size(Qt::TextSingleLine,"ABC");
        int iTxtHeight = (errorfiles.size()+1)*sz.height();
        if (iTxtHeight>300){
            ui->txtErrMsg->setFixedHeight(300);
        }else if (iTxtHeight<100){
            ui->txtErrMsg->setFixedHeight(100);
        }else{
            ui->txtErrMsg->setFixedHeight(iTxtHeight+10);
        }
        iHeight += ui->txtErrMsg->height();
        iHeight += 40;
        ui->widgetFailMsg->setFixedHeight(iHeight);
    }
}
