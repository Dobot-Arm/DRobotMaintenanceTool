#include "DTableView.h"


DTableView::DTableView(QWidget *parent)
    : QTableView(parent)
{
    mBodyColumn = colBody;
    mActualResultColumn = colActualResult;
    mExpectResultColumn = colExpectResult;
    this->horizontalHeader()->setSectionsMovable(true);
    connect(this->verticalHeader(),&QHeaderView::sectionMoved,this,&DTableView::slotSectionMoved);
}

void DTableView::slotSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{

//    this->verticalHeader()->reset();
    emit signalUpdateVerticalHeaderData(logicalIndex,oldVisualIndex,newVisualIndex);
}




void DTableView::setMouseMoveItem(int bodyCol,int actualResultCol,int expectResultCol)
{
    mBodyColumn = bodyCol;
    mActualResultColumn = actualResultCol;
    mExpectResultColumn = expectResultCol;
}

void DTableView::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex index = this->indexAt(event->pos());
    int column = index.column();
//    qDebug()<<"column "<<column;
    QVariant data =this->model()->data(index);
    QToolTip::hideText();
    //&&(column==mBodyColumn||column==mActualResultColumn||column==mExpectResultColumn)
    if(data.isValid()){
//        qDebug()<<"column==colBody "<<column;
        QToolTip::showText(event->globalPos(),data.toString(),this,QRect(),8000);
    }
}

void DTableView::sleep(int milliseconds)
{
    QTime dieTime = QTime::currentTime().addMSecs(milliseconds);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void DTableView::diyRowMoved(int row, int oldIndex, int newIndex)
{
    this->rowMoved(row,oldIndex,newIndex);
}

