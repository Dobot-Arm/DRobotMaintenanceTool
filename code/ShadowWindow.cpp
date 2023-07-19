#include "ShadowWindow.h"
#include "ui_ShadowWindow.h"

ShadowWindow::ShadowWindow(QWidget *parent) :
    QWidget(parent,Qt::WindowStaysOnTopHint),
    m_topWidget(nullptr),
    m_widget(nullptr),
    ui(new Ui::ShadowWindow)
{
    ui->setupUi(this);
    init();
}

ShadowWindow::~ShadowWindow()
{
    delete ui;
}

void ShadowWindow::installWidget(QWidget *widget)
{
    if(widget == nullptr)
    {
        return;
    }
    //监控显示窗口事件
    widget->installEventFilter(this);
    m_widget = widget;

    connect(m_widget, &QWidget::destroyed, this, [=](){
            m_widget = nullptr;});
}

void ShadowWindow::setMaskColor(const QColor &color, float opacity)
{
    if(!color.isValid())
    {
        return;
    }

    QPalette palette = this->palette();
    //通常指窗口部件的背景色
    palette.setColor(QPalette::Window,color);
    this->setPalette(palette);

    setWindowOpacity(opacity);
}

void ShadowWindow::setTopWidget(QWidget *widget)
{
    //设置遮罩父窗口
    if(widget == nullptr)
    {
        return;
    }
    m_topWidget = widget;
}

void ShadowWindow::init()
{
    /*
    Qt::FramelessWindowHint
    1.产生一个无边框的窗口，无法移动和改变大小。
    Qt::Tool
    1.工具窗口，如果有父窗口，则工具窗口将始终保留在它的顶部。
    2.默认情况下，当应用程序处于非活动状态时，工具窗口则消失。
    */
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool |Qt::WindowSystemMenuHint);
    setMaskColor(QColor(0,0,0),0.6f);

    //1.返回具有键盘输入焦点的顶级窗口,如果没有应用程序窗口具有焦点，则返回0.
    //2.但是即使没有焦点，也可能有一个活动窗口。例如一个窗口没有小部件接收关键事件。
    m_topWidget = QApplication::activeWindow();
}

void ShadowWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    //确定显示的位置
    this->setGeometry(m_topWidget->geometry());
}

bool ShadowWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == m_widget)
    {
        if(event->type() == QEvent::Show)
        {
            //显示窗口显示，则遮罩显示，this就是遮罩本身
            this->show();
        }

        if(event->type() == QEvent::Hide)
        {
            this->hide();
        }
    }

    return QObject::eventFilter(watched, event);
}
