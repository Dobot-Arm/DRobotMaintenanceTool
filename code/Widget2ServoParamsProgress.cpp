#include "Widget2ServoParamsProgress.h"
#include "ui_Widget2ServoParamsProgress.h"
#include <QStyle>

Widget2ServoParamsProgress::Widget2ServoParamsProgress(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::Widget2ServoParamsProgress)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    setGeometry(0,0,parent->width(),parent->height());
    connect(ui->btnOk,&QPushButton::clicked,this,&Widget2ServoParamsProgress::close);
    connect(ui->btnCancel,&QPushButton::clicked,this,&Widget2ServoParamsProgress::close);
    connect(ui->btnFail,&QPushButton::clicked,this,&Widget2ServoParamsProgress::close);

    m_pMovie = new QMovie(":/image/images/loading.gif");
}

Widget2ServoParamsProgress::~Widget2ServoParamsProgress()
{
    delete ui;
}

void Widget2ServoParamsProgress::show()
{
    this->raise();
    QWidget::show();
}

void Widget2ServoParamsProgress::loading()
{
    ui->labelDoing->setMovie(m_pMovie);
    m_pMovie->start();

    ui->widgetProgress->show();
    ui->widgetOk->hide();
    ui->widgetFail->hide();
    ui->widgetFailMsg->hide();
}

void Widget2ServoParamsProgress::setMessage(QString strMsg, bool bOk, QStringList errorParams)
{
    m_pMovie->stop();
    ui->widgetProgress->hide();
    ui->widgetOk->hide();
    ui->widgetFail->hide();
    ui->widgetFailMsg->hide();

    if (bOk)
    {
        ui->widgetOk->show();
        ui->labelOkTitle->setText(strMsg);
        return;
    }
    if (errorParams.isEmpty())
    {
        ui->widgetFail->show();
        ui->labelVersionFail->setText(strMsg);
    }
    else
    {
        ui->widgetFailMsg->show();
        int iMargin = 56;
        int iWidth = ui->widgetFailMsg->width();
        ui->labelErrorTitle->setText(strMsg);

        int iHeight = 0;
        int itxtwidth = iWidth-iMargin*2;
        QFontMetrics info(ui->labelErrorTitle->font());
        QSize sz = info.size(Qt::TextSingleLine,ui->labelErrorTitle->text());
        if (sz.width()>itxtwidth){
            int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
            ui->labelErrorTitle->setFixedSize(itxtwidth, h+64);
        }else{
            ui->labelErrorTitle->setFixedSize(itxtwidth,sz.height()+64);
        }
        iHeight += ui->labelErrorTitle->height();
        iHeight += ui->labelFailAddr->height();
        iHeight += ui->btnCancel->height();
        iHeight += 32;

        QString strErrMsg;
        for (int i=0; i<errorParams.size(); ++i){
            strErrMsg += errorParams[i];
            strErrMsg += '\n';
        }
        ui->txtErrMsg->setText(strErrMsg);
        info = QFontMetrics(ui->txtErrMsg->font());
        sz = info.size(Qt::TextSingleLine,"ABC");
        int iTxtHeight = (errorParams.size()+1)*sz.height();
        if (iTxtHeight>300){
            ui->txtErrMsg->setFixedHeight(300);
        }else if (iTxtHeight<100){
            ui->txtErrMsg->setFixedHeight(100);
        }else{
            ui->txtErrMsg->setFixedHeight(iTxtHeight+10);
        }
        iHeight += ui->txtErrMsg->height();
        iHeight += 40;
        ui->widgetFailMsg->setFixedHeight(iHeight);
    }
}
