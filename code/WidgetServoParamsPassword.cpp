#include "WidgetServoParamsPassword.h"
#include "ui_WidgetServoParamsPassword.h"

WidgetServoParamsPassword::WidgetServoParamsPassword(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetServoParamsPassword)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowModality(Qt::ApplicationModal); //禁用主窗口
    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        int correctPassword = m_currentRandom|4353;
        bool ok;
        int password = ui->linePassword->text().toInt(&ok);
        if(!ok)
        {
            ui->labelConfirmStatus->setText(tr("密码错误，请重新输入"));
            ui->labelConfirmStatus->show();
        }
        if(password==correctPassword)
        {
            ui->linePassword->clear();
            emit signal_servoParamsPassword(true);
        }
        else
        {
            ui->labelConfirmStatus->setText(tr("密码错误，请重新输入"));
            ui->labelConfirmStatus->show();
        }
    });
    connect(ui->btnClose,&QPushButton::clicked,this,[&]{
        ui->linePassword->clear();
        close();
    });
}

WidgetServoParamsPassword::~WidgetServoParamsPassword()
{
    delete ui;
}

void WidgetServoParamsPassword::initStatus()
{
    m_currentRandom = getRandom();
    ui->labelRandomNum->setText(tr("随机码：")+QString("%1").arg(m_currentRandom));
    ui->labelConfirmStatus->hide();
}

bool WidgetServoParamsPassword::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
int WidgetServoParamsPassword::getRandom()
{
    int randTime =  QTime::currentTime().msec();
    if(randTime<10)
    {
        return randTime*1000;
    }
    if(randTime<100)
    {
        return randTime*100;
    }
    if(randTime<1000)
    {
        return randTime*10;
    }
    return randTime;
}

void WidgetServoParamsPassword::paintEvent(QPaintEvent *event)
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
