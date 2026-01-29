#include "FormServoWriteProgress.h"
#include "ui_FormServoWriteProgress.h"

FormServoWriteProgress::FormServoWriteProgress(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FormServoWriteProgress)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnOk,&QPushButton::clicked,this,&FormServoWriteProgress::close);
    connect(ui->btnCancel,&QPushButton::clicked,this,&FormServoWriteProgress::close);
    connect(ui->btnFail,&QPushButton::clicked,this,&FormServoWriteProgress::close);
    ui->progressBar->setTextVisible(false);
    ui->progressBar->setRange(0,100);
}

FormServoWriteProgress::~FormServoWriteProgress()
{
    delete ui;
}

void FormServoWriteProgress::showSuccess()
{
    ui->widgetFail->hide();
    ui->widgetFailMsg->hide();
    ui->widgetProgress->hide();
    ui->widgetOk->show();
}

void FormServoWriteProgress::showFail(QString strErrMsg/*=""*/)
{
    ui->widgetOk->hide();
    ui->widgetProgress->hide();
    ui->widgetFail->hide();
    ui->widgetFailMsg->hide();
    if (strErrMsg.isEmpty())
    {
        ui->widgetFail->show();
    }
    else
    {
        ui->widgetFailMsg->show();
        int iMargin = 56;
        int iWidth = ui->widgetFailMsg->width();
        int iHeight = 40;

        int itxtwidth = iWidth-iMargin*2;
        QFontMetrics info(ui->labelErrorTitle->font());
        QSize sz = info.size(Qt::TextSingleLine,ui->labelErrorTitle->text());
        if (sz.width()>itxtwidth){
            int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
            ui->labelErrorTitle->setFixedSize(itxtwidth, h+20);
        }else{
            ui->labelErrorTitle->setFixedSize(itxtwidth,sz.height()+20);
        }
        iHeight += ui->labelErrorTitle->height();
        iHeight += ui->btnCancel->height();
        iHeight += 56;

        ui->txtErrMsg->setText(strErrMsg);
        info = QFontMetrics(ui->txtErrMsg->font());
        sz = info.size(Qt::TextSingleLine,"ABC");
        int iTxtHeight = (strErrMsg.count('\n')+1)*sz.height();
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

void FormServoWriteProgress::setUpgrading(int iPercent)
{
    ui->widgetOk->hide();
    ui->widgetFail->hide();
    ui->widgetFailMsg->hide();
    QString str=tr("写入中");
    ui->labelProg->setText(str+QString::asprintf("(%d%%)...", iPercent));
    ui->progressBar->setValue(iPercent);
    ui->widgetProgress->show();
}
