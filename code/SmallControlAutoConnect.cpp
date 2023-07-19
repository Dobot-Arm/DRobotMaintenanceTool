#include "SmallControlAutoConnect.h"
#include "ui_SmallControlAutoConnect.h"

SmallControlAutoConnect::SmallControlAutoConnect(QWidget *parent) :
    QWidget(parent),
    mouse_press(false),
    ui(new Ui::SmallControlAutoConnect)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowModality(Qt::ApplicationModal); //禁用主窗口

    connect(ui->btnClose,&QPushButton::clicked,this,&SmallControlAutoConnect::slot_close);

    initStatus();
}

SmallControlAutoConnect::~SmallControlAutoConnect()
{
    delete ui;
}

void SmallControlAutoConnect::setUpgadeStatus(int status)
{
    ui->labelDoing->hide();
    ui->labelMessage->hide();
    ui->btnClose->show();
    if(status == 2){
        ui->labelStatus->setText(tr("固件升级成功！"));
        ui->labelStatusIcon->setStyleSheet("QLabel{\n	image: url(:/image/images2/Icon_doingSuccessed_big.png);\n	border-radius: none;\n}");
    }
    if(status == -1){
        ui->labelStatus->setText(tr("固件升级失败！"));
        ui->labelStatusIcon->setStyleSheet("QLabel{\n	image: url(:/image/images2/Icon_doingFailed_big.png);\n	border-radius: none;\n}");
    }
    ui->labelStatus->show();
    ui->labelStatusIcon->show();
}

void SmallControlAutoConnect::initStatus()
{
    //加载gif图片
    QMovie *movie = new QMovie(":/image/images2/loading.gif");
    ui->labelDoing->setMovie(movie);
    movie->start();
    ui->labelDoing->show();
    ui->labelMessage->show();
    ui->btnClose->hide();
    ui->labelStatus->hide();
    ui->labelStatusIcon->hide();
}


void SmallControlAutoConnect::slot_close()
{
    ui->labelDoing->show();
    ui->labelMessage->show();

    ui->btnClose->hide();
    ui->labelStatus->hide();
    ui->labelStatusIcon->hide();
    close();
}

void SmallControlAutoConnect::mousePressEvent(QMouseEvent *e)
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

void SmallControlAutoConnect::mouseMoveEvent(QMouseEvent *e)
{
    if(mouse_press)
    {
        QPoint move_pos=e->globalPos();
        this->move(move_pos-move_point);
    }
}

void SmallControlAutoConnect::mouseReleaseEvent(QMouseEvent *e)
{
    mouse_press = false;
    this->setCursor(Qt::ArrowCursor);
}

void SmallControlAutoConnect::paintEvent(QPaintEvent *event)
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

bool SmallControlAutoConnect::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
