#ifndef TITLEBARWIDGET_H
#define TITLEBARWIDGET_H

#include <QWidget>

namespace Ui {
class TitleBarWidget;
}

class TitleBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBarWidget(QWidget *parent = nullptr);
    ~TitleBarWidget();

private:
    Ui::TitleBarWidget *ui;
};

#endif // TITLEBARWIDGET_H
