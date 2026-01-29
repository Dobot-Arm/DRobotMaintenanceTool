#include "WidgetToast.h"
#include "ui_WidgetToast.h"
#include <QFontMetrics>
#include "MainWidget2.h"
#include "Define.h"
#include <QPropertyAnimation>

WidgetToast::WidgetToast(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::WidgetToast)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    //setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    ui->labelIcon->setText("");
    m_pTimer = new QTimer(this);
    m_pTimer->setSingleShot(true);
    connect(m_pTimer, &QTimer::timeout, this, [this]{
        QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
        connect(animation,&QPropertyAnimation::finished,this,&WidgetToast::close);
        animation->setDuration(1000);
        animation->setStartValue(1);
        animation->setEndValue(0);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

WidgetToast::~WidgetToast()
{
    delete ui;
}

void WidgetToast::ok(const QString &strTxt, int iDurationsMillseconds)
{
    auto pToast = new WidgetToast(PublicgGetMainWindow());
    pToast->setText(strTxt, 1, iDurationsMillseconds);
}

void WidgetToast::fail(const QString &strTxt, int iDurationsMillseconds)
{
    auto pToast = new WidgetToast(PublicgGetMainWindow());
    pToast->setText(strTxt, 2, iDurationsMillseconds);
}

void WidgetToast::warn(const QString &strTxt, int iDurationsMillseconds)
{
    auto pToast = new WidgetToast(PublicgGetMainWindow());
    pToast->setText(strTxt, 3, iDurationsMillseconds);
}

void WidgetToast::setText(const QString &strTxt, int iType, int iDurationsMillseconds)
{
    QFontMetrics mt(ui->labelInfo->font());
    QRect rc = mt.boundingRect(QRect(0,0,1,1),Qt::TextSingleLine,strTxt);
    rc.setWidth(rc.width()+60+ui->labelIcon->width());
    rc.setHeight(rc.height()+20);

    ui->labelInfo->setText(strTxt);
    this->style()->unpolish(this);
    this->style()->unpolish(ui->labelIcon);
    if (1==iType){
        ui->labelIcon->setProperty("state", QVariant("ok"));
        setStyleSheet(".WidgetToast{background: #F0FCEF;border: 1px solid #3EB72A;border-radius: 3px;}");
    }
    else if (2==iType){
        ui->labelIcon->setProperty("state", QVariant("fail"));
        setStyleSheet(".WidgetToast{background: #FFE0E0;border: 1px solid #CE4949;border-radius: 3px;}");
    }
    else if (3==iType){
        ui->labelIcon->setProperty("state", QVariant("warn"));
        setStyleSheet(".WidgetToast{background: #FFF6D4;border: 1px solid #EEC01C;border-radius: 3px;}");
    }
    this->style()->polish(ui->labelIcon);
    this->style()->polish(this);

    auto pWidget = PublicgGetMainWindow();
    QPoint pt((pWidget->width()-rc.width())/2, 10);
    this->setGeometry(pt.x(), pt.y(), rc.width(),rc.height());
    m_pTimer->start(iDurationsMillseconds);
    show();
}
