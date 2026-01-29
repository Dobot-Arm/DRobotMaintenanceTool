#ifndef WIDGETSYSTEMLOGEXPORT_H
#define WIDGETSYSTEMLOGEXPORT_H

#include <QWidget>
#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QMenu>
#include <QStyleOption>
#include <QDebug>
#include <QDesktopWidget>
#include <QListView>
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class WidgetSystemLogExport;
}

class WidgetSystemLogExport : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit WidgetSystemLogExport(QWidget *parent = nullptr);
    ~WidgetSystemLogExport();
    void setProjectList(QStringList projectList);
    void show();

signals:
    void signal_systemLogExport(QString projectName,QString dstExportDir);

protected slots:
    void slot_selectExportDir();
    void slot_confirm();

private:
    Ui::WidgetSystemLogExport *ui;
    QString m_saveDir;
    QPoint move_point;
    QMenu* m_projectsMenu;
    QStringList m_projectList;
};


#endif // WIDGETSYSTEMLOGEXPORT_H
