#ifndef TABLEDELEGATE_H
#define TABLEDELEGATE_H

#include "TableModel.h"
#include "DiyPushButton.h"
#include <QItemDelegate>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QTextEdit>
#include  <QDebug>
#include <QProgressBar>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QDialog>
#include "Define.h"
//#include "BodyInfo.h"
class TableDelegate: public QItemDelegate {
    Q_OBJECT

  public:
    explicit TableDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}

};

/**************************************************************************
  *结果 默认第2列 单独读取列
  * 利用QPushButton和正则表达式对输入进行限制
  **************************************************************************/
class SingleReadDelegate : public QItemDelegate {
    Q_OBJECT

  public:
    explicit SingleReadDelegate(QObject *parent = 0): QItemDelegate(parent) { }
  private:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const {
        Q_UNUSED(option)
        qDebug()<<"createEditor";
        DiyPushButton *editor = new DiyPushButton(parent);
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const {
        qDebug()<<"setEditorData";
        QString text = index.model()->data(index, Qt::EditRole).toString();
        DiyPushButton *btn = static_cast<DiyPushButton *>(editor);
        btn->setText(text);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const {
        qDebug()<<"setModelData";
        DiyPushButton *btn = static_cast<DiyPushButton *>(editor);
        QString text = btn->text();
        model->setData(index, text, Qt::EditRole);
    }

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const {
        Q_UNUSED(index)
        editor->setGeometry(option.rect);
    }

    // 绘制图像
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        DiyPushButton* button = m_pushbtns.value(index);
        if(button!=nullptr){
        }else{
            button = new DiyPushButton("read");
            (const_cast<SingleReadDelegate *>(this))->m_pushbtns.insert(index, button);
        }
        button->setGeometry(option.rect);
        painter->save();
        painter->translate(option.rect.topLeft());
        button->render(painter);
        painter->restore();



    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
        if (event->type() == QEvent::MouseButtonPress) {
            qDebug()<<" QEvent::MouseButtonPress ";
            m_pushbtns.value(index)->setStyleSheet("QPushButton{color: rgb(0, 255, 255); padding-top:2px;padding-left:2px;}");
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            qDebug()<<" QEvent::MouseButtonRelease ";
            m_pushbtns.value(index)->setStyleSheet("");
            emit startSingleRead(index.row());
        }
        QPainter* painter = new QPainter();
        m_pushbtns.value(index)->setGeometry(option.rect);
        painter->save();
        painter->translate(option.rect.topLeft());
        m_pushbtns.value(index)->render(painter);
        painter->restore();

        return true;
    }

  signals:
    void startSingleRead(int currentRow);

  private:
    QPushButton mbtn;
    QMap<QModelIndex, QStyleOptionButton  *> m_btns;
    QMap<QModelIndex, DiyPushButton  *> m_pushbtns;

};


#include <QMessageBox>
class ButtonDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit ButtonDelegate(QObject *parent = 0): QItemDelegate(parent) { }
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QPair<QStyleOptionButton*, QStyleOptionButton*>* buttons = m_btns.value(index);
        if (!buttons) {
            QStyleOptionButton* button1 = new QStyleOptionButton();
            //button1->rect = option.rect.adjusted(4, 4, -(option.rect.width() / 2 + 4) , -4); //
            button1->text = "X";
            button1->state |= QStyle::State_Enabled;

            QStyleOptionButton* button2 = new QStyleOptionButton();
            //button2->rect = option.rect.adjusted(button1->rect.width() + 4, 4, -4, -4);
            button2->text = "Y";
            button2->state |= QStyle::State_Enabled;
            buttons =new  QPair<QStyleOptionButton*, QStyleOptionButton*>(button1, button2);
            (const_cast<ButtonDelegate *>(this))->m_btns.insert(index, buttons);
        }
        buttons->first->rect = option.rect.adjusted(4, 4, -(option.rect.width() / 2 + 4) , -4); //
        buttons->second->rect = option.rect.adjusted(buttons->first->rect.width() + 4, 4, -4, -4);
        painter->save();

        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());

        }
        painter->restore();
        QApplication::style()->drawControl(QStyle::CE_PushButton, buttons->first, painter);
        QApplication::style()->drawControl(QStyle::CE_PushButton, buttons->second, painter);
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        if (event->type() == QEvent::MouseButtonPress) {

            QMouseEvent* e =(QMouseEvent*)event;

            if (m_btns.contains(index)) {
                QPair<QStyleOptionButton*, QStyleOptionButton*>* btns = m_btns.value(index);
                if (btns->first->rect.contains(e->x(), e->y())) {
                    btns->first->state |= QStyle::State_Sunken;
                }
                else if(btns->second->rect.contains(e->x(), e->y())) {
                    btns->second->state |= QStyle::State_Sunken;
                }
            }
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* e =(QMouseEvent*)event;

            if (m_btns.contains(index)) {
                QPair<QStyleOptionButton*, QStyleOptionButton*>* btns = m_btns.value(index);
                if (btns->first->rect.contains(e->x(), e->y())) {
                    btns->first->state &= (~QStyle::State_Sunken);
                    showMsg(tr("btn1 column %1").arg(index.column()));
                } else if(btns->second->rect.contains(e->x(), e->y())) {
                    btns->second->state &= (~QStyle::State_Sunken);
                    showMsg(tr("btn2 row %1").arg(index.row()));
                }
            }
        }
    }

    void showMsg(QString str)
    {
        QMessageBox msg;
        msg.setText(str);
        msg.exec();
    }

