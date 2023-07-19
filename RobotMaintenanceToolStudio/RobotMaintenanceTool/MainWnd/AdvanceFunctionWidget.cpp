#include "AdvanceFunctionWidget.h"
#include "ui_AdvanceFunctionWidget.h"

AdvanceFunctionWidget::AdvanceFunctionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdvanceFunctionWidget)
{
    ui->setupUi(this);
}

AdvanceFunctionWidget::~AdvanceFunctionWidget()
{
    delete ui;
}
