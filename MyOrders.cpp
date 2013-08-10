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
#include "TransactionManager.h"

#include <iostream>
#include <sstream>

MyOrders * MyOrders::me = NULL;

MyOrders * MyOrders::Instance(){ return me; }


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
            return QVariant(order->m_amount);
        case 2:
            return QVariant(order->m_price);
        default:
            return QVariant();
        }
    }
    return QVariant();
}


int MyOrders::match( Order * order )
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    if( order->m_orderType == Order::ASK ){  // we are selling - compare with the bids
        OrderList bids;
        m_bids->filterOrders( bids, order->m_currency );
        for( OrderIterator bidIt = bids.begin(); bidIt != bids.end(); bidIt++ ){
            Order * other = *bidIt;
            if( other->m_trader_id == p3zr->getOwnId() ) continue; // don't fill own orders
            if( order->m_price_d > other->m_price_d ) break;    // no need to try and find matches beyond
            std::cerr << "Zero Reserve: Match at ask price " << other->m_price.toStdString() << std::endl;
            bool ok;
            if( order->m_amount.toDouble( &ok ) > other->m_amount.toDouble( &ok ) ){
               sell( other, other->m_amount );
            }
            else {
                sell( other, order->m_amount );
                return ZR::ZR_SUCCESS;
            }
        }
    }
    else {
        OrderList asks;
        m_asks->filterOrders( asks, order->m_currency );
        for( OrderIterator askIt = asks.begin(); askIt != asks.end(); askIt++ ){
            Order * other = *askIt;
            if( other->m_trader_id == p3zr->getOwnId() ) continue; // don't fill own orders
            if( order->m_price_d < other->m_price_d ) break;    // no need to try and find matches beyond
            std::cerr << "Zero Reserve: Match at ask price " << other->m_price.toStdString() << std::endl;
            bool ok;
            ZR::ZR_Number myAmount = order->m_amount.toDouble( &ok );
            ZR::ZR_Number otherAmount = other->m_amount.toDouble( &ok );
            if( myAmount > otherAmount ){
                buy( other, other->m_amount );
                order->m_amount = QString::number( myAmount - otherAmount );
            }
            else {
                buy( other, order->m_amount );
                order->m_amount = "";
                return ZR::ZR_FINISH;
            }
        }
    }
    return ZR::ZR_SUCCESS;
}


void MyOrders::buy( Order * order, QString amount )
{
    TransactionManager * tm = new TransactionManager();
    std::ostringstream s_amountToPay;
    s_amountToPay << amount.toDouble() * order->m_price_d;

    Payment * payment = new PaymentSpender( order->m_trader_id, s_amountToPay.str(), Currency::currencySymbols[ order->m_currency ], Payment::BITCOIN );
    std::ostringstream timestamp;
    timestamp << order->m_timeStamp;
    payment->setText( timestamp.str() );
    if( ZR::ZR_FAILURE == tm->initCoordinator( payment ) ) delete tm;
}


void MyOrders::sell( Order * order, QString amount )
{

}


int MyOrders::startExecute()
{
    return ZR::ZR_SUCCESS;
}


int MyOrders::finishExecute( Payment * payment )
{
    std::cerr << "Zero Reserve: Finishing Order execution for " << payment->getText() << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    Order order;
    std::stringstream s_timestamp( payment->getText() );
    s_timestamp >> order.m_timeStamp;
    order.m_trader_id = p3zr->getOwnId();
    order.m_currency = Currency::getCurrencyBySymbol( payment->getCurrency() );
    remove( &order );
    Order * oldOrder = m_asks->remove( &order );
    // TODO: publish delete order and possibly updated order
    if( NULL != oldOrder ){
        bool ok;
        ZR::ZR_Number payAmount = atof( payment->getAmount().c_str() );
        ZR::ZR_Number bitcoinAmount = oldOrder->m_amount.toDouble( &ok);
        if( bitcoinAmount * oldOrder->m_price_d > payAmount ){ // order only partly filled
            oldOrder->m_purpose = Order::PARTLY_FILLED;
            std::ostringstream newAmount;
            newAmount << bitcoinAmount - payAmount / oldOrder->m_price_d;
            oldOrder->m_amount = QString::fromStdString( newAmount.str() );
        }
        else {  // completely filled
            oldOrder->m_purpose = Order::FILLED;
        }
        p3zr->publishOrder( oldOrder );
    }
    else {
        std::cerr << "Zero Reserve: Could not find order" << std::endl;
    }
    return ZR::ZR_SUCCESS;
}

