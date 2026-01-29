#include "UIBaseWidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QStyle>

UIBaseWidget::UIBaseWidget(QWidget *parent) : QWidget(parent)
{

}

UIBaseWidget::~UIBaseWidget()
{

}

void UIBaseWidget::updateStyleSheet(QWidget* p)
{
    //p->style()->unpolish(p);
    //p->style()->polish(p);
    p->setStyle(p->style());
}

void UIBaseWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);

    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
