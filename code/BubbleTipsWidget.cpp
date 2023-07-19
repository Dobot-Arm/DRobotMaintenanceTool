#include "BubbleTipsWidget.h"
#include "ui_BubbleTipsWidget.h"
#include <QPainter>

BubbleTipsWidget::BubbleTipsWidget(QWidget *parent)
    : BaseWidget(parent)
    , ui(new Ui::BubbleTipsWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowModality(Qt::ApplicationModal); //禁用主窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setDirect(DIRECT::DIRECT_BOTTOM, 0.75);

    connect(ui->btnClose,&QPushButton::clicked,this,[this]{
        this->close();
    });
    connect(ui->btnInterrupt,&QPushButton::clicked,this,[this]{
        emit signal_interruptUpgrade(true);
        this->close();
    },Qt::DirectConnection);
    connect(ui->btnCancel,&QPushButton::clicked,this,[this]{
        this->close();
    });
    QFont f;
    f.setFamily("Microsoft YaHei UI");
    f.setPixelSize(14);
    setContentFont(f);
    setContent("I am a default text");


    setLeftTopMargin(5,5);

    // 2、设置阴影边框;
    shadowEffect = new QGraphicsDropShadowEffect(this);
    // 阴影偏移
    shadowEffect->setOffset(0, 0);
    // 阴影颜色;
    shadowEffect->setColor(QColor(0, 0, 0, 60));
    // 阴影半径;
    shadowEffect->setBlurRadius(18);
    // 给窗口设置上当前的阴影效果;
    this->setGraphicsEffect(shadowEffect);
    this->setAttribute(Qt::WA_TranslucentBackground);

}

BubbleTipsWidget::~BubbleTipsWidget()
{
    delete ui;
}


void BubbleTipsWidget::setBackColor(int r, int g, int b, int a)
{
    m_backColor = QColor(r, g, b, a);
}

void BubbleTipsWidget::setDirect(DIRECT direct, double size)
{
    m_direct = direct;
    m_posSize = size;
}

void BubbleTipsWidget::setContentFont(QFont font)
{
    ui->label->setFont(font);
}

void BubbleTipsWidget::setContent(const QString &content)
{
    ui->label->setText(content);

}

void BubbleTipsWidget::setLeftTopMargin(int leftMargin, int topMargin)
{
    m_leftMargin = leftMargin;
    m_topMargin = topMargin;
    this->setContentsMargins(m_leftMargin + TRANSPARENT_LENGTH,
                             m_topMargin + TRANSPARENT_LENGTH,
                             m_leftMargin + TRANSPARENT_LENGTH,
                             m_topMargin + TRANSPARENT_LENGTH);
}

void BubbleTipsWidget::setPos(int x,int y)
{
    this->move(x, y);
}

void BubbleTipsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);


    painter.setPen(Qt::NoPen);
    painter.setBrush(m_backColor);

    //相对于当前布局的起点坐标
    painter.drawRoundedRect(TRANSPARENT_LENGTH, TRANSPARENT_LENGTH,
                            width() - TRANSPARENT_LENGTH * 2,
                            height() - TRANSPARENT_LENGTH * 2, 4, 4);
    QPointF points[3];

    switch (m_direct) {
    case DIRECT::DIRECT_LEFT: {
        points[0] = QPointF(TRANSPARENT_LENGTH,
                            height() * m_posSize - DEF_TRIANGLE_HEIGHT);
        points[1] = QPointF(TRANSPARENT_LENGTH - DEF_TRIANGLE_HEIGHT,
                            height() * m_posSize);
        points[2] = QPointF(TRANSPARENT_LENGTH,
                            height() * m_posSize + DEF_TRIANGLE_HEIGHT);
        break;
    }

    case DIRECT::DIRECT_TOP: {
        points[0] = QPointF(width() * m_posSize - DEF_TRIANGLE_HEIGHT,
                            TRANSPARENT_LENGTH);
        points[1] = QPointF(width() * m_posSize,
                            TRANSPARENT_LENGTH - DEF_TRIANGLE_HEIGHT);
        points[2] = QPointF(width() * m_posSize + DEF_TRIANGLE_HEIGHT,
                            TRANSPARENT_LENGTH);
        break;
    }

    case DIRECT::DIRECT_RIGHT: {
        points[0] = QPointF(width() - TRANSPARENT_LENGTH,
                            height() * m_posSize - DEF_TRIANGLE_HEIGHT);
        points[1] = QPointF(width() - DEF_TRIANGLE_HEIGHT, height() * m_posSize);
        points[2] = QPointF(width() - TRANSPARENT_LENGTH,
                            height() * m_posSize + DEF_TRIANGLE_HEIGHT);
        break;
    }

    case DIRECT::DIRECT_BOTTOM: {
        points[0] = QPointF(width() * m_posSize - DEF_TRIANGLE_HEIGHT,
                            height() - TRANSPARENT_LENGTH);
        points[1] = QPointF(width() * m_posSize, height() - DEF_TRIANGLE_HEIGHT);
        points[2] = QPointF(width() * m_posSize + DEF_TRIANGLE_HEIGHT,
                            height() - TRANSPARENT_LENGTH);
        break;
    }

    default:
        break;
    }

    painter.drawPolygon(points, 3);
}


bool BubbleTipsWidget::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}


