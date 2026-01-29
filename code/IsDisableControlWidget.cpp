#include "IsDisableControlWidget.h"
#include "ui_IsDisableControlWidget.h"

IsDisableControlWidget::IsDisableControlWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::IsDisableControl)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnDisableControlMode,&QPushButton::clicked,this,[&]{
        emit signalDiableControlMode();
    });
    connect(ui->btnDisconnect,&QPushButton::clicked,this,[&]{
        emit signalDisconnect();
        close();
    });
}

IsDisableControlWidget::~IsDisableControlWidget()
{
    delete ui;
}

bool IsDisableControlWidget::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
