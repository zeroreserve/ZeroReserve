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

#include <QAbstractItemModel>
#include <QDateTime>
#include <QList>

#include "Currency.h"


class OrderBook : public QAbstractItemModel
{
    Q_OBJECT

public:
    class Order
    {
    public:
        enum OrderType { BID = 0, ASK };

        std::string m_trader_id;
        OrderType m_orderType;
        QString m_amount;
        QString m_price;      // a string of the form "77.123" - any length. For printing
        double m_price_d;     // the amount as a double for sorting
        Currency::CurrencySympols m_currency;
        time_t m_timeStamp;   // no more than 1 order / second
        bool sent;            //

        bool setPrice( QString price );
        void setCurrencyFromName( QString currency );
        void setCurrencyFromSymbol( const std::string & currency );

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

    void addOrder( Order* order );


protected:
    QList < Order* > orders;

signals:

public slots:

private:
    static bool compareOrder( const Order * left, const Order * right );

};

#endif // ORDERBOOK_H
