#include "MyCheckBoxHeader.h"



MyCheckBoxHeader::MyCheckBoxHeader(Qt::Orientation orientation, QWidget* parent /*= 0*/)
    : QHeaderView(orientation, parent)
{
    this->setSectionsMovable(true);
    this->setFirstSectionMovable(true);
    isChecked_ = true;
}

void MyCheckBoxHeader::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if (logicalIndex == colIsCheck)
    {
        QStyleOptionButton option;

        option.rect = QRect(rect.left()+1,rect.y()+5,15,15);

        option.state = QStyle::State_Enabled | QStyle::State_Active;

        if (isChecked_)
            option.state |= QStyle::State_On;
        else
            option.state |= QStyle::State_Off;
        option.state |= QStyle::State_Off;

        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
    }
}

void MyCheckBoxHeader::mousePressEvent(QMouseEvent* event)
{
    if(this->logicalIndexAt(event->pos())==colIsCheck)
    {
        setIsChecked(!isChecked());

        emit checkBoxClicked(isChecked());

    }

}

void MyCheckBoxHeader::redrawCheckBox()
{
    viewport()->update();
}

void MyCheckBoxHeader::setIsChecked(bool val)
{
    if (isChecked_ != val)
    {
        isChecked_ = val;

        redrawCheckBox();
    }
}



void MyCheckBoxHeader::setMouseMoveItem(int bodyCol,int actualResultCol,int expectResultCol)
{
    mBodyColumn = bodyCol;
    mActualResultColumn = actualResultCol;
    mExpectResultColumn = expectResultCol;
}

void MyCheckBoxHeader::mouseMoveEvent(QMouseEvent *event)
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


void MyCheckBoxHeader::diyRowMoved(int row, int oldIndex, int newIndex)
{
    this->moveSection(oldIndex,newIndex);
}


