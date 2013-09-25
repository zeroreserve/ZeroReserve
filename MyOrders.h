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


#ifndef MYORDERS_H
#define MYORDERS_H

#include "OrderBook.h"
#include "Payment.h"

#include <map>

/**
 * @brief Holds pointers to all orders from myself.
 * Matches oders and controls execution
 */

class MyOrders : public OrderBook
{
    Q_OBJECT
    MyOrders();

public:
    MyOrders(OrderBook *bids, OrderBook *asks);
    virtual int columnCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;


    /** Seller side: start executing the deal - initiate Bitcoin payment */
    int startExecute(Payment *payment);

    /** Seller side: remove Order from the book, if partly filled, publish
     * a new order, finish Bitcoin payment
     */
    int finishExecute( Payment *payment );
    ZR::RetVal updateOrders( Payment * payment , const ZR::VirtualAddress &txId );
    void rollback( PaymentReceiver *payment );
    void rollback( PaymentSpender *payment, const ZR::VirtualAddress & txId );

    void cancelOrder( int index );

    static MyOrders * Instance();

protected:
    /** Matches our new order with all others  */
    virtual ZR::RetVal match(Order *order);
    virtual ZR::RetVal matchAsk(Order *order);

    /** Matches incoming new order with ours */
    virtual ZR::RetVal matchOther( Order * other );


    /** Buyer side: start buying Bitcoins */
    void buy( Order * order, ZR::ZR_Number amount , const Order::ID &myId );

    void filterBids( OrderList & filteredOrders, const Currency::CurrencySymbols currencySym );
    static bool reverseCompareOrder( const Order * left, const Order * right );

private:
    OrderBook * m_bids;
    OrderBook * m_asks;

    std::map< ZR::TransactionId, std::pair< Order, Order > > m_CurrentTxOrders;

private:

    static MyOrders * me;
};

#endif // MYORDERS_H
