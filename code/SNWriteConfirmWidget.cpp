#include "SNWriteConfirmWidget.h"
#include "ui_SNWriteConfirmWidget.h"
#include "Define.h"
SNWriteConfirmWidget::SNWriteConfirmWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::SNWriteConfirmWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    connect(ui->btnWrite,&QPushButton::clicked,this,[&]{
        m_isConfirmWrite = true;
        m_loopExit.quit();
    });
    connect(ui->btnCancel,&QPushButton::clicked,this,[&]{
        m_isConfirmWrite = false;
        m_loopExit.quit();
    });
    hide();
    setGeometry(0,0,parent->width(),parent->height());
}

SNWriteConfirmWidget::~SNWriteConfirmWidget()
{
    delete ui;
}

void SNWriteConfirmWidget::setMessage(QString msg)
{
    ui->labelTitle->setText(msg);
}

void SNWriteConfirmWidget::exec()
{
    raise();
    show();
    m_loopExit.exec();
    hide();
}
