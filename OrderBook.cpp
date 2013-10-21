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
#include "zrdb.h"

#include "util/radix64.h"

#include <openssl/sha.h>
#include <iostream>

#ifdef ZR_TESTNET
const qint64 OrderBook::Order::timeout = 1800000;   // 30 minutes
#else
const qint64 OrderBook::Order::timeout = 86400000;  // one day
#endif

/// @brief Constructor
//
OrderBook::OrderBook() :
    m_order_mutex("order_mutex")
{
    m_myOrders = NULL;
}

/// @brief Destructor
OrderBook::~OrderBook()
{
    for( OrderIterator it = m_orders.begin(); it != m_orders.end(); it++) 
	    delete *it;
}

/// @brief Index creation for order book
///
/// @param x
/// @param y
/// @param QModelIndex
///
/// @return 
QModelIndex OrderBook::index(int x, int y, const QModelIndex&) const
{
    return createIndex(x, y);
}

/// @brief Orderbook parent
///
/// @param QModelIndex
///
/// @return 
QModelIndex OrderBook::parent(const QModelIndex&) const
{
    return QModelIndex();
}

/// @brief Count rows in orderbook
///
/// @param QModelIndex
///
/// @return 
int OrderBook::rowCount(const QModelIndex&) const
{
    RsStackMutex orderMutex( m_order_mutex );
    return m_filteredOrders.size();
}

/// @brief Column count for order book
///
/// @param QModelIndex
///
/// @return 2
int OrderBook::columnCount(const QModelIndex&) const
{
    return 2;
}


