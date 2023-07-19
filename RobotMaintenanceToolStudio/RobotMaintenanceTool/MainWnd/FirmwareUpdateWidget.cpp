#include "FirmwareUpdateWidget.h"
#include "ui_FirmwareUpdateWidget.h"
#include "Public.h"

FirmwareUpdateWidget::FirmwareUpdateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FirmwareUpdateWidget)
{
    ui->setupUi(this);
    SET_STYLE_SHEET(FirmwareUpdateWidget);
}

FirmwareUpdateWidget::~FirmwareUpdateWidget()
{
    delete ui;
}
