#ifndef MYCHECKBOXHEADER_H
#define MYCHECKBOXHEADER_H

#include <QHeaderView>
#include <QToolTip>
#include <QPainter>
#include <QDebug>
#include "Define.h"
#include <QMouseEvent>
class MyCheckBoxHeader : public QHeaderView
{
    Q_OBJECT

public:
    MyCheckBoxHeader(Qt::Orientation orientation, QWidget* parent = 0);

    bool isChecked() const { return isChecked_; }
    void setIsChecked(bool val);

signals:
    void checkBoxClicked(bool isTrue);

protected:
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;

    void mousePressEvent(QMouseEvent* event);

    void setMouseMoveItem(int bodyCol,int actualResultCol,int expectResultCol);
    void mouseMoveEvent(QMouseEvent *event)override;
    void diyRowMoved(int row, int oldIndex, int newIndex);


private:
    bool isChecked_;

    void redrawCheckBox();
    int mBodyColumn;
    int mActualResultColumn;
    int mExpectResultColumn;
};
#endif // MYCHECKBOXHEADER_H
