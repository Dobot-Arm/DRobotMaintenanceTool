#include "ShadowWindowPng.h"
#include <QPainter>
#include <QBrush>
#include <QtMath>
#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QStyleOption>
#include <QPainter>

ShadowWindowPng::ShadowWindowPng(QWidget* pShadowWidget, QWidget *parent)
    : QWidget(parent),
    m_iShadowSize(32),
    m_pShadowWidget(pShadowWidget)
{
    //设置窗口无边框
    setWindowFlags(Qt::FramelessWindowHint);
    //设置窗口背景透明
    setAttribute(Qt::WA_TranslucentBackground, true);

    setWindowModality(Qt::ApplicationModal); //禁用主窗口

    InitConstruct();
}

ShadowWindowPng::~ShadowWindowPng()
{
}

void ShadowWindowPng::setEnableScale(bool isEnable)
{
    m_bEnableScale = isEnable;
}

void ShadowWindowPng::InitConstruct()
{
    QMatrix mt;
    mt.rotate(90);
    bool bFlag = m_bmpRectRight.load(":/image/images2/shawdow_straight.png");
    m_bmpRectBottom = m_bmpRectRight.transformed(mt);
    m_bmpRectLeft = m_bmpRectBottom.transformed(mt);
    m_bmpRectTop = m_bmpRectLeft.transformed(mt);

    m_bmpRoundTopRight.load(":/image/images2/shawdow_corner.png");
    m_bmpRoundBottomRight = m_bmpRoundTopRight.transformed(mt);
    m_bmpRoundBottomLeft = m_bmpRoundBottomRight.transformed(mt);
    m_bmpRoundTopLeft = m_bmpRoundBottomLeft.transformed(mt);

    //关联一个父窗口
    m_pShadowWidget->setParent(this);

    //设置阴影窗口大小
    const QSize sz = m_pShadowWidget->size();
    resize(m_iShadowSize + sz.width(), m_iShadowSize + sz.height());
    m_pShadowWidget->move(m_iShadowSize, m_iShadowSize);

    /*当鼠标不按下时，qt为了节省消耗，不会响应mousemove事件，所以需要设置true，这样即使不按下鼠标也会收到鼠标移动事件
    子窗口也要设置，是因为鼠标移动事件发生在了子窗口上，被拦截了
    */
    setMouseTracking(true);
    m_pShadowWidget->setMouseTracking(true);
}

void ShadowWindowPng::resizeEvent(QResizeEvent*)
{
    QSize sz = size();
    m_pShadowWidget->setGeometry(m_iShadowSize, m_iShadowSize,
        qAbs(sz.width() - m_iShadowSize * 2), qAbs(sz.height() - m_iShadowSize * 2));
}

void ShadowWindowPng::paintEvent(QPaintEvent*)
{
    //下列方式绘制的圆角阴影，边角有毛刺
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    const QRect rcWnd = rect();
    //4个角落
    painter.drawPixmap(QRect(0, 0, m_iShadowSize, m_iShadowSize),
                       m_bmpRoundTopLeft, QRect(0, 0, m_iShadowSize, m_iShadowSize));
    painter.drawPixmap(QRect(rcWnd.width()-m_iShadowSize, 0, m_iShadowSize, m_iShadowSize),
                       m_bmpRoundTopRight,QRect(0, 0, m_iShadowSize, m_iShadowSize));
    painter.drawPixmap(QRect(0, rcWnd.height()-m_iShadowSize, m_iShadowSize, m_iShadowSize),
                       m_bmpRoundBottomLeft, QRect(0, 0, m_iShadowSize, m_iShadowSize));
    painter.drawPixmap(QRect(rcWnd.width()-m_iShadowSize, rcWnd.height()-m_iShadowSize, m_iShadowSize, m_iShadowSize),
                       m_bmpRoundBottomRight,QRect(0, 0, m_iShadowSize, m_iShadowSize));

    //上下边
    painter.drawPixmap(QRect(m_iShadowSize,0,rcWnd.width()- m_iShadowSize*2,m_iShadowSize),
                       m_bmpRectTop,QRect(0, 0, m_iShadowSize, m_iShadowSize));
    painter.drawPixmap(QRect(m_iShadowSize, rcWnd.height()-m_iShadowSize,
                             rcWnd.width()- m_iShadowSize*2,m_iShadowSize),
                       m_bmpRectBottom,QRect(0, 0, m_iShadowSize, m_iShadowSize));

    //左右边
    painter.drawPixmap(QRect(0, m_iShadowSize, m_iShadowSize, rcWnd.height() - m_iShadowSize*2),
                       m_bmpRectLeft, QRect(0, 0, m_iShadowSize, m_iShadowSize));
    painter.drawPixmap(QRect(rcWnd.width() - m_iShadowSize, m_iShadowSize, m_iShadowSize, rcWnd.height() - m_iShadowSize * 2),
                       m_bmpRectRight,QRect(0, 0, m_iShadowSize, m_iShadowSize));
    return;
}

