#ifndef FORMMSGBOX_H
#define FORMMSGBOX_H

#include <QWidget>
#include <QMessageBox>
#include "baseUI/UIBaseWidget.h"
#include <QEventLoop>

namespace Ui {
class FormMsgBox;
}

class FormMsgBox : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FormMsgBox(QWidget *parent = nullptr);
    ~FormMsgBox();

    void setText(QString strText, bool bTrue);
    void setOk(bool bShow, QString strTxt="");
    void setCancel(bool bShow, QString strTxt="");
    QMessageBox::StandardButton exec();

private:
    Ui::FormMsgBox *ui;
    QMessageBox::StandardButton m_btn;
    QEventLoop m_loopExit;
};

#endif // FORMMSGBOX_H
