#include "FormPackageVersionDetail.h"
#include "ui_FormPackageVersionDetail.h"

FormPackageVersionDetail::FormPackageVersionDetail(QString str, QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FormPackageVersionDetail)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnClose,&QPushButton::clicked,this,&FormPackageVersionDetail::close);
    ui->textBrowser->setPlainText(str);
}

FormPackageVersionDetail::~FormPackageVersionDetail()
{
    delete ui;
}

void FormPackageVersionDetail::resizeEvent(QResizeEvent *event)
{
    QSize sz(event->size().width()*2/3,event->size().height()*2/3);
    ui->widget->setFixedSize(sz);
}
