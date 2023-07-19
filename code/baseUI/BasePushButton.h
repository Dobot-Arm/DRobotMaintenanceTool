#ifndef BASEPUSHBUTTON_H
#define BASEPUSHBUTTON_H

#include <QObject>
#include <QPushButton>
class BasePushButton :public QPushButton
{

public:
    explicit BasePushButton(QWidget *parent = nullptr);
    ~BasePushButton();
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // BASEPUSHBUTTON_H
