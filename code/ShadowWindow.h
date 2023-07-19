#ifndef SHADOWWINDOW_H
#define SHADOWWINDOW_H

#include <QWidget>

namespace Ui {
class ShadowWindow;
}

class ShadowWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ShadowWindow(QWidget *parent = nullptr);
    ~ShadowWindow();
    //注册要显示在遮罩中的窗口
    void installWidget(QWidget *widget);

    //设置遮罩颜色、透明度
    void setMaskColor(const QColor &color,float opacity);

    //设置顶层窗口
    void setTopWidget(QWidget *widget);

private:
    void init();
    void showEvent(QShowEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QWidget *m_topWidget;    //顶层窗口，设置遮罩颜色、透明度
    QWidget *m_widget;       //遮罩中显示的窗口
private:
    Ui::ShadowWindow *ui;
};

#endif // SHADOWWINDOW_H
