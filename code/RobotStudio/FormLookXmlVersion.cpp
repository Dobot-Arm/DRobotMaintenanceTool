#include "FormLookXmlVersion.h"
#include "ui_FormLookXmlVersion.h"
#include "../PlinkCmd.h"
#include "Define.h"
#include <QTimer>

QString FormLookXmlVersion::m_strXmlVersion;
QString FormLookXmlVersion::m_strXmlVersionOld;
FormLookXmlVersion::FormLookXmlVersion(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FormLookXmlVersion)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnClose,&QPushButton::clicked,this,&FormLookXmlVersion::close);

    readXml();
}

FormLookXmlVersion::~FormLookXmlVersion()
{
    delete ui;
}

void FormLookXmlVersion::resizeEvent(QResizeEvent *event)
{
    QSize sz(event->size().width()*2/3,event->size().height()*2/3);
    ui->widget->setFixedSize(sz);
}

void FormLookXmlVersion::readXml()
{
    ui->textBrowser->setPlainText(m_strXmlVersionOld);
    m_strXmlVersion = "";
    auto pLink = new PlinkCmd(this);
    pLink->setIp(g_strCurrentIP);
    pLink->setPlink(QCoreApplication::applicationDirPath()+"/tool/plink/plink.exe");
    QObject::connect(pLink, &PlinkCmd::signalRead, this, [](QString str){
        FormLookXmlVersion::m_strXmlVersion.append(str);
    });
    QObject::connect(pLink, &PlinkCmd::signalFinishedCmd,this,[this]{
        m_strXmlVersion.remove("modify by lf");
        ui->textBrowser->clear();
        ui->textBrowser->setPlainText(m_strXmlVersion);
        m_strXmlVersionOld = m_strXmlVersion;
    });
    pLink->execCmd("cat /dobot/userdata/SlaveLog");
}
