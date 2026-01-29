#ifndef FORMMENUBUTTON_H
#define FORMMENUBUTTON_H

#include <QWidget>
#include <QMenu>
#include <QEvent>
#include <QMouseEvent>
#include "baseUI/UIBaseWidget.h"
#include <functional>

namespace Ui {
class FormMenuButton;
}

class FormMenuButton : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FormMenuButton(QWidget *parent = nullptr);
    ~FormMenuButton();

    void setMenu(QMenu* pMenu){m_pMenu = pMenu;};

    void setOnEventBeforeShowMenu(const std::function<bool()>& cb){m_pfnEvent=cb;}

    void changeMenuButtonText(QString strText, bool bHasPackage=false);

private slots:
    void slotClickShowMenu();

private:
    bool event(QEvent *event) override;
    void mousePressEvent(QMouseEvent* pEvent) override;

private:
    Ui::FormMenuButton *ui;
    QMenu* m_pMenu = nullptr;
    QString m_strPackVersion;
    std::function<bool()> m_pfnEvent = nullptr;
};

#endif // FORMMENUBUTTON_H
