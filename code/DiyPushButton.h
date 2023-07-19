#ifndef DIYPUSHBUTTON_H
#define DIYPUSHBUTTON_H

#include <QPushButton>

class DiyPushButton : public QPushButton
{
    Q_OBJECT
public:
    DiyPushButton(QWidget *parent = nullptr);
    DiyPushButton(const QString &text, QWidget *parent = nullptr);
};

#endif // DIYPUSHBUTTON_H
