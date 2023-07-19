#ifndef _BUBBLETIPSWIDGET_H_
#define _BUBBLETIPSWIDGET_H_

#include <QWidget>
#include <QGraphicsDropShadowEffect>

#include "baseUI/BaseWidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class BubbleTipsWidget; }
QT_END_NAMESPACE

// 三角形的高度
#define DEF_TRIANGLE_HEIGHT 10
// 透明宽度
#define TRANSPARENT_LENGTH 10
// 文字左边距
#define LEFT_MARGIN 10
// 文字上边距
#define TOP_MARGIN 10

enum class DIRECT {
    DIRECT_LEFT = 1,
    DIRECT_TOP,
    DIRECT_RIGHT,
    DIRECT_BOTTOM
};

class BubbleTipsWidget : public BaseWidget
{
    Q_OBJECT
public:
    explicit BubbleTipsWidget(QWidget *parent = nullptr);
    ~BubbleTipsWidget();
    void setBackColor(int r, int g, int b, int a = 255);
    // 设置三角方向(左上右下)，位置系数(宽度 * 系数)
    void setDirect(DIRECT direct = DIRECT::DIRECT_TOP, double size = 0.75);
    void setContentFont(QFont font = {});
    void setContent(const QString &content);
    void setLeftTopMargin(int leftMargin = LEFT_MARGIN, int topMargin = TOP_MARGIN);
    void setPos(int x,int y);
signals:
    void signal_interruptUpgrade(bool isInterrupt);
protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);

private:
    Ui::BubbleTipsWidget *ui;
    QColor          m_backColor  {255, 255, 255, 255};
    DIRECT          m_direct;

    double          m_posSize    {0.75};
    int             m_leftMargin {LEFT_MARGIN};
    int             m_topMargin  {TOP_MARGIN};
    QGraphicsDropShadowEffect* shadowEffect;
};
#endif // _BUBBLETIPSWIDGET_H_

