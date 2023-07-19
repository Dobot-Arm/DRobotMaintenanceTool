#include "ControlPannelWidget.h"
#include "ui_ControlPannelWidget.h"
#include "Public.h"

ControlPannelWidget::ControlPannelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPannelWidget)
{
    ui->setupUi(this);
    SET_STYLE_SHEET(ControlPannelWidget);
}

ControlPannelWidget::~ControlPannelWidget()
{
    delete ui;
}