private:

    typedef QMap<QModelIndex, QPair<QStyleOptionButton*, QStyleOptionButton*>* >  collButtons;
    collButtons m_btns;

};

#include <QMessageBox>
#include <QHBoxLayout>
#include <QList>
class ReadAWriteBtnDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ReadAWriteBtnDelegate(QObject *parent = 0): QItemDelegate(parent) {
        connect(this,&ReadAWriteBtnDelegate::signal_renderPIDWidget,this,[&](int pidWidgetRow)
        {
            m_pidWidgetList.append(pidWidgetRow);
        });
    }
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        return nullptr;
    }
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {

        QWidget* btnWidget = m_btnWidgets.value(index);
        if(btnWidget == nullptr)
        {

            btnWidget = new QWidget();
            QHBoxLayout *layout = new QHBoxLayout;
            btnWidget->setLayout(layout);
            btnWidget->setStyleSheet("font-size: 16px;background-color:rgb(255,255,255);color:#0047BB;");
            layout->setSpacing(0);
            layout->setContentsMargins(0, 0, 0, 0);
            QPushButton* button1 = new QPushButton("Read");
            button1->setCursor(Qt::PointingHandCursor);
            QPushButton* button2 = new QPushButton("Write");
            button2->setCursor(Qt::PointingHandCursor);
            //只读的选项 写按钮 禁止
            if(index.data().isValid()&&index.data().toBool())
            {
                button2->setDisabled(true);
            }
            button1->setMaximumWidth(60);
            button1->setStyleSheet("QPushButton{font-size: 16px;background-color: transparent;border-top:none;border-bottom:none;border-left:none; border-right: 1px solid #0047BB;}");
            button2->setMaximumWidth(60);
            button2->setStyleSheet("QPushButton{font-size: 16px;background-color: transparent;border:none;}QPushButton:disabled{color:rgba(0, 71, 187, 80);}");
            layout->addWidget(button1);
            layout->addWidget(button2);
            (const_cast<ReadAWriteBtnDelegate *>(this))->m_btnWidgets.insert(index, btnWidget);

        }
        btnWidget->setStyleSheet("background-color:rgb(255,255,255);color:#0047BB;");
        //给PID参数设置特殊背景色
        for(int row : m_pidWidgetList)
        {
            if(row == index.row())
            {
                btnWidget->setStyleSheet("background-color:rgba(238, 244, 252);color:#0047BB;");
            }

        }

        btnWidget->setGeometry(option.rect);
        painter->save();
        painter->translate(option.rect.topLeft());
        btnWidget->render(painter);
        painter->restore();
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
    {

        if (event->type() == QEvent::MouseButtonPress) {

            qDebug()<<"index.data  "<<index.data();
            QMouseEvent* e =(QMouseEvent*)event;
            if (m_btnWidgets.contains(index)) {
                QWidget* btnWidget = m_btnWidgets.value(index);
                QPoint p = btnWidget->mapFromGlobal(e->pos());
                qDebug()<<"p --> "<<p;
                QWidget* widget = btnWidget->childAt(p);
                if(widget!=nullptr)
                {
                    QPushButton* btn = (QPushButton*)widget;
                    if(btn->text().contains(QString("Read")))
                    {
                        emit startSingleRead(index.row());
                        qDebug()<<QString("XXXXXXXXXXX %1  %2").arg(index.row()).arg(index.column());
                    }
                    if(btn->text().contains(QString("Write"))&&btn->isEnabled())
                    {
                        emit startSingleWrite(index.row());
                        qDebug()<<QString("YYYYYYYYYYYYYYYYYY %1  %2").arg(index.row()).arg(index.column());
                    }
                }

            }
        }
        return QItemDelegate::editorEvent(event,model,option,index);
    }

    void showMsg(QString str)
    {
        QMessageBox msg;
        msg.setText(str);
        msg.exec();
    }

    void clear(){
        for (auto itr=m_btnWidgets.begin(); itr!=m_btnWidgets.end();++itr){
            itr.value()->deleteLater();
        }
        m_btnWidgets.clear();
        m_pidWidgetList.clear();
    }
signals:
    void startSingleRead(int currentRow);
    void startSingleWrite(int currentRow);
    void signal_renderPIDWidget(int pidWidgetRow);
private:
    QMap<QModelIndex,QWidget* >m_btnWidgets;
public:
    QList<int> m_pidWidgetList;

};


