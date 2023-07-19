#include "DiyPushButton.h"
#include <QDebug>
DiyPushButton::DiyPushButton(QWidget *parent)
    : QPushButton(parent)
{

}

DiyPushButton::DiyPushButton(const QString &text, QWidget *parent):QPushButton(parent)
{
    this->setText(text);
    this->setStyleSheet("QPushButton:pressed{color: rgb(0, 255, 255); padding-top:2px;padding-left:2px;}");
}

