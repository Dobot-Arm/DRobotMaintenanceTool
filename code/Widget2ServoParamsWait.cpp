#include "Widget2ServoParamsWait.h"
#include "ui_Widget2ServoParamsWait.h"

Widget2ServoParamsWait::Widget2ServoParamsWait(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget2ServoParamsWait)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowModality(Qt::ApplicationModal); //禁用主窗口

    connect(ui->btnClose,&QPushButton::clicked,this,&Widget2ServoParamsWait::slot_close);
}

Widget2ServoParamsWait::~Widget2ServoParamsWait()
{
    delete ui;
}

void Widget2ServoParamsWait::setStatus(int status,QString msg,QString errorParams)
{
    ui->labelDoing->hide();
    ui->labelMessage->hide();
    ui->btnClose->show();
    if(status == 2){
        ui->labelStatus->setStyleSheet(g_strFontSuccessStyleSheet);
    }
    if(status == -2)
    {
        ui->labelErrorAddr->show();
        ui->textErrorServoParams->setText(errorParams);
        ui->textErrorServoParams->show();
        ui->labelStatus->setStyleSheet(g_strFontFailedStyleSheet);
    }
    if(status == -1){
        ui->labelStatus->setStyleSheet(g_strFontFailedStyleSheet);
    }
    if(!msg.isEmpty())
    {
        ui->labelStatus->setText(msg);
    }
    ui->labelStatus->show();
}

void Widget2ServoParamsWait::initStatus()
{
    ui->labelErrorAddr->hide();
    ui->textErrorServoParams->hide();
    //加载gif图片
    QMovie *movie = new QMovie(":/image/images2/loading.gif");
    ui->labelDoing->setMovie(movie);
    movie->start();
    ui->labelDoing->show();
    ui->labelMessage->show();
    ui->btnClose->hide();
    ui->labelStatus->hide();
}

void Widget2ServoParamsWait::slot_close()
{
    ui->labelDoing->show();
    ui->labelMessage->show();

    ui->btnClose->hide();
    ui->labelStatus->hide();
    close();
}

bool Widget2ServoParamsWait::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

void Widget2ServoParamsWait::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(5, 5, this->width() - 5 * 2, this->height() - 5 * 2, 3, 3);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(Qt::gray);
    for (int i = 0; i < 5; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRoundedRect(5 - i, 5 - i, this->width() - (5 - i) * 2, this->height() - (5 - i) * 2, 3 + i, 3 + i);
        color.setAlpha(80 - qSqrt(i) * 40);
        painter.setPen(color);
        painter.drawPath(path);
    }
}
