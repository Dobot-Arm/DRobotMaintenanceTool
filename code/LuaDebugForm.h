#ifndef LUADEBUGFORM_H
#define LUADEBUGFORM_H

#include <QWidget>

namespace Ui {
class LuaDebugForm;
}

class LuaDebugForm : public QWidget
{
    Q_OBJECT

public:
    explicit LuaDebugForm(QWidget *parent = nullptr);
    ~LuaDebugForm();

private slots:
    void slotClear();
    void slotLog(QString strLog, QtMsgType type);

private:
    Ui::LuaDebugForm *ui;
};

#endif // LUADEBUGFORM_H
