#include "LoadingUI.h"
#include "ui_LoadingUI.h"
#include "Define.h"
#include <QMovie>

LoadingUI::LoadingUI(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::LoadingUI)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | windowFlags() );//无边框，置顶
    setAttribute(Qt::WA_TranslucentBackground, true);

    SET_STYLE_SHEET(LoadingUI);
    //加载gif图片
    QMovie *movie = new QMovie(":/image/images/loading.gif");
    ui->labelDoing->setMovie(movie);
    movie->start();
    ui->label->hide();
    ui->labelBig->hide();
}

LoadingUI::~LoadingUI()
{
    delete ui;
}

void LoadingUI::setText(const QString &str)
{
    ui->label->setText(str);
    if(str.isEmpty())
        ui->label->hide();
    else
        ui->label->show();
}

void LoadingUI::setBigText(const QString &str)
{
    ui->labelBig->setText(str);
    if(str.isEmpty())
        ui->labelBig->hide();
    else
        ui->labelBig->show();
}
