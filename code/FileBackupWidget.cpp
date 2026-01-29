#include "FileBackupWidget.h"
#include "ui_FileBackupWidget.h"

#include <QStyleOption>
#include <QPainter>
#include <QDateTime>
#include <QFileDialog>
#include "CommonData.h"
FileBackupWidget::FileBackupWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FileBackupWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    DobotType::ControllerType controllerType = CommonData::getControllerType();
    //获取当前时间
    QDateTime current_time = QDateTime::currentDateTime();
    QString currentTime = current_time.toString("yyyyMMdd");
    QString ControllCabinetSNCode = CommonData::getSettingsProductInfoHardwareInfo().ControllCabinetSNCode;
    QString backupFileName = controllerType.name+"_"+CommonData::getStrPropertiesCabinetType().name+"_"+CommonData::getCurrentSettingsVersion().control+"_"+currentTime+"_Backup";
    QRegExp expCRV("[A-Za-z0-9-\\_]");
    if(!ControllCabinetSNCode.isEmpty()&&ControllCabinetSNCode!="NULL"&&expCRV.exactMatch(ControllCabinetSNCode))
    {
        backupFileName = ControllCabinetSNCode+"_"+backupFileName;
    }

    ui->lineBackupFileName->setText(backupFileName);
    connect(ui->btnClose,&QPushButton::clicked,this,[&]
    {
        close();
    });

    connect(ui->btnBakupSelectDir,&QPushButton::clicked,this,[&]{
       QString backupSelectDir = QFileDialog::getExistingDirectory(nullptr,QString(),
                                                            "/home",
                                                            QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks);
        if(!backupSelectDir.isEmpty()){
            ui->lineSelectedBackupDir->setText(backupSelectDir);

        }
        ui->lineSelectedBackupDir->repaint();
        return;
    });

    connect(ui->btnStartBackup,&QPushButton::clicked,this,[&]{
        if(ui->lineSelectedBackupDir->text().isEmpty()){
            qDebug()<<" 未选择相应的备份位置";
            return ;
        }
        if(ui->lineBackupFileName->text().isEmpty()){
            qDebug()<<" 备份包的压缩包名称为空";
            return ;
        }
        emit signalFileBackupStart(ui->lineSelectedBackupDir->text(),ui->lineBackupFileName->text());
        close();
    });
}

FileBackupWidget::~FileBackupWidget()
{
    delete ui;
    this->deleteLater();
}
