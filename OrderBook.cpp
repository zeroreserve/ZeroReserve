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
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"

#include <openssl/sha.h>
#include <iostream>


bool OrderBook::compareOrder( const Order * left, const Order * right ){
    if(left->m_orderType == Order::ASK){
        if( left->m_price < right->m_price){
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if( left->m_price < right->m_price){
            return false;
        }
        else {
            return true;
        }
    }
}

OrderBook::OrderBook()
{
    m_myOrders = NULL;
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
                return QVariant( order->m_amount.toDouble() );
            case 1:
                return QVariant( order->m_price.toDouble() );
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
    beginResetModel();
    filterOrders( m_filteredOrders, m_currency );
    endResetModel();
}

void OrderBook::filterOrders( OrderList & filteredOrders, const Currency::CurrencySymbols currencySym )
{
    filteredOrders.clear();
    for(OrderIterator it = m_orders.begin(); it != m_orders.end(); it++){
        if( (*it)->m_currency == currencySym )
            filteredOrders.append( *it );
    }
    qSort( filteredOrders.begin(), filteredOrders.end(), compareOrder);
}


ZR::RetVal OrderBook::processOrder( Order* order )
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if( currentTime - order->m_timeStamp >  172800000        // older than two days
            || order->m_timeStamp - currentTime > 3600000){  // or more than an hour in the future
        return ZR::ZR_FAILURE;
    }

    if( Order::FILLED == order->m_purpose || Order::CANCEL == order->m_purpose ){
        Order * oldOrder = remove( order );
        if( oldOrder ){
            p3zr->publishOrder( order );
            delete oldOrder;
            return ZR::ZR_SUCCESS;
        }
        return ZR::ZR_FINISH;
    }

    if( Order::PARTLY_FILLED == order->m_purpose ){
        for(OrderIterator it = m_orders.begin(); it != m_orders.end(); it++){
            if( *order == *(*it) ){
                if( order->m_amount == (*it)->m_amount )
                    return ZR::ZR_FINISH; // we have it already - do nothing
            }
        }
        remove( order );  // remove so it gets reinserted with the updates values below.
    }

    // do not insert an order that already exists
    for(OrderIterator it = m_orders.begin(); it != m_orders.end(); it++){
        if( *order == *(*it) ) return ZR::ZR_FINISH;
    }

    if( order->m_isMyOrder ){
        if( ZR::ZR_FINISH == m_myOrders->match( order ) ){
            return ZR::ZR_FINISH; // completely executed - do not add
        }
        m_myOrders->addOrder( order );
    }
    else{
        if( ZR::ZR_FINISH == m_myOrders->matchOther( order ) ){
            return ZR::ZR_FINISH; // completely executed - do not add
        }
    }

    if( ZR::ZR_FINISH != addOrder( order ) ){
        p3zr->publishOrder( order );
    }

    return ZR::ZR_SUCCESS;
}



int OrderBook::addOrder( Order * order )
{
    std::cerr << "Zero Reserve: Inserting Type: " << (int)order->m_orderType <<
                 " Currency: " << order->m_currency << std::endl;

    m_orders.append(order);
    if( order->m_currency != m_currency ) return true;

    beginInsertRows( QModelIndex(), m_filteredOrders.size(), m_filteredOrders.size());
    filterOrders( m_filteredOrders, m_currency );
    endInsertRows();
    return ZR::ZR_SUCCESS;
}



OrderBook::Order * OrderBook::remove( Order * order )
{
    for(OrderIterator it = m_orders.begin(); it != m_orders.end(); it++){
        if( *order == *(*it) ){
            m_orders.erase( it );
            beginResetModel();
            filterOrders( m_filteredOrders, m_currency );
            endResetModel();
            return *it;
        }
    }
    return NULL;
}

OrderBook::Order * OrderBook::remove( const std::string & order_id )
{
    for(OrderIterator it = m_orders.begin(); it != m_orders.end(); it++){
        if( order_id == (*it)->m_order_id ){
            m_orders.erase( it );
            beginResetModel();
            filterOrders( m_filteredOrders, m_currency );
            endResetModel();
            return *it;
        }
    }
    return NULL;
}


// Order implementation

void OrderBook::Order::setOrderId()
{
    unsigned char md[SHA256_DIGEST_LENGTH];
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    std::ostringstream dataStream;
    dataStream << p3zr->getOwnId()  << m_timeStamp << rand() << m_currency << m_orderType;
    std::string data = dataStream.str();
    if(!SHA256((unsigned char *)data.c_str(), data.length(), md))
    {
        std::cerr << "Zero Reserve: cannot create a hash for the Order ID" << std::endl;
    }
}


bool OrderBook::Order::operator == ( const OrderBook::Order & other )
{
    if(m_order_id == other.m_order_id )
        return true;
    else
        return false;
}
