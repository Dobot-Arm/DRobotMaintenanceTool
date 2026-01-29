#include "SNAlarmBubbleTipWidget.h"
#include "ui_SNAlarmBubbleTipWidget.h"
#include "Define.h"

SNAlarmBubbleTipWidget::SNAlarmBubbleTipWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::SNAlarmBubbleTipWidget)
{
    ui->setupUi(this);
    SET_STYLE_SHEET(SNAlarmBubbleTipWidget);
    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
}

SNAlarmBubbleTipWidget::~SNAlarmBubbleTipWidget()
{
    delete ui;
}
