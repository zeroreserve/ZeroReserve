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

#ifndef BITCOINADDRESSLIST_H
#define BITCOINADDRESSLIST_H

#include "ZRBitcoin.h"

#include <QAbstractItemModel>

#include <QList>

/// @classBitcoinAddressList
//
class BitcoinAddressList : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit BitcoinAddressList(QObject *parent = 0);

    virtual QModelIndex index(int, int, const QModelIndex&) const;
    virtual QModelIndex parent(const QModelIndex&) const;
    virtual int rowCount(const QModelIndex&) const;
    virtual int columnCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void addWallet( ZR::Wallet * wallet );
    
signals:
    
public slots:


private:
    QList< ZR::Wallet* > m_walletList;
    
};

#endif // BITCOINADDRESSLIST_H
