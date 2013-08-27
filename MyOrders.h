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

    void cancelOrder( int index );

protected:
    /** Matches our new order with all others  */
    virtual int match(Order *order);

    /** Matches incoming new order with ours */
    virtual int matchOther( Order * other );


    /** Buyer side: start buying Bitcoins */
    void buy(Order * order, ZR::ZR_Number amount );


private:
    OrderBook * m_bids;
    OrderBook * m_asks;

private:
    // FIXME: ugly hack - this class is not supposed to be a singleton.
    // but I have no idea how PaymentReceiver can have a reference to this.
    // since there is only ever one object we get away with it for now.
    friend class PaymentReceiver;
    static MyOrders * Instance();
    static MyOrders * me;
};

#endif // MYORDERS_H
