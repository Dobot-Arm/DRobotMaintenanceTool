#include "WidgetSingleUpgrading.h"
#include "ui_WidgetSingleUpgrading.h"

WidgetSingleUpgrading::WidgetSingleUpgrading(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::WidgetSingleUpgrading)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    connect(ui->btnOk,&QPushButton::clicked,this,&WidgetSingleUpgrading::close);
    connect(ui->btnFail,&QPushButton::clicked,this,&WidgetSingleUpgrading::close);

    //加载gif图片
    m_pMovie = new QMovie(":/image/images2/loading.gif");

    ui->labetUpgradeText->setText("");
}

WidgetSingleUpgrading::~WidgetSingleUpgrading()
{
    delete ui;
}

void WidgetSingleUpgrading::retranslate()
{
    ui->retranslateUi(this);
}

void WidgetSingleUpgrading::setSingleUpgradeStatus(int status,QString errorMsg)
{
    m_pMovie->stop();
    ui->widgetProgress->hide();
    ui->widgetOk->hide();
    ui->widgetFail->hide();
    if (status == 1)
    {
        ui->labelDoing->setMovie(m_pMovie);
        m_pMovie->start();
        ui->widgetProgress->show();
    }
    else if (status == 2)
    {
        ui->widgetOk->show();
        int iMargin = 56;
        int iWidth = ui->widgetOk->width();
        int iHeight = 0;
        int itxtwidth = iWidth-iMargin*2;
        QFontMetrics info(ui->labelOkTitle->font());
        QSize sz = info.size(Qt::TextSingleLine,ui->labelOkTitle->text());
        if (sz.width()>itxtwidth){
            int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
            ui->labelOkTitle->setFixedSize(itxtwidth, h+64);
        }else{
            ui->labelOkTitle->setFixedSize(itxtwidth,sz.height()+64);
        }
        iHeight += ui->labelOkTitle->height();
        iHeight += ui->btnOk->height();
        iHeight += 32;
        ui->widgetOk->setFixedHeight(iHeight);
    }
    else if (status == -1)
    {
        ui->widgetFail->show();
        int iMargin = 56;
        int iWidth = ui->widgetFail->width();
        int iHeight = 0;
        int itxtwidth = iWidth-iMargin*2;
        QFontMetrics info(ui->labelVersionFail->font());
        QSize sz = info.size(Qt::TextSingleLine,ui->labelVersionFail->text());
        if (sz.width()>itxtwidth){
            int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
            ui->labelVersionFail->setFixedSize(itxtwidth, h+64);
        }else{
            ui->labelVersionFail->setFixedSize(itxtwidth,sz.height()+64);
        }
        iHeight += ui->labelVersionFail->height();

        ui->labetUpgradeText->setText(errorMsg);
        if (errorMsg.isEmpty()){
            ui->labetUpgradeText->hide();
        }else{
            ui->labetUpgradeText->show();
            info = QFontMetrics(ui->labetUpgradeText->font());
            sz = info.size(Qt::TextSingleLine,ui->labetUpgradeText->text());
            if (sz.width()>itxtwidth){
                int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
                ui->labetUpgradeText->setFixedSize(itxtwidth, h+40);
            }else{
                ui->labetUpgradeText->setFixedSize(itxtwidth,sz.height()+40);
            }
            iHeight += ui->labetUpgradeText->height();
        }

        iHeight += ui->btnFail->height();
        iHeight += 32;
        ui->widgetFail->setFixedHeight(iHeight);
    }
}

void WidgetSingleUpgrading::show()
{
    raise();
    QWidget::show();
}

bool WidgetSingleUpgrading::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
