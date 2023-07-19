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
namespace Ui {
class WidgetSystemLogExport;
}

class WidgetSystemLogExport : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSystemLogExport(QWidget *parent = nullptr);
    ~WidgetSystemLogExport();
    void setComboxProjectList(QStringList projectList);
    void setProjectList(QStringList projectList);
    void retranslate();
public slots:
    void slot_selectExportDir();
    void slot_cancel();
    void slot_confirm();
signals:
    void signal_systemLogExport(QString projectName,QString dstExportDir);
private:
    Ui::WidgetSystemLogExport *ui;
    QString m_saveDir;
    bool mouse_press;
    QPoint move_point;
    QMenu* m_projectsMenu;
    QStringList m_projectList;
    void setDiyPos();
    int m_posDiyX;
    int m_posDiyY;
protected:
//    //鼠标按下
//    void mousePressEvent(QMouseEvent *e);
//    //鼠标移动
//    void mouseMoveEvent(QMouseEvent *e);
//    //鼠标释放
//    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);
};


#endif // WIDGETSYSTEMLOGEXPORT_H
