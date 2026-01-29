#include "AboutMeWidget.h"
#include "ui_AboutMeWidget.h"
#include "Define.h"
#include "MainWidget2.h"
#include <QStorageInfo>
#include <QSettings>
#include <QStyleOption>
#include <QPainter>
#include "CommonData.h"
#include "MessageWidget.h"
#include "EnvDebugConfig.h"

AboutMeWidget::AboutMeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutMeWidget)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明

    connect(ui->btnConfirm,&QPushButton::clicked,this,[&]{close();});
    connect(ui->rdbRelease, &QRadioButton::clicked, this, &AboutMeWidget::slotRadioReleaseClicked);
    connect(ui->rdbDebug, &QRadioButton::clicked, this, &AboutMeWidget::slotRadioDebugClicked);
    connect(ui->btnCheckKey, &QRadioButton::clicked, this, &AboutMeWidget::slotKeyBtnClicked);
}

AboutMeWidget::~AboutMeWidget()
{
    delete ui;
}

void AboutMeWidget::refreshInfo()
{
    QSettings setting("config/config.ini",QSettings::IniFormat);
    QString softVersion = setting.value("Version/softVersion").toString();
    QString manualVersion = setting.value("Version/manualVersion").toString();
    QString strRegCode = setting.value("CFG/regcode").toString();
    QString strRandomCode = setting.value("CFG/randomcode").toString();

    ui->lineEditComputerCode->setText(strRandomCode);
    ui->lineEditKey->setText(strRegCode);
    if (EnvDebugConfig::isDebugEnv())
    {
        ui->rdbRelease->setChecked(false);
        ui->rdbDebug->setChecked(true);
        slotRadioDebugClicked();
    }
    else
    {
        ui->rdbRelease->setChecked(true);
        ui->rdbRelease->setChecked(false);
        slotRadioReleaseClicked();
    }

#ifdef V136_NOVA_UPDATE
    softVersion = V136_NOVA_UPDATE;
#endif

    ui->labelVersion->setOpenExternalLinks(true);//设置为true才能打开网页
    ui->labelVersion->setText(tr("<p>机器人维护工具版本：%1</p><p>工具服务版本：%2</p><p>使用手册版本：%3</p><a style='color: #0047BB; text-decoration: none' href = %4>隐私协议")
                         .arg(softVersion)
                         .arg(CommonData::getStructProtocolVersion().toolVersion)
                         .arg(manualVersion)
                         .arg("https://www.dobot.cn/privacy-policy"));
    ui->labelVersion->setAlignment(Qt::AlignCenter);//文字居中
}

void AboutMeWidget::slotRadioDebugClicked()
{
    ui->btnCheckKey->setVisible(true);
    ui->lineEditKey->setVisible(true);
    ui->labelComputerCode->setVisible(true);
    ui->lineEditComputerCode->setVisible(true);

    ui->lineEditKey->setText("");
    ui->lineEditComputerCode->setText(EnvDebugConfig::GetComputerCode());

    QSettings setting("config/config.ini",QSettings::IniFormat);
    setting.setValue("CFG/env","debug");
    setting.sync();

}

void AboutMeWidget::slotRadioReleaseClicked()
{
    ui->btnCheckKey->setVisible(false);
    ui->lineEditKey->setVisible(false);
    ui->labelComputerCode->setVisible(false);
    ui->lineEditComputerCode->setVisible(false);

    EnvDebugConfig::change2Release();

    ui->lineEditKey->setText("");
    ui->lineEditComputerCode->setText("");
}

void AboutMeWidget::slotKeyBtnClicked()
{
    QString strComputeCode = ui->lineEditComputerCode->text();
    QString strRegCode = ui->lineEditKey->text();
    if (EnvDebugConfig::change2Debug(strComputeCode, strRegCode))
    {
        auto* pWnd = new MessageWidget(PublicgGetMainWindow());
        pWnd->setMessage("warn",tr("已切换为测试环境，重启后生效！"));
        pWnd->show();
    }
    else
    {
        EnvDebugConfig::change2Release();

        auto* pWnd = new MessageWidget(PublicgGetMainWindow());
        pWnd->setMessage("warn",tr("校验失败，无法切换为测试环境！"));
        pWnd->show();
    }
}

void AboutMeWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool AboutMeWidget::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}
