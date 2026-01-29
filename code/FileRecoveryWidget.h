#ifndef FILERECOVERYWIDGET_H
#define FILERECOVERYWIDGET_H

#include <QWidget>
#include "baseUI/UIBaseWidget.h"

namespace Ui {
class FileRecoveryWidget;
}

class FileRecoveryWidget : public UIBaseWidget
{
    Q_OBJECT

public:
    explicit FileRecoveryWidget(QWidget *parent = nullptr);
    ~FileRecoveryWidget();
signals:
    void signalFileRecoveryStart(QString strSelectedBackupFileName);
private:
    Ui::FileRecoveryWidget *ui;
};

#endif // FILERECOVERYWIDGET_H
