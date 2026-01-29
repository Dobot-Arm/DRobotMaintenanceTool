#ifndef FORMMACDETAIL_H
#define FORMMACDETAIL_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"
#include <QList>
#include <QPair>

class FormMacDetail : public UIBaseWidget
{
    Q_OBJECT
public:
    explicit FormMacDetail(QWidget *parent,QList<QPair<QString,QString>> allCard);
    void show();

signals:

};

#endif // FORMMACDETAIL_H
