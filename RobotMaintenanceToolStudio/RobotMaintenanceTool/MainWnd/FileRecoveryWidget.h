#ifndef FILERECOVERYWIDGET_H
#define FILERECOVERYWIDGET_H

#include <QWidget>

namespace Ui {
class FileRecoveryWidget;
}

class FileRecoveryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileRecoveryWidget(QWidget *parent = nullptr);
    ~FileRecoveryWidget();

private:
    Ui::FileRecoveryWidget *ui;
};

#endif // FILERECOVERYWIDGET_H
