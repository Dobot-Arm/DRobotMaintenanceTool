#ifndef DTABLEVIEW_H
#define DTABLEVIEW_H

#include <QTableView>
#include <QMouseEvent>
#include <QDebug>
#include <QCoreApplication>
#include <QToolTip>
#include <QTime>
#include <QHeaderView>
#include <QModelIndex>
#include <QCheckBox>
#include <QPainter>

#include "Define.h"

class DTableView : public QTableView
{
    Q_OBJECT
public:
    DTableView(QWidget *parent = nullptr);
public slots:
    void slotSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);

signals:
    void signalUpdateVerticalHeaderData(int logicalIndex, int oldVisualIndex, int newVisualIndex);
public :
   void setMouseMoveItem(int bodyCol,int actualResultCol,int expectResultCol);
   void mouseMoveEvent(QMouseEvent *event)override;
   void sleep(int milliseconds);
   void diyRowMoved(int row, int oldIndex, int newIndex);


private:
   int mBodyColumn;
   int mActualResultColumn;
   int mExpectResultColumn;

};

#endif // DTABLEVIEW_H
