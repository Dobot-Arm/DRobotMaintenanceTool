#include "FormRestartApp.h"
#include "ui_FormRestartApp.h"
#include "Define.h"
#include "MainWidget2.h"

FormRestartApp::FormRestartApp(QWidget *parent,bool bRestartApp) :
    UIBaseWidget(parent),m_bRestartApp(bRestartApp),
    ui(new Ui::FormRestartApp)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    if (m_bRestartApp)
    {
        ui->btnOk->setText(tr("重启"));
        ui->labelInfo->setText(tr("重启软件完成模式切换"));
        ui->labelInfo->setStyleSheet("font-size: 20px;"
"font-weight: 500;"
"color: #3D3D3D;"
"background-position: left center;"
"background-repeat:no-repeat;"
"padding-left:40px;"
"background-image: url(:/image/images/tipsok.png);");
    }
    else
    {
        ui->btnOk->setText(tr("我知道了"));
        ui->labelInfo->setText(tr("密码错误，请重新输入"));
        ui->labelInfo->setStyleSheet("font-size: 20px;"
"font-weight: 500;"
"color: #CE4949;"
"background-position: left center;"
"background-repeat:no-repeat;"
"padding-left:40px;"
"background-image: url(:/image/images/tipserror.png);");
    }

    connect(ui->btnOk, &QPushButton::clicked, this, &FormRestartApp::slotClickOk);
}

FormRestartApp::~FormRestartApp()
{
    delete ui;
}

void FormRestartApp::slotClickOk()
{
    if (m_bRestartApp)
    {
        g_bReStartApp = true;
        PublicgGetMainWindow()->slot_closeWindow();
    }
    close();
}
