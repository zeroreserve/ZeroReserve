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
#include "ZeroReservePlugin.h"

#include <map>

/**
 * @brief Holds pointers to all orders from myself.
 * Matches oders and controls execution
 */

class MyOrders : public OrderBook
{
    Q_OBJECT

    MyOrders(){}

    friend class ZeroReservePlugin;
    MyOrders(OrderBook *bids, OrderBook *asks);

public:
    ZR::RetVal init();
    virtual int columnCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void cancelOrder( int index );

    /** iterate through all my orders and try matching */
    void match();

    OrderBook * getBids(){ return m_bids; }
    OrderBook * getAsks(){ return m_asks; }

    static MyOrders * Instance();

protected:
    /** Matches one of my orders with all "other" others  */
    ZR::RetVal match( Order *myOrder );

    /** Buyer side: start buying Bitcoins */
    void buy( Order * other, Order * myOrder, const ZR::ZR_Number amount );

private:
    OrderBook * m_bids;
    OrderBook * m_asks;


private:

    static MyOrders * me;
};

#endif // MYORDERS_H
