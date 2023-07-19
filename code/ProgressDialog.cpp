#include "ProgressDialog.h"
#include "ui_ProgressDialog.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    mouse_press(false),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    connect(ui->btnConfirm,&QPushButton::clicked,this,&ProgressDialog::slot_progressConfirm);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowModality(Qt::ApplicationModal); //禁用主窗口
    connect(ui->btnClose,&QPushButton::clicked,this,&ProgressDialog::slot_closeDialog);
    ui->btnConfirm->hide();
    ui->labelDownloadCase->hide();
    ui->labelDownloadSpeed->hide();
    ui->labelProgress->hide();
    ui->btnClose->hide();
//    // 2、设置阴影边框;
//    shadowEffect = new QGraphicsDropShadowEffect(this);
//    // 阴影偏移
//    shadowEffect->setOffset(0, 0);
//    // 阴影颜色;
//    shadowEffect->setColor(QColor(0, 0, 0, 60));
//    // 阴影半径;
//    shadowEffect->setBlurRadius(18);
//    // 给窗口设置上当前的阴影效果;
//    this->setGraphicsEffect(shadowEffect);
//    this->setAttribute(Qt::WA_TranslucentBackground);
//    this->setContentsMargins(6,6,3,3);
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setProgressRange(int range)
{
    ui->btnConfirm->hide();
    ui->btnClose->hide();
    ui->progressBar->setRange(0,range);
    qDebug()<<"setProgressRange "<< range;
    m_range = range;
}

int ProgressDialog::progressRange()
{
    return m_range;
}

void ProgressDialog::setProgressValue(int progress)
{
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
    qDebug()<<"setProgressValue "<< progress;
}
//QProgressBar { /* all types of tool button */\n  border: none;\n  border-radius:10px;\n  text-align: top;\n  background: rgba(55,71,95,25);\n}\nQProgressBar::chunk\n{\n	border-radius: 10px;\n	background-color: rgb(48, 207, 88);\n}

void ProgressDialog::setHeadLabel(QString text)
{
    if(text.contains("download")){
        ui->labelDownloadCase->show();
        ui->labelDownloadSpeed->show();
        ui->labelProgress->setText(tr("下载中"));
        return;
    }
    if(text.contains("success")||text.contains("fail")){
        ui->btnConfirm->show();
//        ui->btnClose->show();
        ui->progressBar->setStyleSheet("QProgressBar { /* all types of tool button */\n  border: none;\n  border-radius: 10px;\n  text-align: top;\n background: rgba(55,71,95,25);\n}\nQProgressBar::chunk\n{\n	border-radius: 10px;\n	background-color: #3EB72A;\n}");
        ui->labelProgress->setText(ui->labelTitle->text());
        return;
    }
    if(text.contains("fail")){
        ui->progressBar->setStyleSheet("QProgressBar { /* all types of tool button */\n  border: none;\n  border-radius: 10px;\n  text-align: top;\n background: rgba(55,71,95,25);\n}\nQProgressBar::chunk\n{\n	border-radius: 10px;\n	background-color: #CE4949;\n}");
        ui->labelProgress->setText(ui->labelTitle->text());
        return;
    }

    if(text.contains("import")){
        ui->labelProgress->setText(tr("导入中 ")+QString("%1").arg(ui->progressBar->text()));
        ui->labelDownloadCase->hide();
        ui->labelDownloadSpeed->hide();
    }


}

void ProgressDialog::setTitle(QString title)
{
//    if(title.contains("成功")||title.contains("success")){

//    }

    ui->labelTitle->setText(title);
}

void ProgressDialog::setDownloadProgress(int progress, int total)
{

}

bool ProgressDialog::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

//void ProgressDialog::mousePressEvent(QMouseEvent *e)
//{
//    if(e->button()==Qt::LeftButton
//      && e->x() < this->width()
//      && e->y() < this->height())
//    {
//        this->setCursor(Qt::ClosedHandCursor);
//        mouse_press = true;
//    }
//    move_point=e->globalPos()-this->pos();
//}

//void ProgressDialog::mouseMoveEvent(QMouseEvent *e)
//{
//    if(mouse_press)
//    {
//        QPoint move_pos=e->globalPos();
//        this->move(move_pos-move_point);
//    }
//}

//void ProgressDialog::mouseReleaseEvent(QMouseEvent *e)
//{
//    mouse_press = false;
//    this->setCursor(Qt::ArrowCursor);
//}

void ProgressDialog::slot_progressConfirm()
{
    ui->btnConfirm->hide();
    ui->progressBar->setStyleSheet("QProgressBar { /* all types of tool button */\n  border: none;\n  border-radius: 10px;\n  text-align: top;\n background: rgba(55,71,95,25);\n}\nQProgressBar::chunk\n{\n	border-radius:10px;\n	background-color: #0047BB;\n}");
    close();
}

void ProgressDialog::slot_closeDialog()
{
    close();
}
