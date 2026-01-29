#include "UpgradeIsOldElectronicSkinWidget.h"
#include "ui_UpgradeIsOldElectronicSkinWidget.h"

UpgradeIsOldElectronicSkinWidget::UpgradeIsOldElectronicSkinWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::UpgradeIsOldElectronicSkinWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        emit signal_toUpgradeDevice();
        close();
    });

    connect(ui->btnCancel,&QPushButton::clicked,this,&UpgradeIsOldElectronicSkinWidget::close);
}

UpgradeIsOldElectronicSkinWidget::~UpgradeIsOldElectronicSkinWidget()
{
    delete ui;
}
