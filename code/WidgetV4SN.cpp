#include "WidgetV4SN.h"
#include "ui_WidgetV4SN.h"

WidgetV4SN::WidgetV4SN(QWidget *parent) :
    QWidget(parent),
    mouse_press(false),
    ui(new Ui::WidgetV4SN)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);

    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        m_controlAndSN = ui->labelControl->text() + ui->lineBackupBag->text();
        ui->lineBackupBag->clear();
        m_v4ExportDir = ui->lineExportDir->text();
        ui->lineExportDir->clear();
        emit singal_wigdetV4SNToUpgradeDevice(m_controlAndSN,m_v4ExportDir);
        close();

    });

    connect(ui->btnCancel,&QPushButton::clicked,this,[&]{
        ui->lineBackupBag->clear();
        ui->lineExportDir->clear();
        close();
    });

    connect(ui->btnExport,&QPushButton::clicked,this,[&]{
        QString dir = QFileDialog::getExistingDirectory(nullptr,QString(),
                                                                       "/home",
                                                                       QFileDialog::ShowDirsOnly
                                                                       | QFileDialog::DontResolveSymlinks);
        if(!dir.isEmpty())
        {
            ui->lineExportDir->setText(dir);
        }
    });



}

WidgetV4SN::~WidgetV4SN()
{
    delete ui;
}

QString WidgetV4SN::getMessage()
{
    return m_controlAndSN;
}

QString WidgetV4SN::setControl(QString control)
{
    ui->labelControl->setText(control.mid(0,16));
}

void WidgetV4SN::retranslate()
{
    ui->retranslateUi(this);
}


void WidgetV4SN::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton
      && e->x() < this->width()
      && e->y() < this->height())
    {
        this->setCursor(Qt::ClosedHandCursor);
        mouse_press = true;
    }
    move_point=e->globalPos()-this->pos();
}

void WidgetV4SN::mouseMoveEvent(QMouseEvent *e)
{
    if(mouse_press)
    {
        QPoint move_pos=e->globalPos();
        this->move(move_pos-move_point);
    }
}

void WidgetV4SN::mouseReleaseEvent(QMouseEvent *e)
{
    mouse_press = false;
    this->setCursor(Qt::ArrowCursor);
}

void WidgetV4SN::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(5, 5, this->width() - 5 * 2, this->height() - 5 * 2, 3, 3);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(Qt::gray);
    for (int i = 0; i < 5; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRoundedRect(5 - i, 5 - i, this->width() - (5 - i) * 2, this->height() - (5 - i) * 2, 3 + i, 3 + i);
        color.setAlpha(80 - qSqrt(i) * 40);
        painter.setPen(color);
        painter.drawPath(path);
    }

}

bool WidgetV4SN::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
