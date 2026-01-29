#ifndef IPADRESSWIDGET_H
#define IPADRESSWIDGET_H

#include <QWidget>
#include <QTime>

#include "baseUI/UIBaseWidget.h"
namespace Ui {
class IPAdressWidget;
}

class IPAdressWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit IPAdressWidget(QWidget *parent = nullptr);
    ~IPAdressWidget();
    void setSerial(QString serial);
    QStringList getIPAddress();
    void setIPAddress(QStringList ip);

signals:
    void signal_deleteThisWidget(QWidget* w);

private slots:
    void slot_delIP();
private:
    Ui::IPAdressWidget *ui;
};

#endif // IPADRESSWIDGET_H
