#include "WidgetSystemLogExport.h"
#include "ui_WidgetSystemLogExport.h"

WidgetSystemLogExport::WidgetSystemLogExport(QWidget *parent) :
    QWidget(parent),
    mouse_press(false),
    ui(new Ui::WidgetSystemLogExport)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口

    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
//    setDiyPos();
    ui->comboBox->setGeometry(ui->comboBox->geometry());
    connect(ui->btnSelectExportDir,&QPushButton::clicked,this,&WidgetSystemLogExport::slot_selectExportDir);
    connect(ui->btnCancel,&QPushButton::clicked,this,&WidgetSystemLogExport::slot_cancel);
    connect(ui->btnConfirm,&QPushButton::clicked,this,&WidgetSystemLogExport::slot_confirm);


}

WidgetSystemLogExport::~WidgetSystemLogExport()
{
    delete ui;
}

void WidgetSystemLogExport::setComboxProjectList(QStringList projectList)
{

}

void WidgetSystemLogExport::setProjectList(QStringList projectList)
{
    m_projectList = projectList;
    ui->comboBox->addItems(m_projectList);
}

void WidgetSystemLogExport::retranslate()
{
    ui->retranslateUi(this);
}

void WidgetSystemLogExport::slot_selectExportDir()
{
    QString dir = QFileDialog::getExistingDirectory(nullptr,QString(),
                                                        "/home",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty()){
        m_saveDir = dir;
    }
    ui->lineExportDir->setText(m_saveDir);

}


void WidgetSystemLogExport::slot_cancel()
{
    ui->lineExportDir->setText(QString());
    m_saveDir = "";
    m_projectList.clear();
    close();
}

void WidgetSystemLogExport::slot_confirm()
{
    if(ui->comboBox->currentText().isEmpty()||ui->lineExportDir->text().isEmpty()){
        qDebug()<<"导出文件夹路径或者工程文件 不能为空 ";
    }
    emit signal_systemLogExport(ui->comboBox->currentText(),ui->lineExportDir->text());
    ui->comboBox->clear();
    ui->lineExportDir->setText(QString());
    m_saveDir = "";
    m_projectList.clear();
    close();

}

void WidgetSystemLogExport::setDiyPos()
{
    this->setGeometry(m_posDiyX,m_posDiyY-100,this->width(),this->height());
}

//void WidgetSystemLogExport::mousePressEvent(QMouseEvent *e)
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

//void WidgetSystemLogExport::mouseMoveEvent(QMouseEvent *e)
//{
//    if(mouse_press)
//    {
//        QPoint move_pos=e->globalPos();
//        this->move(move_pos-move_point);
//    }
//}

//void WidgetSystemLogExport::mouseReleaseEvent(QMouseEvent *e)
//{
//    mouse_press = false;
//    this->setCursor(Qt::ArrowCursor);
//}

void WidgetSystemLogExport::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool WidgetSystemLogExport::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
