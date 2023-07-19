#include "MessageWidget.h"
#include "ui_MessageWidget.h"

MessageWidget::MessageWidget(QWidget *parent) :
    QWidget(parent),
    mouse_press(false),
    ui(new Ui::MessageWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口

    connect(ui->btnClose,&QPushButton::clicked,this,&MessageWidget::slot_closeWidget);
    connect(ui->btnConfirm,&QPushButton::clicked,this,&MessageWidget::slot_confirmWidget);
}

MessageWidget::~MessageWidget()
{
    delete ui;
}

void MessageWidget::setMessage(QString type, QString message)
{
    if(type == "warn"){
        ui->labelMessage->setStyleSheet("border:none;color: rgb(255, 0, 0);");
        ui->labelMessage->setText(message);
    }else{
        ui->labelMessage->setStyleSheet("border:none;color: rgb(0, 0, 255);");
        ui->labelMessage->setText(message);
    }
    if(type == "doing")
    {
        ui->btnConfirm->hide();
    }
    else
    {
        ui->btnConfirm->show();
    }
}

//void MessageWidget::mousePressEvent(QMouseEvent *e)
//{
//    if(e->button()==Qt::LeftButton
//      && e->x() < this->width()
//      && e->y() < this->height())
//    {
//        this->setCursor(Qt::ClosedHandCursor);
//        mouse_press = true;
//    }
//    move_point=e->globalPos()-this->pos();
//}

//void MessageWidget::mouseMoveEvent(QMouseEvent *e)
//{
//    if(mouse_press)
//    {
//        QPoint move_pos=e->globalPos();
//        this->move(move_pos-move_point);
//    }
//}

//void MessageWidget::mouseReleaseEvent(QMouseEvent *e)
//{
//    mouse_press = false;
//    this->setCursor(Qt::ArrowCursor);
//}

void MessageWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool MessageWidget::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}


void MessageWidget::slot_closeWidget()
{
    close();
}

void MessageWidget::slot_confirmWidget()
{
    close();
}
