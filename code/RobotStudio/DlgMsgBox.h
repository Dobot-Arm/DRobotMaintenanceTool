#ifndef DLGMSGBOX_H
#define DLGMSGBOX_H

#include <QWidget>
#include <QMessageBox>
#include "baseUI/UIBaseWidget.h"
#include <QEventLoop>
#include <QSpacerItem>

namespace Ui {
class DlgMsgBox;
}

class DlgMsgBox : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit DlgMsgBox(QWidget *parent = nullptr);
    ~DlgMsgBox();

    enum Type{
        none,
        ok,
        error,
        warn
    };

    void setTitle(QString str, DlgMsgBox::Type type);
    void setText(QString str, bool bShow=true);
    void setOk(QString str, bool bShow=true);
    void setCancel(QString str, bool bShow=true);

    void show(); //非阻塞形式显示

    QMessageBox::StandardButton getClickBtn() const{return m_btn;}

signals:
    void clickedOk(QPrivateSignal);
    void clickedCancel(QPrivateSignal);

private:
    Ui::DlgMsgBox *ui;
    QMessageBox::StandardButton m_btn;
    QSpacerItem* m_pSpacer;
    bool m_bShowText,m_bShowOk,m_bShowCancel;
};

#endif // DLGMSGBOX_H
