#ifndef WIDGET2PROGRESSDIALOG_H
#define WIDGET2PROGRESSDIALOG_H

#include <QWidget>
#include <QGraphicsDropShadowEffect>

#include <QDialog>
#include <QStyle>
#include <QAction>
#include <QMouseEvent>
#include <QLabel>
#include <QDebug>
#include "ShadowWindow.h"
#include "MessageWidget.h"
namespace Ui {
class Widget2ProgressDialog;
}

class Widget2ProgressDialog : public QWidget
{
    Q_OBJECT

public:
    explicit Widget2ProgressDialog(QWidget *parent = nullptr);
    ~Widget2ProgressDialog();
    void setProgressRange(int range);
    int  progressRange();
    void setProgressValue(int progress);
    void setWarnMessage(QString text);
    void setHeadLabel(QString text);
    void setTitle(QString title);
    void setDownloadProgress(int progress,int total);

public slots:
    void slot_progressConfirm();

protected:
    bool event(QEvent *event);
private:
    Ui::Widget2ProgressDialog *ui;
    QPoint move_point;
    bool mouse_press;
    QGraphicsDropShadowEffect* shadowEffect;
    int m_range;
    int m_value;
    QString m_functionName;
};

#endif // WIDGET2PROGRESSDIALOG_H
