#include "FormMsgBox.h"
#include "ui_FormMsgBox.h"

FormMsgBox::FormMsgBox(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FormMsgBox)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    connect(ui->btnOk, &QPushButton::clicked, this,[this]{
        m_btn = QMessageBox::Ok;
        m_loopExit.quit();
    });
    connect(ui->btnCancel, &QPushButton::clicked, this,[this]{
        m_btn = QMessageBox::Cancel;
        m_loopExit.quit();
    });
    ui->btnOk->hide();
    ui->btnCancel->hide();
    ui->labelInfo->setWordWrap(true);
}

FormMsgBox::~FormMsgBox()
{
    delete ui;
}

QMessageBox::StandardButton FormMsgBox::exec()
{
    int iWidth = ui->labelIcon->width();
    int iHeight = ui->labelIcon->height();

    QFontMetrics info(ui->labelInfo->font());
    QSize sz = info.size(Qt::TextSingleLine,ui->labelInfo->text());
    if (sz.width()>800){
        int h = ((sz.width()+799)/800)*sz.height();
        ui->labelInfo->setFixedSize(800, h);
    }else if(sz.width()<300){
        ui->labelInfo->setFixedWidth(300);
    }else{
        ui->labelInfo->setFixedSize(sz.width()+10,sz.height()+5);
    }
    iWidth += ui->labelInfo->width();
    iHeight = iHeight>ui->labelInfo->height()?iHeight:ui->labelInfo->height();

    int iWidthBtn = 0;
    int iHeightBtn = 0;
    if (ui->btnOk->isVisible() && ui->btnCancel->isVisible()){
        QFontMetrics ok(ui->btnOk->font());
        QFontMetrics cancel(ui->btnCancel->font());
        int w1 = ok.size(Qt::TextSingleLine,ui->btnOk->text()).width();
        int w2 = cancel.size(Qt::TextSingleLine,ui->btnCancel->text()).width();
        iWidthBtn = w1>w2?w1:w2;
        ui->btnOk->setFixedWidth(iWidthBtn);
        ui->btnCancel->setFixedWidth(iWidthBtn);
        iWidthBtn *= 2;
        iHeightBtn = ui->btnOk->height();
    }
    else if (ui->btnOk->isVisible()){
        QFontMetrics ok(ui->btnOk->font());
        iWidthBtn = ok.size(Qt::TextSingleLine,ui->btnOk->text()).width();
        ui->btnOk->setFixedWidth(iWidthBtn);
        iHeightBtn = ui->btnOk->height();
    }
    else if (ui->btnCancel->isVisible()){
        QFontMetrics ok(ui->btnCancel->font());
        iWidthBtn = ok.size(Qt::TextSingleLine,ui->btnCancel->text()).width();
        ui->btnCancel->setFixedWidth(iWidthBtn);
        iHeightBtn = ui->btnCancel->height();
    }
    iWidth = (iWidth>iWidthBtn?iWidth:iWidthBtn)+40;
    iHeight += iHeightBtn+100;

    ui->widget->setFixedSize(iWidth, iHeight);
    raise();
    QWidget::show();
    m_loopExit.exec();
    hide();
    return m_btn;
}

void FormMsgBox::setText(QString strText, bool bTrue)
{
    ui->labelInfo->setText(strText);
    ui->labelIcon->setEnabled(bTrue);
}

void FormMsgBox::setOk(bool bShow, QString strTxt)
{
    if (bShow){
        ui->btnOk->setText(strTxt);
    }
    ui->btnOk->setVisible(bShow);
}

void FormMsgBox::setCancel(bool bShow, QString strTxt)
{
    if (bShow){
        ui->btnCancel->setText(strTxt);
    }
    ui->btnCancel->setVisible(bShow);
}
