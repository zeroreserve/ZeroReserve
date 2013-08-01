/*
    This file is part of the Zero Reserve Plugin for Retroshare.

    Zero Reserve is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zero Reserve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Zero Reserve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "OrderBook.h"

#include <iostream>


bool OrderBook::compareOrder( const Order * left, const Order * right ){
    if(left->m_orderType == Order::ASK){
        if( left->m_price_d < right->m_price_d){
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if( left->m_price_d < right->m_price_d){
            return false;
        }
        else {
            return true;
        }
    }
}

OrderBook::OrderBook()
{
}

OrderBook::~OrderBook()
{
    for( OrderIterator it = m_orders.begin(); it != m_orders.end(); it++) delete *it;
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
    return m_filteredOrders.size();
}

int OrderBook::columnCount(const QModelIndex&) const
{
    return 2;
}


QVariant OrderBook::data( const QModelIndex& index, int role ) const
{
    if (role == Qt::DisplayRole && index.row() < m_filteredOrders.size()){
        Order * order = m_filteredOrders[index.row()];
        switch(index.column()){
            case 0:
                return QVariant(order->m_amount);
            case 1:
                return QVariant(order->m_price);
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

void OrderBook::setCurrency( const QString & currency )
{
    m_currency = Currency::getCurrencyByName( currency.toStdString() );
    filterOrders();
}

void OrderBook::filterOrders()
{
    beginResetModel();
    m_filteredOrders.clear();
    for(OrderIterator it = m_orders.begin(); it != m_orders.end(); it++){
        if( (*it)->m_currency == m_currency )
            m_filteredOrders.append( *it );
    }
    endResetModel();
}



bool OrderBook::addOrder( Order * order )
{
    // do not insert an order that already exists
    for(OrderIterator it = m_orders.begin(); it != m_orders.end(); it++){
        if( *order == *(*it) ) return false;
    }
    std::cerr << "Zero Reserve: Inserting Type: " << (int)order->m_orderType <<
                 " Currency: " << order->m_currency << std::endl;

    m_orders.append(order);
    if( order->m_currency != m_currency ) return true;

    beginInsertRows( QModelIndex(), m_filteredOrders.size(), m_filteredOrders.size());
    filterOrders();
    qSort(m_filteredOrders.begin(), m_filteredOrders.end(), compareOrder);
    endInsertRows();
    return true;
}

bool OrderBook::Order::setPrice(QString price)
{
    bool ok;
    m_price = price;
    m_price_d = price.toDouble( &ok );
    return ok;
}


bool OrderBook::Order::operator == ( const OrderBook::Order & other )
{
    if(m_trader_id == other.m_trader_id &&
       m_amount == other.m_amount &&
       m_price == other.m_price &&
       m_currency == other.m_currency &&
       m_timeStamp == other.m_timeStamp
       )
        return true;
    else
        return false;
}


