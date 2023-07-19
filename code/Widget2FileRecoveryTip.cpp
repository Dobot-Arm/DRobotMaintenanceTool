#include "Widget2FileRecoveryTip.h"
#include "ui_Widget2FileRecoveryTip.h"

Widget2FileRecoveryTip::Widget2FileRecoveryTip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget2FileRecoveryTip)
{
    ui->setupUi(this);

    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowModality(Qt::ApplicationModal); //禁用主窗口
//    ui->labelIsRecovery->hide();
//    ui->btnRecoveryConfirm->hide();
//    ui->btnCancel->hide();
    ui->labelRecoveryFail->hide();
    ui->btnRecoveryFailConfirm->hide();


    connect(ui->btnCancel,&QPushButton::clicked,this,[&]{
        close();
    });

    connect(ui->btnRecoveryConfirmStart,&QPushButton::clicked,this,[&]{
        emit signalsFileRecoveryStart();
    });

    connect(ui->btnRecoveryFailConfirm,&QPushButton::clicked,this,[&]{
        setWidgetStatus(true);
        close();
    });
}

Widget2FileRecoveryTip::~Widget2FileRecoveryTip()
{
    delete ui;
}



void Widget2FileRecoveryTip::setWidgetStatus(bool isFinish,QString msg)
{
    if(isFinish){
        ui->labelIsRecovery->show();
        ui->btnRecoveryConfirmStart->show();
        ui->btnCancel->show();

        ui->btnRecoveryFailConfirm->hide();
        ui->labelRecoveryFail->hide();
    }else{
        if(!msg.isEmpty()){
            ui->labelRecoveryFail->setText(msg+tr("不相同，恢复失败"));
        }
        ui->labelIsRecovery->hide();
        ui->btnRecoveryConfirmStart->hide();
        ui->btnCancel->hide();

        ui->btnRecoveryFailConfirm->show();
        ui->labelRecoveryFail->show();
    }
}

void Widget2FileRecoveryTip::mousePressEvent(QMouseEvent *e)
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

void Widget2FileRecoveryTip::mouseMoveEvent(QMouseEvent *e)
{
    if(mouse_press)
    {
        QPoint move_pos=e->globalPos();
        this->move(move_pos-move_point);
    }
}

void Widget2FileRecoveryTip::mouseReleaseEvent(QMouseEvent *e)
{
    mouse_press = false;
    this->setCursor(Qt::ArrowCursor);
}

void Widget2FileRecoveryTip::paintEvent(QPaintEvent *event)
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

bool Widget2FileRecoveryTip::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

