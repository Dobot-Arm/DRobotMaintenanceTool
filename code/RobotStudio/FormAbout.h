#ifndef FORMABOUT_H
#define FORMABOUT_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class FormAbout;
}

class FormAbout : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FormAbout(QWidget *parent = nullptr);
    ~FormAbout();
    void show();

signals:
    void emitLogExport();

private:
    void initUI();
    bool event(QEvent *event) override;

private slots:
    void slotClickGoBack();
    void slotClickLogExport();
    void slotClickLicense();
    void slotClickOk();

private:
    Ui::FormAbout *ui;
};

#endif // FORMABOUT_H
