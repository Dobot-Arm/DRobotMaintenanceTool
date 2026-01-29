#include "FormMenuButton.h"
#include "ui_FormMenuButton.h"
#include <QStyle>

FormMenuButton::FormMenuButton(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FormMenuButton)
{
    ui->setupUi(this);
}

FormMenuButton::~FormMenuButton()
{
    delete ui;
}

bool FormMenuButton::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange && m_strPackVersion.isEmpty()){
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::event(event);
}

void FormMenuButton::mousePressEvent(QMouseEvent *pEvent)
{
    QRect rc(mapToGlobal(QPoint(0,0)),size());
    if (m_pMenu && !m_pMenu->isEmpty() && rc.contains(pEvent->globalPos()))
    {
        if (!m_pfnEvent()) return;
        slotClickShowMenu();
    }
}

static void setAllSubMenuFixWidth(QMenu* pMenu, int iWidth)
{
    pMenu->setFixedWidth(iWidth);
    auto allAction = pMenu->actions();
    for (QAction* pAct : allAction)
    {
        QMenu* pSub = pAct->menu();
        if (pSub)
        {
            setAllSubMenuFixWidth(pSub, iWidth);
        }
    }
}

void FormMenuButton::slotClickShowMenu()
{
    if (!m_pMenu) return;

    bool bShowMenu = false;
    this->style()->unpolish(this);
    this->style()->unpolish(ui->menuRightIcon);
    QString strProperty = property("drop-down").toString();
    if ("on"==strProperty)
    {//已打开则收拢
        setProperty("drop-down", QVariant());
        ui->menuRightIcon->setProperty("drop-down", QVariant(""));
    }
    else
    {//未打开则打开
        setProperty("drop-down", QVariant("on"));
        ui->menuRightIcon->setProperty("drop-down", QVariant("on"));
        bShowMenu = true;
    }
    this->style()->polish(ui->menuRightIcon);
    this->style()->polish(this);

    if (bShowMenu)
    {
        setAllSubMenuFixWidth(m_pMenu,width());
        QPoint pt = mapToGlobal(QPoint(0,height()));
        pt.setY(pt.y()+2);
        m_pMenu->exec(pt);

        this->style()->unpolish(this);
        this->style()->unpolish(ui->menuRightIcon);
        setProperty("drop-down", QVariant());
        ui->menuRightIcon->setProperty("drop-down", QVariant(""));
        this->style()->polish(ui->menuRightIcon);
        this->style()->polish(this);
    }
}
void FormMenuButton::changeMenuButtonText(QString strText, bool bHasPackage)
{
    m_strPackVersion = strText;
    this->style()->unpolish(ui->menuLeftIcon);
    if (strText.isEmpty())
    {
        ui->menuText->setStyleSheet("color:#A6AEBA;font-size:14px;font-weight:350;background-color:#FFF;");
        ui->menuText->setText(tr("请选择将要升级的版本"));
        ui->menuLeftIcon->hide();
    }
    else
    {
        ui->menuText->setText(strText);
        ui->menuLeftIcon->show();
        if (bHasPackage)
        {//存在该包
            ui->menuLeftIcon->setProperty("download",QVariant("ok"));
        }
        else
        {
            ui->menuLeftIcon->setProperty("download",QVariant());
        }
    }
    this->style()->polish(ui->menuLeftIcon);
}
