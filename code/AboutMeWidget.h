#ifndef ABOUTMEWIDGET_H
#define ABOUTMEWIDGET_H

#include <QWidget>

namespace Ui {
class AboutMeWidget;
}

class AboutMeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AboutMeWidget(QWidget *parent = nullptr);
    ~AboutMeWidget();

    void refreshInfo();

private slots:
    void slotRadioDebugClicked();
    void slotRadioReleaseClicked();
    void slotKeyBtnClicked();

protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);

private:
    Ui::AboutMeWidget *ui;
};

#endif // ABOUTMEWIDGET_H
