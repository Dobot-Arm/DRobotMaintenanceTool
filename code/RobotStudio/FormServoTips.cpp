#include "FormServoTips.h"
#include "ui_FormServoTips.h"

FormServoTips::FormServoTips(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FormServoTips)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnOk, &QPushButton::clicked, this, &FormServoTips::close);

    setGeometry(0,0,parent->width(),parent->height());
}

FormServoTips::~FormServoTips()
{
    delete ui;
}

void FormServoTips::setText(QString str)
{
    ui->labelInfo->setText(str);
}

void FormServoTips::show()
{
    int iMargin = 56;
    int iWidth = ui->widget->width();
    int iHeight = 0;

    int itxtwidth = iWidth-iMargin*2;
    QFontMetrics info(ui->labelInfo->font());
    QSize sz = info.size(Qt::TextSingleLine,ui->labelInfo->text());
    if (sz.width()>itxtwidth){
        int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
        ui->labelInfo->setFixedSize(itxtwidth, h+64);
    }else{
        ui->labelInfo->setFixedSize(itxtwidth,sz.height()+64);
    }
    iHeight += ui->labelInfo->height();
    iHeight += ui->btnOk->height();
    iHeight += 32;
    if (iHeight<180){
        iHeight = 180;
    }
    ui->widget->setFixedSize(iWidth, iHeight);
    raise();
    QWidget::show();
}
