#include "OrderBook.h"

OrderBook::OrderBook(QObject *parent) :
    QAbstractItemModel(parent)
{
}

QModelIndex OrderBook::index(int, int, const QModelIndex&) const
{
    return QModelIndex();
}

QModelIndex OrderBook::parent(const QModelIndex&) const
{
    return QModelIndex();
}

int OrderBook::rowCount(const QModelIndex&) const
{
    return 0;
}

int OrderBook::columnCount(const QModelIndex&) const
{
    return 2;
}

QVariant OrderBook::data(const QModelIndex&, int) const
{
    return 0;
}

QVariant OrderBook::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Volume");
            case 1:
                return QString("Price");
            }
        }
    }
    return QVariant();
}
