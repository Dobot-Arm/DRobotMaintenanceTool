#include "SNWriteStatusWidget.h"
#include "ui_SNWriteStatusWidget.h"
#include "Define.h"
SNWriteStatusWidget::SNWriteStatusWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::SNWriteStatusWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    connect(ui->btnConfirm,&QPushButton::clicked,this,[this]{
        m_isClose = true;
        m_loopExit.quit();
    });
    connect(ui->btnRestart,&QPushButton::clicked,this,[this]{
        m_isClose = false;
        m_loopExit.quit();
    });
    hide();
    setGeometry(0,0,parent->width(),parent->height());

    ui->labelFailedMsg->hide();
    ui->labelSuccessMsg->hide();
    ui->btnRestart->hide();
    ui->btnConfirm->hide();
}

SNWriteStatusWidget::~SNWriteStatusWidget()
{
    delete ui;
}

void SNWriteStatusWidget::exec()
{
    //ui宽度为固定，高度自适应
    int iMargin = 56;
    int iWidth = ui->widget->width();
    int iHeight = 0;

    int itxtwidth = iWidth-iMargin*2;

    if (m_bSuccess){
        QFontMetrics info(ui->labelSuccessMsg->font());
        QSize sz = info.size(Qt::TextSingleLine,ui->labelSuccessMsg->text());
        if (sz.width()>itxtwidth){
            int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
            ui->labelSuccessMsg->setFixedSize(itxtwidth, h+64);
        }else{
            ui->labelSuccessMsg->setFixedSize(itxtwidth,sz.height()+64);
        }
        iHeight += ui->labelSuccessMsg->height();
        iHeight += ui->btnConfirm->height();
        iHeight += 32;
    }else{
        QFontMetrics info(ui->labelFailedMsg->font());
        QSize sz = info.size(Qt::TextSingleLine,ui->labelFailedMsg->text());
        if (sz.width()>itxtwidth){
            int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
            ui->labelFailedMsg->setFixedSize(itxtwidth, h+64);
        }else{
            ui->labelFailedMsg->setFixedSize(itxtwidth,sz.height()+64);
        }
        iHeight += ui->labelFailedMsg->height();
        iHeight += ui->btnRestart->height();
        iHeight += 32;
    }
    ui->widget->setFixedSize(iWidth, iHeight);
    raise();
    show();
    m_loopExit.exec();
    hide();
}

void SNWriteStatusWidget::setWriteFailedMsg(QString msg)
{
    if(!msg.isEmpty())ui->labelFailedMsg->setText(msg);
    else ui->labelFailedMsg->setText(tr("写入失败！"));
}

void SNWriteStatusWidget::setWriteOkMsg(QString msg)
{
    if(!msg.isEmpty()) ui->labelSuccessMsg->setText(msg);
    else ui->labelSuccessMsg->setText(tr("写入成功！"));
}

void SNWriteStatusWidget::setWriteStatus(bool isWirteSuccess)
{
    m_bSuccess = isWirteSuccess;
    if(isWirteSuccess)
    {
        ui->labelSuccessMsg->show();
        ui->btnConfirm->show();
        ui->labelFailedMsg->hide();
        ui->btnRestart->hide();
    }
    else
    {
        ui->labelFailedMsg->show();
        ui->btnRestart->show();
        ui->labelSuccessMsg->hide();
        ui->btnConfirm->hide();
    }
}

bool SNWriteStatusWidget::isClose()
{
    return m_isClose;
}
