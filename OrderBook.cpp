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

OrderBook::OrderBook()
{
    OrderBook::Order * ask = new OrderBook::Order();
    ask->setPrice( "888.99" );
    ask->setCurrencyFromName( "Euro" );
    ask->m_amount = "77.77";
    ask->m_orderType = OrderBook::Order::ASK;
    orders.append(ask);
}

OrderBook::~OrderBook()
{
    for( QList<OrderBook::Order*>::iterator it = orders.begin(); it != orders.end(); it++) delete *it;
}

QModelIndex OrderBook::index(int x, int y, const QModelIndex&) const
{
    return createIndex(x, y);
}

QModelIndex OrderBook::parent(const QModelIndex&) const
{
    return QModelIndex();
}

int OrderBook::rowCount(const QModelIndex&) const
{
    return orders.size();
}

int OrderBook::columnCount(const QModelIndex&) const
{
    return 2;
}


QVariant OrderBook::data( const QModelIndex& index, int role ) const
{
    if (role == Qt::DisplayRole && index.row() < orders.size()){
        Order * order = orders[index.row()];
        switch(index.column()){
            case 0:
                return QVariant(order->m_amount);
                break;
            case 1:
                return QVariant(order->m_price);
                break;
            default:
                return QVariant();
        }

    }
    return QVariant();
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

void OrderBook::addOrder( Order* order )
{
    beginInsertRows( QModelIndex(), orders.size(), orders.size());
    orders.append(order);
    endInsertRows();
}

bool OrderBook::Order::setPrice(QString price)
{
    bool ok;
    m_price = price;
    m_price_d = price.toDouble( &ok );
    return ok;
}

void OrderBook::Order::setCurrencyFromName( QString currency )
{
    int index = 0;
    while(Currency::currencyNames[ index ]){
        if( currency == Currency::currencyNames[ index ]){
            this->m_currency = (Currency::CurrencySympols)index;
            return;
        }
        index++;
    }
// TODO: throw
}
