#include "WidgetSystemToolTip.h"
#include "ui_WidgetSystemToolTip.h"

WidgetSystemToolTip::WidgetSystemToolTip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetSystemToolTip)
{
    ui->setupUi(this);
    ui->labelMacAddress->hide();
    ui->labelHelp->hide();
    ui->labelFailed->hide();
    ui->labelSuccess->hide();
    ui->labelFailedIcon->hide();
    ui->labelSuccessIcon->hide();
    ui->labelWarningIcon->hide();
    ui->labelRepairing->hide();
    ui->btnConfirm->hide();
    ui->btnWifiCancel->hide();
    ui->btnWifiConfirm->hide();
    ui->labelIsWifiConnect->hide();
    ui->labelWifiIPRecoverSuccess->hide();
    ui->labelMessage->hide();
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);

    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        setRepairStatus(0);
        close();
    });
    connect(ui->btnWifiConfirm,&QPushButton::clicked,this,[&]{
        setRepairStatus(1);
        emit signal_widgetSystemToolWifiIpRevovery();
    });
    connect(ui->btnWifiCancel,&QPushButton::clicked,this,[&]{
        setRepairStatus(0);
        close();
    });
}


WidgetSystemToolTip::~WidgetSystemToolTip()
{
    delete ui;
}



void WidgetSystemToolTip::setRepairStatus(int status)
{
    ui->labelMacAddress->hide();
    ui->labelHelp->hide();
    ui->labelFailed->hide();
    ui->labelSuccess->hide();
    ui->labelFailedIcon->hide();
    ui->labelSuccessIcon->hide();
    ui->labelWarningIcon->hide();
    ui->labelRepairing->hide();
    ui->btnConfirm->hide();
    ui->btnWifiConfirm->hide();
    ui->btnWifiCancel->hide();
    ui->labelIsWifiConnect->hide();
    ui->labelWifiIPRecoverSuccess->hide();
    ui->labelMessage->hide();
    //修复失败
    if(status == -1){
        ui->labelFailedIcon->show();
        ui->labelFailed->show();
        ui->labelHelp->show();
        ui->btnConfirm->show();
    }
    //初始化
    if(status == 0){

    }
    //修复中
    if(status == 1){
        ui->labelRepairing->show();
    }
    //成功
    if(status == 2){
        ui->labelSuccessIcon->show();
        ui->labelSuccess->show();
        ui->btnConfirm->show();
    }

    //成功并重启控制柜
    if(status == 21){
        ui->labelSuccessIcon->show();
        ui->labelSuccess->show();
        ui->btnConfirm->show();
        ui->labelMessage->show();
        ui->labelWarningIcon->show();
    }

    //成功并重启控制柜Mac显示地址
    if(status == 22){
        ui->labelMacAddress->show();
        ui->labelSuccessIcon->show();
        ui->labelSuccess->show();
        ui->btnConfirm->show();
        ui->labelMessage->show();
        ui->labelWarningIcon->show();
    }

    //默认ip确认是否wifi连接
    if(status == 3){
        ui->labelIsWifiConnect->show();
        ui->btnWifiConfirm->show();
        ui->btnWifiCancel->show();
    }
    //默认ip修复成功
    if(status == 4){
         ui->labelWifiIPRecoverSuccess->show();
         ui->btnConfirm->show();
    }

}

void WidgetSystemToolTip::setMacAddress(QString address)
{
    ui->labelMacAddress->setText("MAC:"+address);
}

void WidgetSystemToolTip::retranslate()
{
     ui->retranslateUi(this);
}

void WidgetSystemToolTip::mousePressEvent(QMouseEvent *e)
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

void WidgetSystemToolTip::mouseMoveEvent(QMouseEvent *e)
{
    if(mouse_press)
    {
        QPoint move_pos=e->globalPos();
        this->move(move_pos-move_point);
    }
}

void WidgetSystemToolTip::mouseReleaseEvent(QMouseEvent *e)
{
    mouse_press = false;
    this->setCursor(Qt::ArrowCursor);
}
void WidgetSystemToolTip::paintEvent(QPaintEvent *event)
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

bool WidgetSystemToolTip::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
