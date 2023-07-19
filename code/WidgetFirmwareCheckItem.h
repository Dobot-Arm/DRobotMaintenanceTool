#ifndef WIDGETFIRMWARECHECKITEM_H
#define WIDGETFIRMWARECHECKITEM_H

#include <QWidget>

namespace Ui {
class WidgetFirmwareCheckItem;
}

class WidgetFirmwareCheckItem : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetFirmwareCheckItem(QWidget *parent = nullptr);
    ~WidgetFirmwareCheckItem();
    void setTheme(QString theme);
    QString theme();
    void setLabelStatus(int status);
    void setThemeStatus(int status);
    void setMainThemeStatus(int status);
    void setBigFont();
signals:
    void signal_firmwareCheckRepair(QString theme);
private:
    Ui::WidgetFirmwareCheckItem *ui;
    QString m_theme;
    bool isBigTheme;

};

#endif // WIDGETFIRMWARECHECKITEM_H
