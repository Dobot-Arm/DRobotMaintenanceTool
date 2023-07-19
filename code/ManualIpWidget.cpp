#include "ManualIpWidget.h"
#include "ui_ManualIpWidget.h"
#include <QWidgetItem>

ManualIpWidget::ManualIpWidget(QWidget *parent) :
    BaseWidget(parent),
    mouse_press(false),
    ui(new Ui::ManualIpWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );//无边框，置顶
    setWindowModality(Qt::ApplicationModal); //禁用主窗口
    m_messageWidget = new MessageWidget();
//    m_messageWidget->hide();

    connect(ui->btnAdd,&QPushButton::clicked,this,&ManualIpWidget::slot_addIP);
    connect(ui->btnCancel,&QPushButton::clicked,this,&ManualIpWidget::slot_Cancel);
    connect(ui->btnSave,&QPushButton::clicked,this,&ManualIpWidget::slot_saveIPs);
    connect(ui->btnClose,&QPushButton::clicked,this,&ManualIpWidget::slot_closeWidget);



    IPAdressWidget* ipAddressW1 = new IPAdressWidget(this);
    ui->verticalLayout->addWidget(ipAddressW1);
    connect(ipAddressW1,&IPAdressWidget::signal_deleteThisWidget,this,&ManualIpWidget::slot_delIPAddress);

}

ManualIpWidget::~ManualIpWidget()
{
    delete ui;
}

void ManualIpWidget::initIP()
{
    if(ui->verticalLayout->count() == 0){
        IPAdressWidget* ipAddressW = new IPAdressWidget(this);
        connect(ipAddressW,&IPAdressWidget::signal_deleteThisWidget,this,&ManualIpWidget::slot_delIPAddress);
        ui->verticalLayout->addWidget(ipAddressW);
    }
    if(ui->verticalLayout->count()<5){
        ui->btnAdd->setDisabled(false);
    }
}

bool ManualIpWidget::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

bool ManualIpWidget::checkIP()
{
    for(int i =0;i< ui->verticalLayout->count();i++){
         IPAdressWidget * ipAddressWidget =qobject_cast<IPAdressWidget *>(ui->verticalLayout->itemAt(i)->widget());
         QStringList ipList = ipAddressWidget->getIPAddress();
         for(QString ip : ipList){
             bool ok;
             ip.toInt(&ok);
             if(!ok||ip>255||ip<0){
                 return false;
             }
         }
    }
    return true;
}

void ManualIpWidget::slot_addIP()
{

    IPAdressWidget* ipAddressW = new IPAdressWidget(this);
    ui->verticalLayout->addWidget(ipAddressW);
    connect(ipAddressW,&IPAdressWidget::signal_deleteThisWidget,this,&ManualIpWidget::slot_delIPAddress);
    slot_checkIPAddressOrder();
    if(ui->verticalLayout->count() == 5){
        ui->btnAdd->setDisabled(true);
    }

}

void ManualIpWidget::slot_Cancel()
{

    for(int i =0;i< ui->verticalLayout->count();i++){
         IPAdressWidget * ipAddressWidget =qobject_cast<IPAdressWidget *>(ui->verticalLayout->itemAt(i)->widget());
         if(!ipAddressWidget->isSave){
            ipAddressWidget->deleteLater();
         }
    }
    this->hide();
}

void ManualIpWidget::slot_saveIPs()
{
    if(checkIP()){
        QStringList saveIpList;
        for(int i =0;i< ui->verticalLayout->count();i++){
             IPAdressWidget * ipAddressWidget =qobject_cast<IPAdressWidget *>(ui->verticalLayout->itemAt(i)->widget());
             ipAddressWidget->isSave = true;
             QStringList ipList = ipAddressWidget->getIPAddress();
             QString saveIp;
             saveIp.append(ipList.at(0)+"."+ipList.at(1)+"."+ipList.at(2)+"."+ipList.at(3));
             qDebug()<< "saveIp  "<<saveIp;
             saveIpList.append(saveIp);
        }
        for(QString ip:saveIpList){
            qDebug()<<"saveIpList ip"<<ip;
        }
        emit signal_saveIpsToMainWindow(saveIpList);
        close();
    }else{
        m_messageWidget->setMessage("warn",tr("IP地址不合法或IP地址不能为空，\n请输入正确的IP!"));
        m_messageWidget->show();
    }
}

//void ManualIpWidget::mousePressEvent(QMouseEvent *e)
//{
//    if(e->button()==Qt::LeftButton
//      && e->x() < this->width()
//      && e->y() < this->height())
//    {
//        this->setCursor(Qt::ClosedHandCursor);
//        mouse_press = true;
//    }
//    move_point=e->globalPos()-this->pos();
//}

//void ManualIpWidget::mouseMoveEvent(QMouseEvent *e)
//{
//    if(mouse_press)
//    {
//        QPoint move_pos=e->globalPos();
//        this->move(move_pos-move_point);
//    }
//}

//void ManualIpWidget::mouseReleaseEvent(QMouseEvent *e)
//{
//    mouse_press = false;
//    this->setCursor(Qt::ArrowCursor);
//}

void ManualIpWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManualIpWidget::slot_closeWidget()
{
    for(int i =0;i< ui->verticalLayout->count();i++){
         IPAdressWidget * ipAddressWidget =qobject_cast<IPAdressWidget *>(ui->verticalLayout->itemAt(i)->widget());
         if(!ipAddressWidget->isSave){
            ipAddressWidget->deleteLater();
         }
    }
    close();
}

void ManualIpWidget::slot_checkIPAddressOrder()
{

    for(int i =0;i< ui->verticalLayout->count();i++){
         IPAdressWidget * ipAddressWidget =qobject_cast<IPAdressWidget *>(ui->verticalLayout->itemAt(i)->widget());
         ipAddressWidget->setSerial(QString(tr("IP地址")+"%1").arg(i+1));
    }
}

void ManualIpWidget::slot_delIPAddress(QWidget* w)
{
    ui->verticalLayout->removeWidget(w);
    ui->btnAdd->setDisabled(false);
    w->deleteLater();
    slot_checkIPAddressOrder();
}

void ManualIpWidget::sleep(int milliseconds)
{
    QTime dieTime = QTime::currentTime().addMSecs(milliseconds);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}



