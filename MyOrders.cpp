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

#include "MyOrders.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"
#include "Payment.h"
#include "TmContract.h"
#include "ZRBitcoin.h"
#include "Router.h"
#include "zrdb.h"

#include <iostream>
#include <sstream>

MyOrders * MyOrders::me = NULL;

MyOrders * MyOrders::Instance()
{
    while(!me);
    return me;
}

MyOrders::MyOrders()
{
    me = this;
}

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

int MyOrders::columnCount(const QModelIndex&) const
{
    return 3;
}

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

bool MyOrders::reverseCompareOrder( const Order * left, const Order * right ){
    return ( left->m_orderType == Order::BID ) ? left->m_price > right->m_price : left->m_price < right->m_price;
}

void MyOrders::filterBids( OrderList & filteredOrders, const Currency::CurrencySymbols currencySym )
{
    RsStackMutex orderMutex( m_order_mutex );
    filteredOrders.clear();
    for(OrderIterator it = m_orders.begin(); it != m_orders.end(); it++){
        Order * order = *it;
        if( order->m_currency == currencySym && order->m_orderType == Order::BID )
            filteredOrders.append( *it );
    }
    qSort( filteredOrders.begin(), filteredOrders.end(), reverseCompareOrder );
}

ZR::RetVal MyOrders::matchOther( Order * other )
{
    if( other->m_isMyOrder ) return ZR::ZR_FAILURE; // don't fill own orders
    if( other->m_orderType == Order::BID ){
        return ZR::ZR_SUCCESS;
    }

    Order * order = NULL;
    OrderList bids;
    filterBids( bids, other->m_currency );
    ZR::ZR_Number amount = other->m_amount;

    for( OrderIterator it = bids.begin(); it != bids.end(); it++ ){
        order = *it;
        if( order->m_price < other->m_price ) break;    // no need to try and find matches beyond
        std::cerr << "Zero Reserve: Match at ask price " << order->m_price.toStdString() << std::endl;

        if( order->m_amount > amount ){
            buy( other, order, amount );
            return ZR::ZR_SUCCESS;
        }
        else {
            buy( other, order, order->m_amount );
        }
        amount -= order->m_amount;
        if( amount == 0 )break;
    }
    return ZR::ZR_SUCCESS;
}


ZR::RetVal MyOrders::match( Order * myOrder )
{
    OrderList asks;
    m_asks->filterOrders( asks, myOrder->m_currency );
    ZR::ZR_Number amount = myOrder->m_amount;
    for( OrderIterator askIt = asks.begin(); askIt != asks.end(); askIt++ ){
        Order * other = *askIt;
        if( other->m_isMyOrder ) continue; // don't fill own orders
        if( myOrder->m_price < other->m_price ) break;    // no need to try and find matches beyond
        std::cerr << "Zero Reserve: Match at ask price " << other->m_price.toStdString() << std::endl;

        if( amount > other->m_amount ){
            buy( other, myOrder, other->m_amount );
        }
        else {
            buy( other, myOrder, amount );
            return ZR::ZR_FINISH;
        }
        amount -= other->m_amount;
    }
    return ZR::ZR_SUCCESS;
}


void MyOrders::buy( Order * other, Order * myOrder, const ZR::ZR_Number amount )
{
    TmContractCoordinator * tm = new TmContractCoordinator( other, myOrder, amount );
    if( ZR::ZR_FAILURE == tm->init() ) delete tm;
}



void MyOrders::cancelOrder( int index )
{
    std::cerr << "Zero Reserve: Cancelling order: " << index << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    Order * order = m_filteredOrders[ index ];

    remove( order->m_order_id );
    if( order->m_orderType == Order::ASK ){
        m_asks->remove( order->m_order_id );
    }
    else{
        m_bids->remove( order->m_order_id );
    }

    order->m_purpose = Order::CANCEL;
    p3zr->publishOrder( order );
}

