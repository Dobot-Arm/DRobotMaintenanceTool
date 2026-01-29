#include "ControllerWarn.h"
#include "ui_ControllerWarn.h"

ControllerWarn::ControllerWarn(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::ControllerWarn)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnClose,&QPushButton::clicked,this,&ControllerWarn::close);
}

ControllerWarn::~ControllerWarn()
{
    delete ui;
}
