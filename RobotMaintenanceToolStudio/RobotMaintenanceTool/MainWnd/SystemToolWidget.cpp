#include "SystemToolWidget.h"
#include "ui_SystemToolWidget.h"
#include "Public.h"

SystemToolWidget::SystemToolWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemToolWidget)
{
    ui->setupUi(this);
    SET_STYLE_SHEET(SystemToolWidget);
}

SystemToolWidget::~SystemToolWidget()
{
    delete ui;
}
