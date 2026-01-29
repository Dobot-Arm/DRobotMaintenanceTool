#include "Widget2ProgressDialog.h"
#include "ui_Widget2ProgressDialog.h"

Widget2ProgressDialog::Widget2ProgressDialog(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::Widget2ProgressDialog)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    ui->btnClose->hide();
    connect(ui->btnClose,&QPushButton::clicked,this,&Widget2ProgressDialog::close);
    connect(ui->btnConfirm,&QPushButton::clicked,this,&Widget2ProgressDialog::close);
    ui->btnConfirm->hide();
    ui->labelDownloadCase->hide();
    ui->labelDownloadSpeed->hide();
}

Widget2ProgressDialog::~Widget2ProgressDialog()
{
    delete ui;
}

void Widget2ProgressDialog::setProgressRange(int range)
{
    ui->btnConfirm->hide();
    ui->progressBar->setRange(0,range);
    qDebug()<<"setProgressRange "<< range;
    m_range = range;
}

int Widget2ProgressDialog::progressRange()
{
    return m_range;
}

void Widget2ProgressDialog::setProgressValue(int progress)
{
    m_value = progress;
    double speed = ((double)progress - (double)ui->progressBar->value())/1024 ;
    ui->progressBar->setValue(progress);
    ui->progressBar->setStyleSheet("QProgressBar { /* all types of tool button */\n  border: none;\n  border-radius: 10px;\n  text-align: top;\n background: rgba(55,71,95,25);\n}\nQProgressBar::chunk\n{\n	border-radius: 10px;\n	background-color: #0047BB;\n}");
    ui->progressBar->setTextVisible(false);
    ui->labelDownloadSpeed->setText(QString(tr("下载速度")+"：%1 KB/s").arg(QString::number(speed,20,2)));
    ui->labelDownloadCase->setText(QString("%1MB/ %2MB")
                                   .arg(progress/(1024*1024))
                                   .arg(ui->progressBar->maximum()/(1024*1024)));
    if(ui->labelProgress->text().contains("import")){
        ui->labelProgress->setText(QString("importing %1").arg(ui->progressBar->text()));
    }

    if(m_functionName.contains("backup")){
        ui->labelProgress->setText(tr("文件备份中...")+QString("%1").arg(ui->progressBar->text()));
        ui->labelProgress->repaint();
    }

    if(m_functionName.contains("fileRecovery")){
        ui->labelProgress->setText(tr("文件恢复中...")+QString("%1").arg(ui->progressBar->text()));
        ui->labelProgress->repaint();
    }
    qDebug()<<"setProgressValue "<< progress;
}

void Widget2ProgressDialog::setWarnMessage(QString text)
{
    ui->labelWarning->setText(text);
}
//QProgressBar { /* all types of tool button */\n  border: none;\n  border-radius: 10px;\n  text-align: top;\n  background: rgba(55,71,95,25);\n}\nQProgressBar::chunk\n{\n	border-radius: 10px;\n	background-color: rgb(48, 207, 88);\n}

void Widget2ProgressDialog::setHeadLabel(QString text)
{
    m_functionName = text;
    if(text.contains("success")){
        ui->labelProgress->setText(ui->labelTitle->text()+" "+tr("完成"));
        ui->btnConfirm->show();
        ui->progressBar->setStyleSheet("QProgressBar { /* all types of tool button */\n  border: none;\n  border-radius: 10px;\n  text-align: top;\n background: rgba(55,71,95,25);\n}\nQProgressBar::chunk\n{\n	border-radius: 10px;\n	background-color: #3EB72A;\n}");
    }
    if(text.contains("fail")){
        ui->labelProgress->setText(ui->labelTitle->text()+" "+tr("失败"));
        ui->btnConfirm->show();
        ui->progressBar->setStyleSheet("QProgressBar { /* all types of tool button */\n  border: none;\n  border-radius: 10px;\n  text-align: top;\n background: rgba(55,71,95,25);\n}\nQProgressBar::chunk\n{\n	border-radius: 10px;\n	background-color: #CE4949;\n}");
    }
    if(text.contains("download")){
        ui->labelDownloadCase->show();
        ui->labelDownloadSpeed->show();
    }else{
        ui->labelDownloadCase->hide();
        ui->labelDownloadSpeed->hide();
    }
    if(text.contains("import")){
        ui->labelProgress->setText(QString("importing %1").arg(ui->progressBar->text()));
    }
    if(text.contains("export")){
        ui->labelWarning->hide();
        ui->labelProgress->setText(tr("文件导出中..."));
    }
    if(text.contains("backup")){
        ui->labelProgress->setText(tr("文件备份中..."));
        ui->labelProgress->repaint();
    }
    if(text.contains("fileRecovery")){
        ui->labelProgress->setText(tr("文件恢复中..."));
    }

    if(text.contains("export")){
        ui->labelProgress->setText(tr("日志导出中..."));
    }
}

void Widget2ProgressDialog::setTitle(QString title)
{
    ui->labelTitle->setText(title);
}

void Widget2ProgressDialog::show()
{
    raise();
    QWidget::show();
}

void Widget2ProgressDialog::slot_progressConfirm()
{
    ui->btnConfirm->hide();
    ui->progressBar->setStyleSheet("QProgressBar { /* all types of tool button */\n  border: none;\n  border-radius: 10px;\n  text-align: top;\n background: rgba(55,71,95,25);\n}\nQProgressBar::chunk\n{\n	border-radius: 10px;\n	background-color: #0047BB;\n}");
    close();
}

bool Widget2ProgressDialog::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
