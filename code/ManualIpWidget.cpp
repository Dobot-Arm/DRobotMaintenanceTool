#include "ManualIpWidget.h"
#include "ui_ManualIpWidget.h"
#include "Define.h"
#include "MainWidget2.h"
#include <QWidgetItem>
#include <QSet>

QList<QStringList> ManualIpWidget::m_allIp;

ManualIpWidget::ManualIpWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::ManualIpWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    connect(ui->btnAdd,&QPushButton::clicked,this,&ManualIpWidget::slot_addIP);
    connect(ui->btnSave,&QPushButton::clicked,this,&ManualIpWidget::slot_saveIPs);
    connect(ui->btnClose,&QPushButton::clicked,this,&ManualIpWidget::close);

    initIP();
}

ManualIpWidget::~ManualIpWidget()
{
    delete ui;
}

void ManualIpWidget::show()
{
    raise();
    QWidget::show();
}

void ManualIpWidget::initIP()
{
    for (int i=0; i<m_allIp.size(); ++i){
        auto pWnd = insertAddAddress();
        pWnd->setIPAddress(m_allIp[i]);
    }
    ui->btnAdd->setEnabled(getAddressCount()<5);
}

void ManualIpWidget::slot_addIP()
{
    insertAddAddress();
    ui->btnAdd->setEnabled(getAddressCount()<5);
}

void ManualIpWidget::slot_delIPAddress(QWidget* w)
{
    ui->verticalLayout->removeWidget(w);
    w->deleteLater();
    updateIPAddressIndex();
    ui->btnAdd->setEnabled(getAddressCount()<5);
}

IPAdressWidget* ManualIpWidget::insertAddAddress()
{
    auto pWnd = new IPAdressWidget(this);
    connect(pWnd,&IPAdressWidget::signal_deleteThisWidget,this,&ManualIpWidget::slot_delIPAddress);
    ui->verticalLayout->insertWidget(m_ipWndCount,pWnd);
    ++m_ipWndCount;
    pWnd->setSerial(QString(tr("IP地址")+"%1").arg(m_ipWndCount));
    return pWnd;
}

int ManualIpWidget::getAddressCount()
{
    return m_ipWndCount;
}

void ManualIpWidget::updateIPAddressIndex()
{
    m_ipWndCount = 0;
    for(int i =0;i< ui->verticalLayout->count();i++){
         auto pWnd = qobject_cast<IPAdressWidget*>(ui->verticalLayout->itemAt(i)->widget());
         if (pWnd){
             ++m_ipWndCount;
            pWnd->setSerial(QString(tr("IP地址")+"%1").arg(m_ipWndCount));
         }
    }
}

bool ManualIpWidget::checkIpValid(QStringList ipList)
{
    if (ipList.size()!=4) return false;
    bool bOk = false;
    int v = 0;
    for (int i=0; i<4; ++i){
        v = ipList[i].toInt(&bOk);
        if (!bOk || v<0 || v>255){
            return false;
        }
    }
    return true;
}
void ManualIpWidget::slot_saveIPs()
{
    QList<QStringList> allIp;
    QSet<QString> saveIpList;
    for(int i =0;i< ui->verticalLayout->count();i++){
        auto pWnd = qobject_cast<IPAdressWidget *>(ui->verticalLayout->itemAt(i)->widget());
        if (!pWnd) continue;
        QStringList ipList = pWnd->getIPAddress();
        if (!checkIpValid(ipList)){
            auto* pWnd = new MessageWidget(PublicgGetMainWindow());
            pWnd->setMessage("warn",tr("IP地址不合法或IP地址不能为空，\n请输入正确的IP!"));
            pWnd->show();
            return;
        }
        QString saveIp = ipList.join('.');
        if (!saveIpList.contains(saveIp)){
            saveIpList.insert(saveIp);
            allIp.append(ipList);
        }
    }
    m_allIp = allIp;
    QStringList iplst = saveIpList.toList();
    emit signal_saveIpsToMainWindow(iplst);
    close();
}
