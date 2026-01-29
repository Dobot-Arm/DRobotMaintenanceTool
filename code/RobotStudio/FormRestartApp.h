#ifndef FORMRESTARTAPP_H
#define FORMRESTARTAPP_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class FormRestartApp;
}

class FormRestartApp : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FormRestartApp(QWidget *parent = nullptr,bool bRestartApp=false);
    ~FormRestartApp();

private slots:
    void slotClickOk();

private:
    Ui::FormRestartApp *ui;
    bool m_bRestartApp = false;
};

#endif // FORMRESTARTAPP_H
