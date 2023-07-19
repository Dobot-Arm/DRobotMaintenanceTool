#include "ServoParameterWidget.h"
#include "ui_ServoParameterWidget.h"
#include "Public.h"

ServoParameterWidget::ServoParameterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServoParameterWidget)
{
    ui->setupUi(this);
    SET_STYLE_SHEET(ServoParameterWidget);
}

ServoParameterWidget::~ServoParameterWidget()
{
    delete ui;
}
