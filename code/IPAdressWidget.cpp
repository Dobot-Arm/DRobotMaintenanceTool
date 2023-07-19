#include "IPAdressWidget.h"
#include "ui_IPAdressWidget.h"

IPAdressWidget::IPAdressWidget(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::IPAdressWidget)
{
    ui->setupUi(this);
    isSave = false;
    connect(ui->btnDelIP,&QPushButton::clicked,this,&IPAdressWidget::slot_delIP);
}

IPAdressWidget::~IPAdressWidget()
{
    delete ui;
}

void IPAdressWidget::setSerial(QString serial)
{
    ui->labelIP->setText(serial);
}

QStringList IPAdressWidget::getIPAddress()
{
    QStringList ipList;
    ipList.append(ui->lineIP_1->text().simplified());
    ipList.append(ui->lineIP_2->text().simplified());
    ipList.append(ui->lineIP_3->text().simplified());
    ipList.append(ui->lineIP_4->text().simplified());

    return ipList;
}

void IPAdressWidget::setIPAddress(QStringList ip)
{
    ui->lineIP_1->setText(ip.takeFirst());
    ui->lineIP_2->setText(ip.takeFirst());
    ui->lineIP_3->setText(ip.takeFirst());
    ui->lineIP_4->setText(ip.takeFirst());
    return;
}

void IPAdressWidget::slot_delIP()
{
    emit signal_deleteThisWidget(this);

}


