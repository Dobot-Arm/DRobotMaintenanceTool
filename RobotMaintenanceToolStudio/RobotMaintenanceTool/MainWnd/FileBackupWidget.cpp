#include "FileBackupWidget.h"
#include "ui_FileBackupWidget.h"
#include "Public.h"

FileBackupWidget::FileBackupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileBackupWidget)
{
    ui->setupUi(this);
    SET_STYLE_SHEET(FileBackupWidget);
}

FileBackupWidget::~FileBackupWidget()
{
    delete ui;
}
