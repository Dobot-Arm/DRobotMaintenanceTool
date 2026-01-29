#ifndef WIDGETTOAST_H
#define WIDGETTOAST_H

#include <QWidget>
#include <QTimer>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class WidgetToast;
}

class WidgetToast : public UIBaseWidget
{
    Q_OBJECT

    explicit WidgetToast(QWidget *parent = nullptr);
    void setText(const QString &strTxt, int iType, int iDurationsMillseconds);
public:
    ~WidgetToast();
    static void ok(const QString& strTxt, int iDurationsMillseconds);
    static void fail(const QString& strTxt, int iDurationsMillseconds);
    static void warn(const QString& strTxt, int iDurationsMillseconds);

private:
    Ui::WidgetToast *ui;
    QTimer* m_pTimer;
};

#endif // WIDGETTOAST_H
