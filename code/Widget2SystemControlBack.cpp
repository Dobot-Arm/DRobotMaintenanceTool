#include "Widget2SystemControlBack.h"
#include "ui_Widget2SystemControlBack.h"

Widget2SystemControlBack::Widget2SystemControlBack(QWidget *parent) :
    QWidget(parent),
    mouse_press(false),
    ui(new Ui::Widget2SystemControlBack)
{
    ui->setupUi(this);

    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);
    connect(ui->btnImport,&QPushButton::clicked,this,[&]{
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                        "/home",
                                                        tr(" (*.zip *.7z *.rar *.gz)"));
        if(!fileName.isEmpty()){
            ui->lineFilePath->setText(fileName);
        }
    });
    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{
        QString filePath = ui->lineFilePath->text();
        ui->lineFilePath->clear();
        emit signal_systemControlBack(filePath);
        close();
    });

    connect(ui->btnCancel,&QPushButton::clicked,this,[&]{
        ui->lineFilePath->clear();
        close();
    });

}

Widget2SystemControlBack::~Widget2SystemControlBack()
{
    delete ui;
}

void Widget2SystemControlBack::retranslate()
{
    ui->retranslateUi(this);
}

void Widget2SystemControlBack::mousePressEvent(QMouseEvent *e)
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

void Widget2SystemControlBack::mouseMoveEvent(QMouseEvent *e)
{
    if(mouse_press)
    {
        QPoint move_pos=e->globalPos();
        this->move(move_pos-move_point);
    }
}

void Widget2SystemControlBack::mouseReleaseEvent(QMouseEvent *e)
{
    mouse_press = false;
    this->setCursor(Qt::ArrowCursor);
}
void Widget2SystemControlBack::paintEvent(QPaintEvent *event)
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

bool Widget2SystemControlBack::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

