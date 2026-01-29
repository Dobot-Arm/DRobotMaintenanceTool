#ifndef FILEBACKUPWIDGET_H
#define FILEBACKUPWIDGET_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class FileBackupWidget;
}

class FileBackupWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FileBackupWidget(QWidget *parent = nullptr);
    ~FileBackupWidget();

signals:
    void signalFileBackupStart(QString lineSelectedBackupDir ,QString backupFileName);
private:
    Ui::FileBackupWidget *ui;
};

#endif // FILEBACKUPWIDGET_H
