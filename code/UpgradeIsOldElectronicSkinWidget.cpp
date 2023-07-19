#include "UpgradeIsOldElectronicSkinWidget.h"
#include "ui_UpgradeIsOldElectronicSkinWidget.h"

UpgradeIsOldElectronicSkinWidget::UpgradeIsOldElectronicSkinWidget(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::UpgradeIsOldElectronicSkinWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        emit signal_upgradeIsOldElectronicSkinWidget_toUpgradeDevice();
        close();
    });

    connect(ui->btnCancel,&QPushButton::clicked,this,[&]{
        close();
    });
}

UpgradeIsOldElectronicSkinWidget::~UpgradeIsOldElectronicSkinWidget()
{
    delete ui;
}

void UpgradeIsOldElectronicSkinWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool UpgradeIsOldElectronicSkinWidget::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