/// @brief Display orderbook data
///
/// @param index
/// @param role
///
/// @return 
QVariant OrderBook::data( const QModelIndex& index, int role ) const
{
    RsStackMutex orderMutex( m_order_mutex );

    if (role == Qt::DisplayRole && index.row() < m_filteredOrders.size())
    {
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

/// @brief Header data for orderbook
///
/// @param section
/// @param orientation
/// @param role
///
/// @return 
QVariant OrderBook::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) 
        {
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

/// @brief Set currency in the order book
///
/// @param currency
void OrderBook::setCurrency( const QString & currency )
{
    m_currency = Currency::getCurrencyByName( currency.toStdString() );
    beginResetModel();
    filterOrders( m_filteredOrders, m_currency );
    endResetModel();
}

/// @brief Filter orders
///
/// @param filteredOrders
/// @param currencySym
void OrderBook::filterOrders( OrderList & filteredOrders, const Currency::CurrencySymbols currencySym )
{
    RsStackMutex orderMutex( m_order_mutex );
    filteredOrders.clear();

    for(OrderIterator it = m_orders.begin(); it != m_orders.end(); it++)
    {
        if( (*it)->m_currency == currencySym )
            filteredOrders.append( *it );
    }
    qSort( filteredOrders.begin(), filteredOrders.end(), compareOrder );
}

/// @brief Process my order
///
/// @todo Some code needs to be re-enabled - refer to source code for 
//  more information.
//  
/// @param order
///
/// @return 
ZR::RetVal OrderBook::processMyOrder( Order* order )
{
    ZR::RetVal retval;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );

    try {
        ZrDB::Instance()->addOrder( order );
    }
    catch( std::runtime_error & e ){
        std::cerr << "Zero Reserve: ERROR:" << e.what() << std::endl;
        return ZR::ZR_FAILURE;
    }

    m_myOrders->addOrder( order );
    addOrder( order );

    if( order->m_orderType == Order::BID )
    {
        retval = m_myOrders->match( order );
    }
    else 
    {
        // TODO: Re-enable this:
        // m_myOrders->matchAsk( order );
        // addOrder( order );
        // TODO: Add a publisher queue for the case that counterparty doesn't respond
        retval = ZR::ZR_SUCCESS;
    }
    if( ZR::ZR_SUCCESS == retval )
    {
        p3zr->publishOrder( order );
    }
    return ZR::ZR_SUCCESS;
}

/// @brief Process order from orderbook
///
/// @param order
///
/// @return 
ZR::RetVal OrderBook::processOrder( Order* order )
{
    if( m_myOrders->find( order->m_order_id ) != m_myOrders->end() )
        return ZR::ZR_FAILURE; // this is my own order

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    if( currentTime - order->m_timeStamp >  Order::timeout
            || order->m_timeStamp - currentTime > 3600000)
    {  // or more than an hour in the future
        return ZR::ZR_FAILURE;
    }

    if( Order::FILLED == order->m_purpose || Order::CANCEL == order->m_purpose )
    {
        Order * oldOrder = remove( order );
        if( oldOrder )
        {
            delete oldOrder;
            return ZR::ZR_SUCCESS;
        }
        return ZR::ZR_FINISH;
    }

    if( Order::PARTLY_FILLED == order->m_purpose )
    {
        for(OrderIterator it = m_orders.begin(); it != m_orders.end(); it++)
        {
            if( *order == *(*it) )
            {
                if( order->m_amount == (*it)->m_amount )
                    return ZR::ZR_FINISH; // we have it already - do nothing
            }
        }
        remove( order );  // remove so it gets reinserted with the updates values below.
        addOrder( order );
        return ZR::ZR_SUCCESS;
    }

    if( find( order->m_order_id ) != end() )
        return ZR::ZR_FINISH; // order already in book

    addOrder( order );
    return m_myOrders->matchOther( order );
}

/// @brief Add order for a currency
///
/// @param order
///
/// @return 
int OrderBook::addOrder( Order * order )
{    
    std::cerr << "Zero Reserve: Inserting Type: " << (int)order->m_orderType <<
                 " Currency: " << order->m_currency << std::endl;
    {
        RsStackMutex orderMutex( m_order_mutex );
        m_orders.append(order);
    }
    if( order->m_currency != m_currency ) return ZR::ZR_SUCCESS;

    beginInsertRows( QModelIndex(), m_filteredOrders.size(), m_filteredOrders.size());
    filterOrders( m_filteredOrders, m_currency );
    endInsertRows();
    return ZR::ZR_SUCCESS;
}

/// @brief Remove an order from the order book
///
/// @param order
///
/// @return 
OrderBook::Order * OrderBook::remove( Order * order )
{

    OrderIterator it = find( order->m_order_id );
    if( it != m_orders.end() )
    {
        {
            RsStackMutex orderMutex( m_order_mutex );
            m_orders.erase( it );
        }
        ZrDB::Instance()->deleteOrder( order);
        beginResetModel();
        filterOrders( m_filteredOrders, m_currency );
        endResetModel();
        return *it;
    }
    return NULL;
}

/// @brief Remove
///
/// @todo Boilerplate - see code for more information
//
/// @param order_id
///
/// @return 
OrderBook::Order * OrderBook::remove( const std::string & order_id )
{
    // FIXME: Boilerplate
    OrderIterator it = find( order_id );
    if( it != m_orders.end() )
    {
        {
            RsStackMutex orderMutex( m_order_mutex );
            m_orders.erase( it );
        }
        beginResetModel();
        filterOrders( m_filteredOrders, m_currency );
        endResetModel();
        return *it;
    }
    return NULL;
}

/// @brief Find a give order id
///
/// @param order_id
///
/// @return 
OrderBook::OrderIterator OrderBook::find( const std::string & order_id )
{
    RsStackMutex orderMutex( m_order_mutex );
    for( OrderIterator it = m_orders.begin(); it != m_orders.end(); it++ )
    {
        if( order_id == (*it)->m_order_id )
        {
            return it;
        }
    }
    return m_orders.end();
}

/// @brief Compare order
///
/// @param left
/// @param right
///
/// @return 
bool OrderBook::compareOrder( const Order * left, const Order * right ){
    return ( left->m_orderType == Order::BID ) ? left->m_price > right->m_price : left->m_price < right->m_price;
}


/// @brief Order implementation
///
/// @todo Some fixme's in the code
//  The getownid() must be replaced by a secret otherwise the order originator can be calculated
//  by frieds and friends of friends
//  
void OrderBook::Order::setOrderId()
{
    unsigned char md[ SHA256_DIGEST_LENGTH ];
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    std::ostringstream dataStream;
    // FIXME: the getOwnId() must be replaced by a secret - else the originator of the order can be calculated
    // FIXME: by friends and friends of friends
    dataStream << p3zr->getOwnId() << m_timeStamp << m_currency << m_orderType << m_amount << m_price;
    std::string data = dataStream.str();
    int length = data.length();
    unsigned char * buff = new unsigned char[ length ];
    memcpy( buff, data.c_str(), length );
    SHA256( buff, length, md);
    Radix64::encode( (const char*)md, SHA256_DIGEST_LENGTH, m_order_id );
    delete [] buff;
}


/// @brief Overloaded == operator
///
/// @param other
///
/// @return 
bool OrderBook::Order::operator == ( const OrderBook::Order & other )
{
    if( m_order_id == other.m_order_id )
        return true;
    else
        return false;
}


/// @brief Overloaded < operator
///
/// @param other
///
/// @return 
bool OrderBook::Order::operator < ( const Order & other ) const
{
    return ( m_order_id.compare( other.m_order_id ) < 0 );
}

/// @brief Time out orders
void OrderBook::timeoutOrders()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    OrderList ordersCopy( m_orders );
    for( OrderIterator it = ordersCopy.begin(); it != ordersCopy.end(); it++ )
    {
        Order * order = *it;
        if( currentTime - order->m_timeStamp > Order::timeout )
        {
            if( order->m_commitment == 0 )
            {
                remove( order );
                if( order->m_isMyOrder )
                {
                    m_myOrders->remove( order );
                }
                delete order;
            }
        }
    }
}

// EOF   
