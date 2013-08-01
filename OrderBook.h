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

#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "zrtypes.h"
#include "Currency.h"

#include <QAbstractItemModel>
#include <QDateTime>
#include <QList>



/**
  An object of this class is either the bid or the ask side of the complete order book.
  */

class OrderBook : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
      A single order
      */
    class Order
    {
    public:
        enum OrderType { BID = 0, ASK };

        std::string m_trader_id;
        OrderType m_orderType;
        QString m_amount;
        QString m_price;      // a string of the form "77.123" - any length. For printing
        ZR_Number m_price_d;     // the amount as number for sorting
        Currency::CurrencySymbols m_currency;
        time_t m_timeStamp;   // no more than 1 order / second
        bool sent;            //

        bool setPrice( QString price );
        bool operator == (const Order & other);
    };

public:
    explicit OrderBook();
    virtual ~OrderBook();

    virtual QModelIndex index(int, int, const QModelIndex&) const;
    virtual QModelIndex parent(const QModelIndex&) const;
    virtual int rowCount(const QModelIndex&) const;
    virtual int columnCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool addOrder( Order* order );


protected:
    typedef QList<Order*>::iterator OrderIterator;

    QList < Order* > m_orders;
    QList < Order* > m_filteredOrders;
    Currency::CurrencySymbols m_currency;

signals:

public slots:
    void setCurrency( const QString & currency );

private:
    static bool compareOrder( const Order * left, const Order * right );

    void filterOrders();
};

#endif // ORDERBOOK_H
