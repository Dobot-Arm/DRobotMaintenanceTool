#include "FileRecoveryWidget.h"
#include "ui_FileRecoveryWidget.h"

#include <QFileDialog>
FileRecoveryWidget::FileRecoveryWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FileRecoveryWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnClose,&QPushButton::clicked,this,[&]{
        close();
    });

    connect(ui->btnFileRevoverySelectDir,&QPushButton::clicked,this,[&]{
       QString fileName = QFileDialog::getOpenFileName(nullptr,QString(),
                                                            "/home",
                                                            tr("Zip (*.zip)"));
       if(!fileName.isEmpty()){
           ui->lineFileRecoveryDir->setText(fileName);
       }
    });

    connect(ui->btnFileRecoveryStart,&QPushButton::clicked,this,[&]{
        emit signalFileRecoveryStart(ui->lineFileRecoveryDir->text());
        close();
    });
}

FileRecoveryWidget::~FileRecoveryWidget()
{
    delete ui;
}

