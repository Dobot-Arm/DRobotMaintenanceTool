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
/**************************************************************************
  *是否生效 默认第0列，只能输入Y或N
  * 利用QCheckBox委托对输入进行限制
  **************************************************************************/
class IsCheckDelegate : public QItemDelegate {
    Q_OBJECT

  public:
    explicit IsCheckDelegate(QObject *parent = nullptr): QItemDelegate(parent) { }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const {
        Q_UNUSED(option)
        Q_UNUSED(index)
        QCheckBox *editor = new QCheckBox(parent);
        editor->setObjectName("checkBox");
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const {
        QString text = index.model()->data(index, Qt::EditRole).toString();
        if(text == "") {
            return;
        }
        qDebug()<<" setEditorData ---------------- " <<index.row();
#if 1
        QCheckBox *checkBox = static_cast<QCheckBox *>(editor);
        if(text == "TRUE") {
            checkBox->setChecked(true);
        }
        if(text == "FALSE") {
            checkBox->setChecked(false);
        }
#endif

    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const {
        qDebug()<<" setModelData +++++++++++  "<<index.row();
        QCheckBox *checkBox = static_cast<QCheckBox *>(editor);
        bool state = checkBox->checkState();
        if(state == true) {
            model->setData(index, "TRUE", Qt::EditRole);
        }
        if(state == false) {
            model->setData(index, "FALSE", Qt::EditRole);
        }

    }

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const {
        Q_UNUSED(option)
        Q_UNUSED(index)

        editor->setGeometry(option.rect);
    }

  private:
    bool m_btnTextChange = false;
    QMouseEvent *event;

};
#endif // TABLEDELEGATE_H
