/*
    This file is part of the Zero Reserve Plugin for Retroshare.

    Zero Reserve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zero Reserve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Zero Reserve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
