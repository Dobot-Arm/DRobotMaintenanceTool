#include "Widget2FileRecoveryTip.h"
#include "ui_Widget2FileRecoveryTip.h"

#include <QDebug>
Widget2FileRecoveryTip::Widget2FileRecoveryTip(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::Widget2FileRecoveryTip)
{
    ui->setupUi(this);

    QWidget::setAttribute(Qt::WA_QuitOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    ui->widgetFail->hide();

    connect(ui->btnCancel,&QPushButton::clicked,this,&Widget2FileRecoveryTip::close);
    connect(ui->btnRecoveryConfirmStart,&QPushButton::clicked,this,[&]{
        emit signalsFileRecoveryStart();
    });
    connect(ui->btnRecoveryFailConfirm,&QPushButton::clicked,this,&Widget2FileRecoveryTip::close);
}

Widget2FileRecoveryTip::~Widget2FileRecoveryTip()
{
    delete ui;
}

void Widget2FileRecoveryTip::setWidgetStatus(bool isFinish,QString msg)
{
    if(isFinish){
        ui->widget->show();
        ui->widgetFail->hide();
    }else{
        if(!msg.isEmpty()){
            ui->labelRecoveryFail->setText(msg+tr("不相同，恢复失败"));
        }
        ui->widget->hide();
        ui->widgetFail->show();
    }
}

