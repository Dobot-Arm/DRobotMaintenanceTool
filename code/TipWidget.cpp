#include "TipWidget.h"
#include "ui_TipWidget.h"

TipWidget::TipWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TipWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint );//无边框，置顶//无边框，置顶
    setWindowModality(Qt::ApplicationModal); //禁用主窗口
    connect(ui->btnClose,&QPushButton::clicked,this,&TipWidget::slot_closeWindow);
    connect(ui->btnConfirm,&QPushButton::clicked,this,&TipWidget::slot_cofirm);
}

TipWidget::~TipWidget()
{
    delete ui;
}



void TipWidget::slot_closeWindow()
{
    close();
}

void TipWidget::slot_cofirm()
{
    close();
}


void TipWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
