#include "SNMatchFailedWidget.h"
#include "ui_SNMatchFailedWidget.h"
#include "Define.h"
SNMatchFailedWidget::SNMatchFailedWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::SNMatchFailedWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        m_loopExit.quit();
    });
    hide();
    setGeometry(0,0,parent->width(),parent->height());
}

SNMatchFailedWidget::~SNMatchFailedWidget()
{
    delete ui;
}

void SNMatchFailedWidget::exec()
{
    raise();
    show();
    m_loopExit.exec();
    hide();
}
