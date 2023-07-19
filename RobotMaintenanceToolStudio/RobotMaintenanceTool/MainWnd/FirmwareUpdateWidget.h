#ifndef FIRMWAREUPDATEWIDGET_H
#define FIRMWAREUPDATEWIDGET_H

#include <QWidget>

namespace Ui {
class FirmwareUpdateWidget;
}

class FirmwareUpdateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FirmwareUpdateWidget(QWidget *parent = nullptr);
    ~FirmwareUpdateWidget();

private:
    Ui::FirmwareUpdateWidget *ui;
};

#endif // FIRMWAREUPDATEWIDGET_H
