#ifndef SNWRITESTATUSWIDGET_H
#define SNWRITESTATUSWIDGET_H

#include <QWidget>
#include <QEventLoop>
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class SNWriteStatusWidget;
}

class SNWriteStatusWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit SNWriteStatusWidget(QWidget *parent = nullptr);
    ~SNWriteStatusWidget();
    void setWriteFailedMsg(QString msg);
    void setWriteOkMsg(QString msg);
    void setWriteStatus(bool isWirteSuccess);
    bool isClose();

    void exec();
private:
    Ui::SNWriteStatusWidget *ui;
    bool m_isClose = false;
    QEventLoop m_loopExit;
    bool m_bSuccess = false;
};

#endif // SNWRITESTATUSWIDGET_H
