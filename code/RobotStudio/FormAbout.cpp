#include "FormAbout.h"
#include "ui_FormAbout.h"
#include "Define.h"
#include "Logger.h"
#include "CommonData.h"
#include <QSettings>
#include <QDesktopServices>
#include "UpdateConfigBean.h"
#include "EnvDebugConfig.h"
#include "FormRestartApp.h"
#include "MainWidget2.h"

FormAbout::FormAbout(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FormAbout)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    initUI();

    connect(ui->btnGoBack, &QPushButton::clicked, this, &FormAbout::slotClickGoBack);
    connect(ui->btnLogExport, &QPushButton::clicked, this, &FormAbout::slotClickLogExport);
    connect(ui->btnLicense, &QPushButton::clicked, this, &FormAbout::slotClickLicense);
    connect(ui->btnOk, &QPushButton::clicked, this, &FormAbout::slotClickOk);
}

FormAbout::~FormAbout()
{
    delete ui;
}

void FormAbout::show()
{
    raise();
    initUI();
    QWidget::show();
}

void FormAbout::initUI()
{
    QSettings setting("config/config.ini",QSettings::IniFormat);
    QString softVersion = setting.value("Version/softVersion").toString();
    QString manualVersion = setting.value("Version/manualVersion").toString();
    QString strRegCode = setting.value("CFG/regcode").toString();
    QString strRandomCode = setting.value("CFG/randomcode").toString();

    ui->labelAppVersion->setText(softVersion);
    ui->labelToolVersion->setText(CommonData::getStructProtocolVersion().toolVersion);
    ui->labelManualVersion->setText(manualVersion);

    ui->labelCfgVersion->setText("");
    UpdateConfigObject localUpdateConfig;
    localUpdateConfig.fromJsonFile(getUpdateConfigJsonFile());
    if (localUpdateConfig.updatefirmware.version!=0)
    {
        ui->labelCfgVersion->setText(QString::asprintf("V%d",localUpdateConfig.updatefirmware.version));
    }

    if (EnvDebugConfig::isDebugEnv())
    {
        ui->btnOk->setText(tr("退出登录"));
        ui->labelCode->hide();
        ui->labelRandomCode->hide();
        ui->labelPwd->hide();
        ui->lineEditPwd->hide();

        ui->btnOk->setFixedSize(350,44);

        ui->labelRandomCode->setText("");
        ui->lineEditPwd->setText("");
    }
    else
    {
        ui->btnOk->setText(tr("确定"));
        ui->labelCode->show();
        ui->labelRandomCode->show();
        ui->labelPwd->show();
        ui->lineEditPwd->show();

        ui->labelRandomCode->setText(EnvDebugConfig::GetComputerCode());
        ui->lineEditPwd->setText("");
    }
}

bool FormAbout::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

void FormAbout::slotClickGoBack()
{
    hide();
}

void FormAbout::slotClickLogExport()
{
    emit emitLogExport();
}

void FormAbout::slotClickLicense()
{
    QDesktopServices::openUrl(QUrl(QString("https://www.dobot.cn/privacy-policy")));
}

void FormAbout::slotClickOk()
{
    bool bRestartApp = false;
    QString strComputeCode = ui->labelRandomCode->text();
    QString strRegCode = ui->lineEditPwd->text();
    if (strComputeCode.isEmpty())
    {//为空，肯定是退出登陆
        bRestartApp = true;
        EnvDebugConfig::change2Release();
    }
    else
    {//校验
        if (EnvDebugConfig::change2Debug(strComputeCode, strRegCode))
        {
            bRestartApp = true;
        }
        else
        {
            EnvDebugConfig::change2Release();
        }
    }
    auto ptr = new FormRestartApp(PublicgGetMainWindow(),bRestartApp);
    ptr->setGeometry(0,0,PublicgGetMainWindow()->width(),PublicgGetMainWindow()->height());
    ptr->raise();
    ptr->show();
}
