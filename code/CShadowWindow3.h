#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QPixmap>

class CShadowWindow3 : public QWidget
{
    Q_OBJECT

public:
    explicit CShadowWindow3(QWidget* pShadowWidget,QWidget *parent = Q_NULLPTR);
    virtual ~CShadowWindow3();

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
    void DragAndResizeWidget(CShadowWindow3::Direction direction, const QPoint& ptOffset);

private:
    QWidget* m_pShadowWidget;
    QPixmap m_bmpBackground;

    int m_iShadowSize;

    QPoint m_ptPreGlobal;
    Direction m_Direction;
};
