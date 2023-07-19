#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QDebug>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);
    ~ProgressDialog();
    void setProgressRange(int range);
    int  progressRange();
    void setProgressValue(int progress);
    void setHeadLabel(QString text);
    void setTitle(QString title);
    void setDownloadProgress(int progress,int total);
    bool event(QEvent *event);
public slots:
    void slot_progressConfirm();
    void slot_closeDialog();
private:
    Ui::ProgressDialog *ui;
    QPoint move_point;
    bool mouse_press;
    QGraphicsDropShadowEffect* shadowEffect;
    int m_range;
protected:
//    //鼠标按下
//    void mousePressEvent(QMouseEvent *e);
//    //鼠标移动
//    void mouseMoveEvent(QMouseEvent *e);
//    //鼠标释放
//    void mouseReleaseEvent(QMouseEvent *e);
};

#endif // PROGRESSDIALOG_H
