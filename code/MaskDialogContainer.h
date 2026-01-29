#ifndef MASKDIALOGCONTAINER_H
#define MASKDIALOGCONTAINER_H

#include <QDialog>
#include "baseUI/BaseWidget.h"

class MaskDialogContainer : public QDialog
{
    Q_OBJECT
public:
    MaskDialogContainer();

    /*
     * 创建一个遮罩层，pWidget需要new出来，而且new的时候，不要指定parent Widget，内部会自动管理pWidget
    */
    void createMask(QWidget* pWidget);
    void replaceMask(QWidget* pWidget);

    template<class WidgetT>
    WidgetT* getWidget()
    {
        return static_cast<WidgetT*>(m_pWidget);
    }

protected:
    bool eventFilter(QObject* pSender, QEvent* pEvent) override;
    void showEvent(QShowEvent* pEvent) override;
    void paintEvent(QPaintEvent*) override;

private:
    QWidget* m_pWidget;
};

#endif // MASKDIALOGCONTAINER_H
