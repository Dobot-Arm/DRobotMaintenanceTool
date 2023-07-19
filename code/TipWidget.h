#ifndef TIPWIDGET_H
#define TIPWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>
namespace Ui {
class TipWidget;
}

class TipWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TipWidget(QWidget *parent = nullptr);
    ~TipWidget();
    void setLabelMessage(QString message);
private:
    Ui::TipWidget *ui;
    QPoint move_point;
    bool mouse_press;
public slots:
    void slot_closeWindow();
    void slot_cofirm();
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // TIPWIDGET_H
