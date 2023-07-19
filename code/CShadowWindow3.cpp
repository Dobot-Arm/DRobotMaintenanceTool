#include "CShadowWindow3.h"
#include <QPainter>
#include <QBrush>
#include <QtMath>
#include <QDebug>
#include <QApplication>
#include <QDir>

CShadowWindow3::CShadowWindow3(QWidget* pShadowWidget, QWidget *parent)
    : QWidget(parent),
    m_iShadowSize(12),
    m_pShadowWidget(pShadowWidget)
{
    //设置窗口无边框
    setWindowFlags(Qt::FramelessWindowHint);
    //设置窗口背景透明
    setAttribute(Qt::WA_TranslucentBackground, true);

    InitConstruct();
}

CShadowWindow3::~CShadowWindow3()
{
}

void CShadowWindow3::InitConstruct()
{
    QString strSkinFile = QString("%1%2skin%2Shadow.png").arg(qApp->applicationDirPath()).arg(QDir::separator());

    bool bFlag = m_bmpBackground.load(strSkinFile);

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

void CShadowWindow3::resizeEvent(QResizeEvent*)
{
    QSize sz = size();
    m_pShadowWidget->setGeometry(m_iShadowSize, m_iShadowSize,
        qAbs(sz.width() - m_iShadowSize * 2), qAbs(sz.height() - m_iShadowSize * 2));
}

void CShadowWindow3::paintEvent(QPaintEvent*)
{
    //下列方式绘制的圆角阴影，边角有毛刺
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

    const int iSize = m_iShadowSize*2;
    const QRect rcBmp = m_bmpBackground.rect();
    const QRect rcWnd = rect();
    //4个角落
    painter.drawPixmap(QRect(0, 0, iSize, iSize), m_bmpBackground, QRect(0, 0, iSize, iSize));
    painter.drawPixmap(QRect(rcWnd.width() - iSize, 0, iSize, iSize), m_bmpBackground,
        QRect(rcBmp.width() - iSize, 0, iSize, iSize));
    painter.drawPixmap(QRect(0, rcWnd.height()-iSize, iSize, iSize), m_bmpBackground,
        QRect(0, rcBmp.height() - iSize, iSize, iSize));
    painter.drawPixmap(QRect(rcWnd.width() - iSize, rcWnd.height() - iSize, iSize, iSize), m_bmpBackground,
        QRect(rcBmp.width() - iSize, rcBmp.height() - iSize, iSize, iSize));

    //上下边
    painter.drawPixmap(QRect(iSize,0,rcWnd.width()-iSize*2,iSize), m_bmpBackground,
        QRect(iSize, 0, rcBmp.width() - iSize * 2, iSize));
    painter.drawPixmap(QRect(iSize, rcWnd.height()-iSize, rcWnd.width() - iSize * 2, iSize), m_bmpBackground,
        QRect(iSize, rcBmp.height() - iSize, rcBmp.width() - iSize * 2, iSize));
    //左右边
    painter.drawPixmap(QRect(0, iSize, iSize, rcWnd.height() - iSize * 2), m_bmpBackground,
        QRect(0, iSize, iSize, rcBmp.height() - iSize * 2));
    painter.drawPixmap(QRect(rcWnd.width() - iSize, iSize, iSize, rcWnd.height() - iSize * 2),
        m_bmpBackground,QRect(rcBmp.width() - iSize, iSize, iSize, rcBmp.height() - iSize * 2));
    return;
}

CShadowWindow3::Direction CShadowWindow3::CalcDirection(const QPoint& pt)
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

void CShadowWindow3::SetMouseCursorStyle(Direction direction)
{
    switch (direction)
    {
    case TOP:
    case BOTTOM:
        setCursor(Qt::SizeVerCursor);
        break;
    case LEFT:
    case RIGHT:
        setCursor(Qt::SizeHorCursor);
        break;
    case LEFT_TOP:
    case RIGHT_BOTTOM:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case RIGHT_TOP:
    case LEFT_BOTTOM:
        setCursor(Qt::SizeBDiagCursor);
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

void CShadowWindow3::DragAndResizeWidget(CShadowWindow3::Direction direction, const QPoint& ptOffset)
{
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

void CShadowWindow3::mousePressEvent(QMouseEvent* pEvent)
{
    if (Qt::LeftButton == pEvent->button())
    {
        m_ptPreGlobal = pEvent->globalPos();
        m_Direction = CalcDirection(pEvent->pos());
        SetMouseCursorStyle(m_Direction);
    }
}

void CShadowWindow3::mouseReleaseEvent(QMouseEvent* pEvent)
{
    m_ptPreGlobal = QPoint();
    setCursor(Qt::ArrowCursor);
}

void CShadowWindow3::mouseMoveEvent(QMouseEvent* pEvent)
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
