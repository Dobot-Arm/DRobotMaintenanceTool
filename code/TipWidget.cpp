#include "TipWidget.h"
#include "ui_TipWidget.h"

TipWidget::TipWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::TipWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnConfirm,&QPushButton::clicked,this,&TipWidget::close);
}

TipWidget::~TipWidget()
{
    delete ui;
}
