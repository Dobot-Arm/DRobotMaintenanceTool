#include "IsControlLogBusyWidget.h"
#include "ui_IsControlLogBusyWidget.h"

IsControlLogBusyWidget::IsControlLogBusyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IsControlLogBusyWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口

    connect(ui->btnConfirm,&QPushButton::clicked,this,&IsControlLogBusyWidget::slot_confirm);
    connect(ui->btnCancel,&QPushButton::clicked,this,&IsControlLogBusyWidget::slot_cancel);
}

IsControlLogBusyWidget::~IsControlLogBusyWidget()
{
    delete ui;
}

bool IsControlLogBusyWidget::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

void IsControlLogBusyWidget::slot_cancel()
{
    emit signal_isContinueUpgrade(false);
    close();
}

void IsControlLogBusyWidget::slot_confirm()
{
    emit signal_isContinueUpgrade(true);
}

void IsControlLogBusyWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