/**************************************************************************
  *结果 默认第1列 单独写入列
  * 利用QPushButton和正则表达式对输入进行限制
  **************************************************************************/
class SingleWriteDelegate : public QItemDelegate {
    Q_OBJECT

  public:
    explicit SingleWriteDelegate(QObject *parent = 0): QItemDelegate(parent) { }
  private:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const {
        Q_UNUSED(option)
        qDebug()<<"createEditor";
        DiyPushButton *editor = new DiyPushButton(parent);
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const {
        qDebug()<<"setEditorData";
        QString text = index.model()->data(index, Qt::EditRole).toString();
        DiyPushButton *btn = static_cast<DiyPushButton *>(editor);
        btn->setText(text);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const {
        qDebug()<<"setModelData";
        DiyPushButton *btn = static_cast<DiyPushButton *>(editor);
        QString text = btn->text();
        model->setData(index, text, Qt::EditRole);
    }

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const {
        Q_UNUSED(index)
        editor->setGeometry(option.rect);
    }

    // 绘制图像
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        DiyPushButton* button = m_pushbtns.value(index);
        if(button!=nullptr){
        }else{
            button = new DiyPushButton("write");
            (const_cast<SingleWriteDelegate *>(this))->m_pushbtns.insert(index, button);
        }
        button->setGeometry(option.rect);
        painter->save();
        painter->translate(option.rect.topLeft());
        button->render(painter);
        painter->restore();

    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
        if (event->type() == QEvent::MouseButtonPress) {
            qDebug()<<" QEvent::MouseButtonPress ";
            m_pushbtns.value(index)->setStyleSheet("QPushButton{color: rgb(0, 255, 255); padding-top:2px;padding-left:2px;}");
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            qDebug()<<" QEvent::MouseButtonRelease ";
            m_pushbtns.value(index)->setStyleSheet("");
            emit startSingleWrite(index.row());
        }
        QPainter* painter = new QPainter();
        m_pushbtns.value(index)->setGeometry(option.rect);
        painter->save();
        painter->translate(option.rect.topLeft());
        m_pushbtns.value(index)->render(painter);
        painter->restore();

        return true;
    }

  signals:
    void startSingleWrite(int currentRow);

  private:
    QPushButton mbtn;
    QMap<QModelIndex, QStyleOptionButton  *> m_btns;
    QMap<QModelIndex, DiyPushButton  *> m_pushbtns;

};

#include "MyCheckBox.h"
#include <QLabel>
/**************************************************************************
  *是否生效 默认第0列，只能输入Y或N
  * 利用QCheckBox委托对输入进行限制
  **************************************************************************/
class IsCheckDelegate : public QItemDelegate {
    Q_OBJECT
  signals:
    void signalIsCheckHeader(bool data);
  public:
    explicit IsCheckDelegate(QObject *parent = nullptr): QItemDelegate(parent) { }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const {
        Q_UNUSED(option)
        Q_UNUSED(index)
        return nullptr;
    }

//    void setEditorData(QWidget *editor, const QModelIndex &index) const {
//        QString text = index.model()->data(index, Qt::EditRole).toString();
//        if(text == "") {
//            return;
//        }
//        qDebug()<<" setEditorData ---------------- " <<index.row();
//#if 1
//        QCheckBox *checkBox = static_cast<QCheckBox *>(editor);
//        if(checkBox->isChecked())
//        {
//            checkBox->setChecked(false);
//        }
//        else
//        {
//            checkBox->setChecked(true);
//        }
////        if(text == "TRUE") {
////            checkBox->setChecked(true);
////        }
////        if(text == "FALSE") {
////            checkBox->setChecked(false);
////        }
//#endif

//    }

//    void setModelData(QWidget *editor, QAbstractItemModel *model,
//                      const QModelIndex &index) const {
//        qDebug()<<" setModelData +++++++++++  "<<index.row();
//        QCheckBox *checkBox = static_cast<QCheckBox *>(editor);
//        bool state = checkBox->checkState();
////        if(state == true) {
////            model->setData(index, "TRUE", Qt::EditRole);
////        }
////        if(state == false) {
////            model->setData(index, "FALSE", Qt::EditRole);
////        }

//    }

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const {
        Q_UNUSED(option)
        Q_UNUSED(index)

        editor->setGeometry(option.rect);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem viewOption(option);
        if (option.state.testFlag(QStyle::State_HasFocus))
            viewOption.state = viewOption.state ^ QStyle::State_HasFocus;

        QItemDelegate::paint(painter, viewOption, index);

        if (index.column() == colIsCheck)
        {
            bool data = index.model()->data(index, Qt::UserRole).toBool();

            QStyleOptionButton checkBoxStyle;
            checkBoxStyle.state = data ? QStyle::State_On : QStyle::State_Off;
            checkBoxStyle.state |= QStyle::State_Enabled;
            checkBoxStyle.iconSize = QSize(10, 10);
            checkBoxStyle.rect = option.rect;

            QCheckBox checkBox;
            QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkBoxStyle, painter, &checkBox);
        }
    }

    // 响应鼠标事件，更新数据
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        QRect decorationRect = option.rect;

        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if ((event->type() == QEvent::MouseButtonPress ||event->type() == QEvent::MouseButtonDblClick)&& decorationRect.contains(mouseEvent->pos()))
        {
            if (index.column() == colIsCheck)
            {
                bool data = model->data(index, Qt::UserRole).toBool();
                emit signalIsCheckHeader(!data);
                model->setData(index, !data, Qt::UserRole);
            }
        }

        return QItemDelegate::editorEvent(event, model, option, index);
    }


  private:
    bool m_btnTextChange = false;
    QMouseEvent *event;

};
#endif // TABLEDELEGATE_H
