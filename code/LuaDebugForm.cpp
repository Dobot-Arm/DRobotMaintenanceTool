#include "LuaDebugForm.h"
#include "ui_LuaDebugForm.h"
#include "LuaApi.h"

LuaDebugForm::LuaDebugForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LuaDebugForm)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->btnClear, &QPushButton::clicked, this, &LuaDebugForm::slotClear);
    connect(CLuaApi::getInstance(), &CLuaApi::signalLuaLog, this, &LuaDebugForm::slotLog);

    //ui->textBrowser->setStyleSheet("line-height:1.5;");
}

LuaDebugForm::~LuaDebugForm()
{
    delete ui;
}

void LuaDebugForm::slotClear()
{
    ui->textBrowser->clear();
}

void LuaDebugForm::slotLog(QString strLog, QtMsgType type)
{
    auto fmt = ui->textBrowser->currentCharFormat();
    if (type==QtMsgType::QtCriticalMsg)
        fmt.setForeground(QBrush(Qt::red));
    else
        fmt.setForeground(QBrush(Qt::black));
    ui->textBrowser->setCurrentCharFormat(fmt);
    ui->textBrowser->moveCursor(QTextCursor::End);
    ui->textBrowser->insertPlainText(strLog);
}
