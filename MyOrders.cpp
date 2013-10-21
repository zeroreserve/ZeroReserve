/*!
 * \file MyOrders.cpp
 * 
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

#include "MyOrders.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"
#include "Payment.h"
#include "TmRemoteCoordinator.h"
#include "Router.h"
#include "zrdb.h"

#include <iostream>
#include <sstream>

MyOrders * MyOrders::me = NULL;

/// @brief Create an instance of an order
///
/// @todo Need expert to explain what this does
//
/// @return 
MyOrders * MyOrders::Instance()
{
    while(!me);
    return me;
}

/// @brief
//
/// @todo Need expert to document what this does
MyOrders::MyOrders()
{
    me = this;
}

/// @brief Myorders
///
/// @param bids
/// @param asks
MyOrders::MyOrders( OrderBook * bids, OrderBook * asks ) :
    m_bids( bids ),
    m_asks( asks )
{
    m_bids->setMyOrders( this );
    m_asks->setMyOrders( this );

    try{
        OrderList myorders;
        ZrDB::Instance()->loadOrders( &myorders );
        for( OrderIterator it = myorders.begin(); it != myorders.end(); it++){
            Order * order = *it;
            addOrder( order );
            if( order->m_orderType == Order::ASK ){
                m_asks->addOrder( order );
            }
            else{
                m_bids->addOrder( order );
            }
        }
    }
    catch( std::runtime_error & e ){
        std::cerr << "Zero Reserve: ERROR: " << e.what() << std::endl;
    }

    me = this;
}

/// @brief Column count
///
/// @param QModelIndex
///
/// @return 3
int MyOrders::columnCount(const QModelIndex&) const
{
    return 3;
}

/// @brief Header data
///
/// @param section
/// @param orientation
/// @param role
///
/// @return 
QVariant MyOrders::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Type");
            case 1:
                return QString("Volume");
            case 2:
                return QString("Price");
            }
        }
    }
    return QVariant();
}


/// @brief Data display
///
/// @param index
/// @param role
///
/// @return 
QVariant MyOrders::data( const QModelIndex& index, int role ) const
{
    if (role == Qt::DisplayRole && index.row() < m_filteredOrders.size()){
        Order * order = m_filteredOrders[index.row()];
        switch(index.column())
        {
        case 0:
            if(order->m_orderType == Order::ASK )
                return QVariant( "Sell");
            else
                return QVariant( "Buy");
        case 1:
            return QVariant( order->m_amount.toDouble() );
        case 2:
            return QVariant( order->m_price.toDouble() );
        default:
            return QVariant();
        }
    }
    return QVariant();
}

/// @brief Reverse compare order
///
/// @param left
/// @param right
///
/// @return 
bool MyOrders::reverseCompareOrder( const Order * left, const Order * right )
{
    return ( left->m_orderType == Order::BID ) ? left->m_price > right->m_price : left->m_price < right->m_price;
}

/// @brief Filter bids
///
/// @param filteredOrders
/// @param currencySym
void MyOrders::filterBids( OrderList & filteredOrders, const Currency::CurrencySymbols currencySym )
{
    filteredOrders.clear();

    for(OrderIterator it = m_orders.begin(); it != m_orders.end(); it++)
    {
        Order * order = *it;
        if( order->m_currency == currencySym && order->m_orderType == Order::BID )
            filteredOrders.append( *it );
    }
    qSort( filteredOrders.begin(), filteredOrders.end(), reverseCompareOrder );
}

/// @brief Match other
///
/// @param other
///
/// @return 
ZR::RetVal MyOrders::matchOther( Order * other )
{
    if( other->m_isMyOrder ) return ZR::ZR_FAILURE; // don't fill own orders

    if( other->m_orderType == Order::BID )
    {
        return ZR::ZR_SUCCESS;
    }

    Order * order = NULL;
    OrderList bids;
    filterBids( bids, other->m_currency );
    ZR::ZR_Number amount = other->m_amount;
    for( OrderIterator it = bids.begin(); it != bids.end(); it++ )
    {
        order = *it;
        if( order->m_price < other->m_price ) break;    // no need to try and find matches beyond
        std::cerr << "Zero Reserve: Match at ask price " << order->m_price.toStdString() << std::endl;

        m_CurrentTxOrders[ other->m_order_id + ':' + order->m_order_id ] = std::pair< Order, Order > ( *order, *other ); // remember the matched order pair for later

        if( order->m_amount > amount )
        {
            buy( other, amount * other->m_price, order->m_order_id );
            return ZR::ZR_FINISH;
        }
        else 
        {
            buy( other, order->m_amount * other->m_price, order->m_order_id );
        }
        amount -= order->m_amount;
    }
    return ZR::ZR_SUCCESS;
}



/// @brief Match asking
///
/// @param order
///
/// @return 
ZR::RetVal MyOrders::matchAsk( Order * order )
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );

    ZR::ZR_Number amountBtc = order->m_amount;
    OrderList bids;
    m_bids->filterOrders( bids, order->m_currency );
    for( OrderIterator bidIt = bids.begin(); bidIt != bids.end(); bidIt++ )
    {
        Order * other = *bidIt;
        if( other->m_isMyOrder ) continue; // don't fill own orders
        if( order->m_price > other->m_price ) break;    // no need to try and find matches beyond
        std::cerr << "Zero Reserve: Match at bid price " << other->m_price.toStdString() << std::endl;
        if( amountBtc > other->m_amount )
        {
            p3zr->sendBuyMsg( order->m_order_id, other->m_order_id, other->m_amount );
        }
        else 
        {
            p3zr->sendBuyMsg( order->m_order_id, other->m_order_id, amountBtc );
            return ZR::ZR_SUCCESS;
        }
    }
    return ZR::ZR_SUCCESS;
}

/// @brief Myorders match
///
/// @param order
///
/// @return 
ZR::RetVal MyOrders::match( Order * order )
{
    OrderList asks;
    m_asks->filterOrders( asks, order->m_currency );
    ZR::ZR_Number amount = order->m_amount;
    for( OrderIterator askIt = asks.begin(); askIt != asks.end(); askIt++ )
    {
        Order * other = *askIt;
        if( other->m_isMyOrder ) continue; // don't fill own orders
        if( order->m_price < other->m_price ) break;    // no need to try and find matches beyond
        std::cerr << "Zero Reserve: Match at ask price " << other->m_price.toStdString() << std::endl;

        m_CurrentTxOrders[ other->m_order_id + ':' + order->m_order_id ] = std::pair< Order, Order > ( *order, *other ); // remember the matched order pair for later

        if( amount > other->m_amount )
        {
            buy( other, other->m_amount * other->m_price, order->m_order_id );
        }
        else 
        {
            buy( other, amount * other->m_price, order->m_order_id );
            return ZR::ZR_FINISH;
        }
        amount -= other->m_amount;
    }
    return ZR::ZR_SUCCESS;
}

/// @brief Buy
///
/// @param order
/// @param amount
/// @param myId
void MyOrders::buy( Order * order, ZR::ZR_Number amount, const Order::ID & myId )
{
    Payment * payment = new PaymentSpender( Router::Instance()->nextHop( order->m_order_id), amount, Currency::currencySymbols[ order->m_currency ], Payment::BITCOIN );
    payment->referrerId( order->m_order_id );
    TmRemoteCoordinator * tm = new TmRemoteCoordinator( order->m_order_id, payment, myId );
    if( ZR::ZR_FAILURE == tm->init() ) delete tm;
}

/// @brief Start execution
///
/// @todo Start 2/3 bitcoin transaction in this function.
/// @param payment
///
/// @return 
int MyOrders::startExecute( Payment * payment )
{
    // TODO: start 2/3 Bitcoin TX here
    std::cerr << "Zero Reserve: Starting Order execution for " << payment->referrerId() << std::endl;

    OrderIterator it = find( payment->referrerId() );
    if( it == end() ) return ZR::ZR_FAILURE;   // no such order

    Order * order = *it;
    ZR::ZR_Number leftover = order->m_amount - order->m_commitment;
    if( leftover == 0 )return ZR::ZR_FAILURE; // nothing left in this order

    ZR::ZR_Number btcAmount = payment->getAmount() / order->m_price;
    if( btcAmount > leftover )
    {
        order->m_commitment = order->m_amount;
        payment->setAmount( leftover * order->m_price );
    }
    else 
    {
        order->m_commitment += btcAmount;
    }

    return ZR::ZR_SUCCESS;
}


/// @brief Finish off the execution
///
/// @todo Sign 2/3 bitcoin transaction here
///
/// @param payment
///
/// @return 
int MyOrders::finishExecute( Payment * payment )
{
    // TODO: sign 2/3 Bitcoin TX here
    std::cerr << "Zero Reserve: Finishing Order execution for " << payment->referrerId() << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );

    OrderIterator it = find( payment->referrerId() );
    if( it != end() )
    {
        Order * oldOrder = *it;
        if( oldOrder->m_amount * oldOrder->m_price >  payment->getAmount() )
        { 
	        // order only partly filled
            beginResetModel();
            m_asks->beginReset();
            oldOrder->m_purpose = Order::PARTLY_FILLED;
            ZR::ZR_Number btcAmount = payment->getAmount() / oldOrder->m_price;
            oldOrder->m_amount -= btcAmount;
            oldOrder->m_commitment -= btcAmount;
            ZrDB::Instance()->updateOrder( oldOrder );
            m_asks->endReset();
            endResetModel();
        }
        else 
        {  
	        // completely filled
            oldOrder->m_purpose = Order::FILLED;
            remove( payment->referrerId() );
            m_asks->remove( payment->referrerId() );
        }
        p3zr->publishOrder( oldOrder );
    }
    else 
    {
        std::cerr << "Zero Reserve: Could not find order" << std::endl;
        return ZR::ZR_FAILURE;
    }
    return ZR::ZR_SUCCESS;
}


/// @brief Update orders
///
/// @param payment
/// @param txId
///
/// @return 
ZR::RetVal MyOrders::updateOrders( Payment * payment, const ZR::VirtualAddress & txId )
{
    std::cerr << "Zero Reserve: Updating order" << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );

    std::map< ZR::TransactionId, std::pair< Order, Order > >::iterator refIt = m_CurrentTxOrders.find( txId );
    if( refIt == m_CurrentTxOrders.end() )
    {
        std::cerr << "Zero Reserve: Could not find Reference" << txId << std::endl;
        return ZR::ZR_FAILURE;
    }
    OrderIterator orderIt = find( (*refIt).second.first.m_order_id );
    if( orderIt == end() ) return ZR::ZR_FAILURE;

    Order * order = *orderIt;
    const Order & other = (*refIt).second.second;

    ZR::ZR_Number fiatAmount = order->m_amount * other.m_price;
    if( fiatAmount > payment->getAmount() )
    {
        beginResetModel();
        m_bids->beginReset();
        order->m_amount -= payment->getAmount() / other.m_price;
        order->m_purpose = Order::PARTLY_FILLED;
        ZrDB::Instance()->updateOrder( order );
        m_bids->endReset();
        endResetModel();
    }
    else
    {
        m_bids->remove( order );
        remove( order );
        order->m_purpose = Order::FILLED;
    }
    p3zr->publishOrder( order );

    m_CurrentTxOrders.erase( refIt );

    return ZR::ZR_SUCCESS;
}

/// @brief Cancel order
///
/// @param index
void MyOrders::cancelOrder( int index )
{
    std::cerr << "Zero Reserve: Cancelling order: " << index << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    Order * order = m_filteredOrders[ index ];

    remove( order );
    if( order->m_orderType == Order::ASK )
    {
        m_asks->remove( order );
    }
    else
    {
        m_bids->remove( order );
    }

    order->m_purpose = Order::CANCEL;
    p3zr->publishOrder( order );
}

/// @brief Roll back the transaction for the payment receiver.
///
/// @param payment
void MyOrders::rollback( PaymentReceiver *payment )
{
    OrderIterator it = find( payment->referrerId() );
    if( it != end() )
    {
        Order * oldOrder = *it;
        ZR::ZR_Number btcAmount = payment->getAmount() / oldOrder->m_price;
        oldOrder->m_commitment -= btcAmount;
    }
}

/// @brief Rollback for spender
///
/// @param payment
/// @param txId
void MyOrders::rollback( PaymentSpender *payment, const ZR::VirtualAddress & txId )
{
    std::cerr << "Zero Reserve: Rolling back " << txId << std::endl;
    m_CurrentTxOrders.erase( txId );
}

// EOF   
