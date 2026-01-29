#ifndef SNWRITECONFIRMWIDGET_H
#define SNWRITECONFIRMWIDGET_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"
#include <QEventLoop>
namespace Ui {
class SNWriteConfirmWidget;
}

class SNWriteConfirmWidget : public UIBaseWidget
{
    Q_OBJECT
signals:
    void signalSNIsWrite(bool isWrite);
public:
    explicit SNWriteConfirmWidget(QWidget *parent = nullptr);
    ~SNWriteConfirmWidget();
    void setMessage(QString msg);
    bool isConfirmWrite(){return m_isConfirmWrite;}

    void exec();
private:
    Ui::SNWriteConfirmWidget *ui;
    bool m_isConfirmWrite = false;
    QEventLoop m_loopExit;
};

#endif // SNWRITECONFIRMWIDGET_H
