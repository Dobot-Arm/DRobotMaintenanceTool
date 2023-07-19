#ifndef MYCHECKBOX_H
#define MYCHECKBOX_H

#include <QCheckBox>

class MyCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    MyCheckBox(QWidget *parent = nullptr);
    MyCheckBox(const QString &text, QWidget *parent = nullptr);
};

#endif // MYCHECKBOX_H
