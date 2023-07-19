#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QStandardItemModel>
#include <QMimeData>
class TableModel: public QStandardItemModel {
    Q_OBJECT

public:
    TableModel(QObject *parent = 0)  : QStandardItemModel(parent) {}

    TableModel(int row, int column, QObject *parent = NULL) : QStandardItemModel(row, column, parent) {}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
        if(role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
        }
        return QStandardItemModel::data(index, role);
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const{
        static int once = 0;
        if(orientation == Qt::Vertical&&once==0) {
            QStandardItemModel::headerData(section,orientation,role);
            once++;
        }
        return QStandardItemModel::headerData(section,orientation,role);

    }

};

#endif // TABLEMODEL_H
