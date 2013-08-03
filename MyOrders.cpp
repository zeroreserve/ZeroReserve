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

#include <iostream>


MyOrders::MyOrders()
{}

MyOrders::MyOrders( OrderBook * bids, OrderBook * asks ) :
    m_bids( bids ),
    m_asks( asks )
{
    m_bids->setMyOrders( this );
    m_asks->setMyOrders( this );
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
            return QVariant(order->m_amount);
        case 2:
            return QVariant(order->m_price);
        default:
            return QVariant();
        }
    }
    return QVariant();
}


void MyOrders::match( Order * order )
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    if( order->m_orderType == Order::ASK ){  // compare with bids
        OrderList bids;
        m_bids->filterOrders( bids, order->m_currency );
        for( OrderIterator bidIt = bids.begin(); bidIt != bids.end(); bidIt++ ){
            if( (*bidIt)->m_trader_id == p3zr->getOwnId() ) continue; // don't fill own orders
            if( order->m_price_d > (*bidIt)->m_price_d ) break;    // no need to try and find matches beyond
            std::cerr << "Zero Reserve: Match at ask price " << (*bidIt)->m_price.toStdString() << std::endl;
        }
    }
    else {
        OrderList asks;
        m_asks->filterOrders( asks, order->m_currency );
        for( OrderIterator askIt = asks.begin(); askIt != asks.end(); askIt++ ){
            if( (*askIt)->m_trader_id == p3zr->getOwnId() ) continue; // don't fill own orders
            if( order->m_price_d < (*askIt)->m_price_d ) break;    // no need to try and find matches beyond
            std::cerr << "Zero Reserve: Match at ask price " << (*askIt)->m_price.toStdString() << std::endl;
        }
    }
}

bool MyOrders::addOrder( Order * order )
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    if( order->m_trader_id != p3zr->getOwnId() ) return true;

    match( order );

    return OrderBook::addOrder( order );
}
