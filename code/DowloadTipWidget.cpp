#include "DowloadTipWidget.h"
#include "ui_DowloadTipWidget.h"

DowloadTipWidget::DowloadTipWidget(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::DowloadTipWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    connect(ui->btnConfirm,&QPushButton::clicked,this,&DowloadTipWidget::slot_confirm);
}

DowloadTipWidget::~DowloadTipWidget()
{
    delete ui;
}

void DowloadTipWidget::setMessage(QString msg)
{
    ui->textDownloadURL->setText(msg);
}

void DowloadTipWidget::slot_confirm()
{
    close();
}

