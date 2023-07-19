#include "WidgetSystemFileCheckTip.h"
#include "ui_WidgetSystemFileCheckTip.h"

WidgetSystemFileCheckTip::WidgetSystemFileCheckTip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetSystemFileCheckTip)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);
    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        close();
    });
}

WidgetSystemFileCheckTip::~WidgetSystemFileCheckTip()
{
    delete ui;
}

void WidgetSystemFileCheckTip::setMessage(int status,QString errorfiles)
{
    ui->btnConfirm->hide();

    ui->textErrorFiles->hide();
    ui->textErrorFiles->clear();
    ui->labelFailed->hide();
    ui->labelFailedIcon->hide();
    ui->labelHelp->hide();

    ui->labelVersionFail->hide();
    ui->labelVersionFailIcon->hide();

    ui->labelRepairing->hide();

    ui->labelSuccess->hide();
    ui->labelSuccessIcon->hide();
    if(status == SystemFileCheckTipStatus::FileCheck_INIT)
    {

    }
    if(status == SystemFileCheckTipStatus::FileCheck_DOING)
    {
        ui->labelRepairing->show();
    }
    if(status == SystemFileCheckTipStatus::FileCheck_SUCCESS)
    {
        ui->labelSuccess->show();
        ui->labelSuccessIcon->show();
        ui->btnConfirm->show();
    }
    if(status == SystemFileCheckTipStatus::FileCheck_FAIL)
    {
        ui->textErrorFiles->setText(errorfiles);
        ui->textErrorFiles->show();
        ui->labelFailed->show();
        ui->labelFailedIcon->show();
        ui->labelHelp->show();
        ui->btnConfirm->show();
    }

    if(status == SystemFileCheckTipStatus::FileCheck_VEVRSION_FAIL)
    {
        ui->labelVersionFail->show();
        ui->labelVersionFailIcon->show();
        ui->btnConfirm->show();
    }
}

void WidgetSystemFileCheckTip::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton
      && e->x() < this->width()
      && e->y() < this->height())
    {
        this->setCursor(Qt::ClosedHandCursor);
        mouse_press = true;
    }
    move_point=e->globalPos()-this->pos();
}

void WidgetSystemFileCheckTip::mouseMoveEvent(QMouseEvent *e)
{
    if(mouse_press)
    {
        QPoint move_pos=e->globalPos();
        this->move(move_pos-move_point);
    }
}

void WidgetSystemFileCheckTip::mouseReleaseEvent(QMouseEvent *e)
{
    mouse_press = false;
    this->setCursor(Qt::ArrowCursor);
}
void WidgetSystemFileCheckTip::paintEvent(QPaintEvent *event)
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

bool WidgetSystemFileCheckTip::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
