#include "WidgetSingleUpgrading.h"
#include "ui_WidgetSingleUpgrading.h"

WidgetSingleUpgrading::WidgetSingleUpgrading(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetSingleUpgrading)
{
    ui->setupUi(this);

    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowModality(Qt::ApplicationModal); //禁用主窗口

    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        close();
    });

    ui->labelDoing->hide();
    //加载gif图片
    QMovie *movie = new QMovie(":/image/images2/loading.gif");
    ui->labelDoing->setMovie(movie);
    movie->start();
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

    ui->labelDoing->hide();
    ui->labelIconUpgrade->hide();
    ui->labelUpgradeStatus->hide();
    ui->labetUpgradeText->hide();
    ui->btnConfirm->hide();
    if (status == 1)
    {
        ui->labetUpgradeText->setText(tr("升级中"));
        ui->labetUpgradeText->show();
        ui->labelDoing->show();
    }
    else if (status == 2)
    {
        ui->labelUpgradeStatus->setText(tr("升级完成！请重启控制柜"));
        ui->labelIconUpgrade->setStyleSheet("QLabel{\n	image: url(:/image/images2/Icon_doingSuccessed_big.png);\n	border-radius: none;\n}");

        ui->btnConfirm->show();
        ui->labelIconUpgrade->show();
        ui->labelUpgradeStatus->show();
    }
    else if (status == -1)
    {
        ui->labetUpgradeText->setText(errorMsg);
        ui->labetUpgradeText->show();
        ui->labelUpgradeStatus->setText(tr("升级失败！请联系FAE"));
        ui->labelIconUpgrade->setStyleSheet("QLabel{\n	image: url(:/image/images2/Icon_doingFailed_big.png);\n	border-radius: none;\n}");
        ui->btnConfirm->show();
        ui->labelIconUpgrade->show();
        ui->labelUpgradeStatus->show();
    }
}

bool WidgetSingleUpgrading::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