ShadowWindowPng::Direction ShadowWindowPng::CalcDirection(const QPoint& pt)
{
    if (pt.x() < m_iShadowSize && pt.y() < m_iShadowSize)
    {
        return LEFT_TOP;
    }
    else if (pt.x() < m_iShadowSize
        &&pt.y() > m_iShadowSize && pt.y() < rect().height() - m_iShadowSize)
    {
        return LEFT;
    }
    else if (pt.x() < m_iShadowSize &&pt.y() > rect().height() - m_iShadowSize)
    {
        return LEFT_BOTTOM;
    }
    else if (pt.x() > rect().width() - m_iShadowSize && pt.y() < m_iShadowSize)
    {
        return RIGHT_TOP;
    }
    else if (pt.x() > rect().width() - m_iShadowSize
        && pt.y() > m_iShadowSize && pt.y() < rect().height() - m_iShadowSize)
    {
        return RIGHT;
    }
    else if (pt.x() > rect().width() - m_iShadowSize
        && pt.y() > rect().height() - m_iShadowSize)
    {
        return RIGHT_BOTTOM;
    }
    else if (pt.x() > m_iShadowSize&&pt.x() < rect().width() - m_iShadowSize
        &&pt.y() < m_iShadowSize)
    {
        return TOP;
    }
    else if (pt.x() > m_iShadowSize&&pt.x() < rect().width() - m_iShadowSize
        &&pt.y() > rect().height() - m_iShadowSize)
    {
        return BOTTOM;
    }
    else if (pt.x() >= m_iShadowSize&&pt.y() >= m_iShadowSize
        &&pt.x() <= rect().width() - m_iShadowSize&&pt.y() <= rect().height() - m_iShadowSize)
    {
        return CENTER;
    }
    return OUTER;
}

void ShadowWindowPng::SetMouseCursorStyle(Direction direction)
{
    switch (direction)
    {
    case TOP:
    case BOTTOM:
        setCursor(m_bEnableScale?Qt::SizeVerCursor:Qt::ArrowCursor);
        break;
    case LEFT:
    case RIGHT:
        setCursor(m_bEnableScale?Qt::SizeHorCursor:Qt::ArrowCursor);
        break;
    case LEFT_TOP:
    case RIGHT_BOTTOM:
        setCursor(m_bEnableScale?Qt::SizeFDiagCursor:Qt::ArrowCursor);
        break;
    case RIGHT_TOP:
    case LEFT_BOTTOM:
        setCursor(m_bEnableScale?Qt::SizeBDiagCursor:Qt::ArrowCursor);
        break;
    case CENTER:
        if (m_ptPreGlobal.isNull())
        {
            setCursor(Qt::ArrowCursor);
        }
        else
        {
            setCursor(Qt::SizeAllCursor);
        }
        break;
    default:
        setCursor(Qt::ArrowCursor);
    }
}

void ShadowWindowPng::DragAndResizeWidget(ShadowWindowPng::Direction direction, const QPoint& ptOffset)
{
    if(!m_bEnableScale)
    {
        return;
    }
    QRect rect = geometry();
    switch (direction)
    {
    case TOP:
        rect.setTop(rect.top() + ptOffset.y());
        break;
    case BOTTOM:
        rect.setBottom(rect.bottom() + ptOffset.y());
        break;
    case LEFT:
        rect.setLeft(rect.left() + ptOffset.x());
        break;
    case RIGHT:
        rect.setRight(rect.right() + ptOffset.x());
        break;
    case LEFT_TOP:
        rect.setTopLeft(rect.topLeft() + ptOffset);
        break;
    case RIGHT_BOTTOM:
        rect.setBottomRight(rect.bottomRight() + ptOffset);
        break;
    case RIGHT_TOP:
        rect.setTopRight(rect.topRight() + ptOffset);
        break;
    case LEFT_BOTTOM:
        rect.setBottomLeft(rect.bottomLeft() + ptOffset);
        break;
    case CENTER:
        break;
    default:;
    }
    if (rect.width() < m_iShadowSize * 3 || rect.height() < m_iShadowSize * 3)
    {
        return;
    }
    setGeometry(rect);
    return;
}

void ShadowWindowPng::mousePressEvent(QMouseEvent* pEvent)
{
    if (Qt::LeftButton == pEvent->button())
    {
        m_ptPreGlobal = pEvent->globalPos();
        m_Direction = CalcDirection(pEvent->pos());
        SetMouseCursorStyle(m_Direction);
    }
}

void ShadowWindowPng::mouseReleaseEvent(QMouseEvent* pEvent)
{
    m_ptPreGlobal = QPoint();
    setCursor(Qt::ArrowCursor);
}

void ShadowWindowPng::mouseMoveEvent(QMouseEvent* pEvent)
{
    if (Qt::LeftButton&pEvent->buttons())
    {
        SetMouseCursorStyle(m_Direction);
        const QPoint ptNow = pEvent->globalPos();
        const QPoint ptOffset = ptNow - m_ptPreGlobal;
        m_ptPreGlobal = ptNow;
        if (Direction::CENTER == m_Direction)
        {
            move(pos() + ptOffset);
        }
        else
        {
            DragAndResizeWidget(m_Direction, ptOffset);
        }
    }
    else
    {
        SetMouseCursorStyle(CalcDirection(pEvent->pos()));
    }
}
