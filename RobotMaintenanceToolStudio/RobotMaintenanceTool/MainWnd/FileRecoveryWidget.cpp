#include "FileRecoveryWidget.h"
#include "ui_FileRecoveryWidget.h"
#include "Public.h"

FileRecoveryWidget::FileRecoveryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileRecoveryWidget)
{
    ui->setupUi(this);
    SET_STYLE_SHEET(FileRecoveryWidget);
}

FileRecoveryWidget::~FileRecoveryWidget()
{
    delete ui;
}
