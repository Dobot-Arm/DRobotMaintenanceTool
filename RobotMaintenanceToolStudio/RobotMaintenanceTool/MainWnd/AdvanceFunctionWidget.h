#ifndef ADVANCEFUNCTIONWIDGET_H
#define ADVANCEFUNCTIONWIDGET_H

#include <QWidget>

namespace Ui {
class AdvanceFunctionWidget;
}

class AdvanceFunctionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdvanceFunctionWidget(QWidget *parent = nullptr);
    ~AdvanceFunctionWidget();

private:
    Ui::AdvanceFunctionWidget *ui;
};

#endif // ADVANCEFUNCTIONWIDGET_H
