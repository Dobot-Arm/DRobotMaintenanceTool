#include "WidgetSystemLogExport.h"
#include "ui_WidgetSystemLogExport.h"

WidgetSystemLogExport::WidgetSystemLogExport(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::WidgetSystemLogExport)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    ui->comboBox->setGeometry(ui->comboBox->geometry());
    ui->comboBox->setView(new QListView());
    connect(ui->btnSelectExportDir,&QPushButton::clicked,this,&WidgetSystemLogExport::slot_selectExportDir);
    connect(ui->btnCancel,&QPushButton::clicked,this,&WidgetSystemLogExport::close);
    connect(ui->btnConfirm,&QPushButton::clicked,this,&WidgetSystemLogExport::slot_confirm);

    ui->labelError->setText(QString());
}

WidgetSystemLogExport::~WidgetSystemLogExport()
{
    delete ui;
}

void WidgetSystemLogExport::setProjectList(QStringList projectList)
{
    m_projectList = projectList;
    ui->comboBox->addItems(m_projectList);
}

void WidgetSystemLogExport::show()
{
    raise();
    QWidget::show();
}

void WidgetSystemLogExport::slot_selectExportDir()
{
    QString dir = QFileDialog::getExistingDirectory(nullptr,QString(),
                                                        "/home",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
    ui->lineExportDir->setText(dir);
}

void WidgetSystemLogExport::slot_confirm()
{
    if(ui->comboBox->currentText().isEmpty()||ui->lineExportDir->text().isEmpty()){
        qDebug()<<"导出文件夹路径或者工程文件不能为空 ";
        ui->labelError->setText(tr("导出文件夹路径或者工程文件不能为空"));
        return;
    }
    emit signal_systemLogExport(ui->comboBox->currentText(),ui->lineExportDir->text());
    close();
}
