#ifndef UIBASEWIDGET_H
#define UIBASEWIDGET_H

#include <QWidget>
#include <QPaintEvent>

class UIBaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UIBaseWidget(QWidget *parent = nullptr);
    ~UIBaseWidget();

signals:

protected:
    void updateStyleSheet(QWidget* p);
    void paintEvent(QPaintEvent*) override;
};

#endif // UIBASEWIDGET_H
