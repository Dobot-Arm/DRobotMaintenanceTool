#ifndef IPADRESSWIDGET_H
#define IPADRESSWIDGET_H

#include <QWidget>
#include <QTime>

#include "baseUI/BaseWidget.h"
namespace Ui {
class IPAdressWidget;
}

class IPAdressWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit IPAdressWidget(QWidget *parent = nullptr);
    ~IPAdressWidget();
    void setSerial(QString serial);
    QStringList getIPAddress();
    void setIPAddress(QStringList ip);
    bool isSave;
public slots:
    void slot_delIP();
signals:
    void signal_deleteThisWidget(QWidget* w);
private:
    Ui::IPAdressWidget *ui;
};

#endif // IPADRESSWIDGET_H
