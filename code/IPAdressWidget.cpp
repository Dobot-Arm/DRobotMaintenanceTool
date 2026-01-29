#include "IPAdressWidget.h"
#include "ui_IPAdressWidget.h"
#include <QRegularExpressionValidator>

IPAdressWidget::IPAdressWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::IPAdressWidget)
{
    ui->setupUi(this);
    setStyleSheet("background: #FAFDFF;");
    connect(ui->btnDelIP,&QPushButton::clicked,this,&IPAdressWidget::slot_delIP);
    ui->lineIP_1->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+")));
    ui->lineIP_2->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+")));
    ui->lineIP_3->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+")));
    ui->lineIP_4->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+")));

    connect(ui->lineIP_1, &QLineEdit::textChanged, this, [this](QString str){
        if (str.length()>=3) ui->lineIP_2->setFocus();
    });
    connect(ui->lineIP_2, &QLineEdit::textChanged, this, [this](QString str){
        if (str.length()>=3) ui->lineIP_3->setFocus();
    });
    connect(ui->lineIP_3, &QLineEdit::textChanged, this, [this](QString str){
        if (str.length()>=3) ui->lineIP_4->setFocus();
    });
    ui->lineIP_1->setFocus();
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


