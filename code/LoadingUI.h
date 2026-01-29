#ifndef LOADINGUI_H
#define LOADINGUI_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class LoadingUI;
}

class LoadingUI : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit LoadingUI(QWidget *parent = nullptr);
    ~LoadingUI();
    void setText(const QString& str);
    void setBigText(const QString& str);

private:
    Ui::LoadingUI *ui;
};

#endif // LOADINGUI_H
