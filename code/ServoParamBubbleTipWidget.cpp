#include "ServoParamBubbleTipWidget.h"
#include "ui_ServoParamBubbleTipWidget.h"


#include <QStyleOption>
#include <QPainter>
#include <QTimer>
#include <QPropertyAnimation>
ServoParamBubbleTipWidget::ServoParamBubbleTipWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServoParamBubbleTipWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowModality(Qt::ApplicationModal); //禁用主窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明

    //延迟1000ms后，渐变隐藏窗口
    QTimer::singleShot(1000, this, [&]()
    {
       QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");//this代表是自己这个窗口使用这个效果
       connect(animation,&QPropertyAnimation::finished,this,[&]{
            close();
       });
       animation->setDuration(1000);//设置动画持续时间
       animation->setStartValue(1);
       animation->setEndValue(0);
       animation->start(QAbstractAnimation::DeleteWhenStopped);//开始动画        参数：停止时删除动画
    });
}

ServoParamBubbleTipWidget::~ServoParamBubbleTipWidget()
{
    delete ui;
}

void ServoParamBubbleTipWidget::setContent(QString text)
{
    ui->labelText->setText(text);
}

void ServoParamBubbleTipWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
