#ifndef SNMATCHFAILEDWIDGET_H
#define SNMATCHFAILEDWIDGET_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"
#include <QEventLoop>
namespace Ui {
class SNMatchFailedWidget;
}

class SNMatchFailedWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit SNMatchFailedWidget(QWidget *parent = nullptr);
    ~SNMatchFailedWidget();

    void exec();
private:
    Ui::SNMatchFailedWidget *ui;
    QEventLoop m_loopExit;
};

#endif // SNMATCHFAILEDWIDGET_H
