#include "DlgMsgBox.h"
#include "ui_DlgMsgBox.h"
#include <QStyle>

DlgMsgBox::DlgMsgBox(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::DlgMsgBox)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    setGeometry(0,0,parent->width(),parent->height());

    ui->labelTitle->hide();
    ui->labelInfo->hide();
    ui->btnNo->hide();
    ui->btnYes->hide();
    ui->labelInfo->setWordWrap(true);
    m_pSpacer = nullptr;
    m_bShowText = false;
    m_bShowOk = false;
    m_bShowCancel = false;

    connect(ui->btnNo, &QPushButton::clicked, this, [this]{
        m_btn = QMessageBox::Cancel;
        emit clickedCancel(QPrivateSignal());
        close();
    });
    connect(ui->btnYes, &QPushButton::clicked, this, [this]{
        m_btn = QMessageBox::Ok;
        emit clickedOk(QPrivateSignal());
        close();
    });
}

DlgMsgBox::~DlgMsgBox()
{
    delete ui;
}

void DlgMsgBox::setTitle(QString str, DlgMsgBox::Type type)
{
    ui->labelTitle->setText(str);
    ui->labelTitleNo->setText(str);
    ui->labelTitle->setVisible(true);
    ui->labelTitleNo->setVisible(false);
    switch(type){
    case DlgMsgBox::Type::ok:
        style()->unpolish(ui->labelTitle);
        ui->labelTitle->setProperty("state","ok");
        style()->polish(ui->labelTitle);
        break;
    case DlgMsgBox::Type::error:
        style()->unpolish(ui->labelTitle);
        ui->labelTitle->setProperty("state","error");
        style()->polish(ui->labelTitle);
        break;
    case DlgMsgBox::Type::warn:
        style()->unpolish(ui->labelTitle);
        ui->labelTitle->setProperty("state","warn");
        style()->polish(ui->labelTitle);
        break;
    case DlgMsgBox::Type::none:
        ui->labelTitle->setProperty("state","none");
        ui->labelTitle->setVisible(false);
        ui->labelTitleNo->setVisible(true);
        ui->labelTitleNo->setFixedHeight(96);
        break;
    }
}

void DlgMsgBox::setText(QString str, bool bShow)
{
    ui->labelInfo->setText(str);
    ui->labelInfo->setVisible(bShow);
    m_bShowText = bShow;
}

void DlgMsgBox::setOk(QString str, bool bShow)
{
    ui->btnYes->setText(str);
    ui->btnYes->setVisible(bShow);
    m_bShowOk = bShow;
}

void DlgMsgBox::setCancel(QString str, bool bShow)
{
    ui->btnNo->setText(str);
    ui->btnNo->setVisible(bShow);
    m_bShowCancel = bShow;
}

void DlgMsgBox::show()
{
    //ui宽度为固定，高度自适应
    int iMargin = 56;
    int iWidth = ui->widget->width();
    int iHeight = ui->labelTitle->height();

    if (m_bShowText){
        int itxtwidth = iWidth-iMargin*2;
        QFontMetrics info(ui->labelInfo->font());
        QSize sz = info.size(Qt::TextSingleLine,ui->labelInfo->text());
        if (sz.width()>itxtwidth){
            int h = ((sz.width()+(itxtwidth-1))/itxtwidth)*sz.height();
            ui->labelInfo->setFixedSize(itxtwidth, h);
        }else{
            ui->labelInfo->setFixedSize(itxtwidth,sz.height());
        }
        iHeight += ui->labelInfo->height();
    }

    if (m_bShowOk || m_bShowCancel){
        if (m_bShowOk && m_bShowCancel){
            ui->btnYes->setFixedSize(150, 44);
            ui->btnNo->setFixedSize(150, 44);

            if (!m_pSpacer){
                m_pSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
                ui->horizontalLayout->insertItem(1, m_pSpacer);
            }
        }
        else{
            ui->btnYes->setFixedSize(200, 44);
            ui->btnNo->setFixedSize(200, 44);
            if (m_pSpacer){
                ui->horizontalLayout->removeItem(m_pSpacer);
                delete m_pSpacer;
                m_pSpacer = nullptr;
            }
        }
        iHeight += iMargin;
        iHeight += ui->btnYes->height();
        iHeight += iMargin;
    }
    ui->widget->setFixedHeight(iHeight);

    raise();
    QWidget::show();
}
