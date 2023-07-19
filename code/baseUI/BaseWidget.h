#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QtMath>
namespace Ui {
class BaseWidget;
}

class BaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BaseWidget(QWidget *parent = nullptr);
    ~BaseWidget();

private:
    Ui::BaseWidget *ui;
protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);

};

#endif // BASEWIDGET_H
