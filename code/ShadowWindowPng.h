#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QPixmap>

class ShadowWindowPng : public QWidget
{
    Q_OBJECT

public:
    explicit ShadowWindowPng(QWidget* pShadowWidget,QWidget *parent = Q_NULLPTR);
    virtual ~ShadowWindowPng();
    int getShadowSize(){
        return m_iShadowSize;
    }
    void setEnableScale(bool isEnable);
protected:
    void paintEvent(QPaintEvent* pEvent) override;
    void resizeEvent(QResizeEvent* pEvent) override;
    void mousePressEvent(QMouseEvent* pEvent) override;
    void mouseReleaseEvent(QMouseEvent* pEvent) override;
    void mouseMoveEvent(QMouseEvent* pEvent) override;

    void InitConstruct();

    enum Direction
    {
        OUTER,
        LEFT_TOP,
        LEFT_BOTTOM,
        RIGHT_TOP,
        RIGHT_BOTTOM,
        CENTER,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };
    Direction CalcDirection(const QPoint& pt);
    void SetMouseCursorStyle(Direction direction);
    void DragAndResizeWidget(ShadowWindowPng::Direction direction, const QPoint& ptOffset);

private:
    QWidget* m_pShadowWidget;
    QPixmap m_bmpRectLeft;
    QPixmap m_bmpRectRight;
    QPixmap m_bmpRectTop;
    QPixmap m_bmpRectBottom;
    QPixmap m_bmpRoundTopRight;
    QPixmap m_bmpRoundTopLeft;
    QPixmap m_bmpRoundBottomRight;
    QPixmap m_bmpRoundBottomLeft;

    int m_iShadowSize;
    bool m_bEnableScale = true;

    QPoint m_ptPreGlobal;
    Direction m_Direction;
};
