#include "MaskDialogContainer.h"
#include "Define.h"
#include <QGridLayout>
#include "MainWidget2.h"
#include <QPainter>
MaskDialogContainer::MaskDialogContainer() : QDialog(PublicgGetMainWindow())
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    setAttribute(Qt::WA_TranslucentBackground, true);
    SET_STYLE_SHEET(MaskDialogContainer);

    m_pWidget = nullptr;
    auto pLayout = new QGridLayout();
    setLayout(pLayout);
}

void MaskDialogContainer::createMask(QWidget *pWidget)
{
    m_pWidget = pWidget;
    m_pWidget->installEventFilter(this);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setWidthForHeight(pWidget->sizePolicy().hasWidthForHeight());
    sizePolicy.setHeightForWidth(pWidget->sizePolicy().hasHeightForWidth());
    pWidget->setSizePolicy(sizePolicy);

    static_cast<QGridLayout*>(layout())->addWidget(pWidget, 0, 0, 1, 1);
}

void MaskDialogContainer::replaceMask(QWidget *pWidget)
{
    if (m_pWidget)
    {
        m_pWidget->removeEventFilter(this);
        m_pWidget->hide();
    }
    m_pWidget = pWidget;
    m_pWidget->installEventFilter(this);
    m_pWidget->show();
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setWidthForHeight(pWidget->sizePolicy().hasWidthForHeight());
    sizePolicy.setHeightForWidth(pWidget->sizePolicy().hasHeightForWidth());
    pWidget->setSizePolicy(sizePolicy);

    static_cast<QGridLayout*>(layout())->takeAt(0);
    static_cast<QGridLayout*>(layout())->addWidget(pWidget, 0, 0, 1, 1);
}

void MaskDialogContainer::showEvent(QShowEvent *pEvent)
{
    this->setGeometry(PublicgGetMainWindow()->geometry());
}

void MaskDialogContainer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), palette().color(QPalette::Window));
}

bool MaskDialogContainer::eventFilter(QObject *pSender, QEvent *pEvent)
{
    if (pSender == m_pWidget && pEvent->type() == QEvent::Close)
    {
        this->accept();
        return true;
    }
    return QDialog::eventFilter(pSender, pEvent);
}
