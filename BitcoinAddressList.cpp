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


#include "BitcoinAddressList.h"

BitcoinAddressList::BitcoinAddressList(QObject *parent) :
    QAbstractItemModel(parent)
{
    loadWallets();
}


QModelIndex BitcoinAddressList::index(int x, int y, const QModelIndex&) const
{
    return createIndex(x, y);
}

QModelIndex BitcoinAddressList::parent(const QModelIndex&) const
{
    return QModelIndex();
}

QVariant BitcoinAddressList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Address");
            case 1:
                return QString("Nick");
            case 2:
                return QString("Balance");
            }
        }
    }
    return QVariant();
}


int BitcoinAddressList::columnCount(const QModelIndex&) const
{
    return 3;
}

int BitcoinAddressList::rowCount(const QModelIndex&) const
{
    return m_walletList.size();
}


QVariant BitcoinAddressList::data( const QModelIndex& index, int role ) const
{
    if (role == Qt::DisplayRole && index.row() < m_walletList.size()){
        ZR::Wallet * wallet = m_walletList[index.row()];
        switch(index.column()){
            case 0:
                return QVariant( QString::fromStdString( wallet->getAddress() ) );
            case 1:
                return QVariant( QString::fromStdString( wallet->getNick() ) );
            case 2:
                return QVariant( wallet->getBalance().toDecimalQString() );
            default:
                return QVariant();
        }

    }
    return QVariant();
}

void BitcoinAddressList::addWallet( ZR::Wallet * wallet )
{
    beginResetModel();
    m_walletList.push_back( wallet );
    endResetModel();
}


void BitcoinAddressList::loadWallets()
{
    std::vector< ZR::MyWallet *> wallets;
    ZR::Bitcoin::Instance()->loadWallets( wallets );
    beginResetModel();
    for( std::vector< ZR::MyWallet *>::const_iterator it = wallets.begin(); it != wallets.end(); it++ ){
        m_walletList.push_back( *it );
    }
    endResetModel();
}


