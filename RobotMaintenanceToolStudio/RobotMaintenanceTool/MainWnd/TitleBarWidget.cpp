#include "TitleBarWidget.h"
#include "ui_TitleBarWidget.h"
#include "Public.h"

TitleBarWidget::TitleBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleBarWidget)
{
    ui->setupUi(this);
    SET_STYLE_SHEET(TitleBarWidget);
}

TitleBarWidget::~TitleBarWidget()
{
    delete ui;
}
