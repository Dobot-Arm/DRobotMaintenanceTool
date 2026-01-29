#ifndef FILECHECKTIPWIDGET_H
#define FILECHECKTIPWIDGET_H

#include <QWidget>
#include "DoStatus.h"
#include "baseUI/UIBaseWidget.h"
namespace Ui {
class FileCheckTipWidget;
}

class FileCheckTipWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FileCheckTipWidget(QWidget *parent = nullptr);
    ~FileCheckTipWidget();
    void setStatus(FileCheckStatus status);
private:
    Ui::FileCheckTipWidget *ui;
    QStringList m_missingFiles;
private slots:
    void slotClose();
};

#endif // FILECHECKTIPWIDGET_H
