#ifndef FILEBACKUPWIDGET_H
#define FILEBACKUPWIDGET_H

#include <QWidget>

namespace Ui {
class FileBackupWidget;
}

class FileBackupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileBackupWidget(QWidget *parent = nullptr);
    ~FileBackupWidget();

private:
    Ui::FileBackupWidget *ui;
};

#endif // FILEBACKUPWIDGET_H
