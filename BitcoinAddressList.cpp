/**
 *  @file BitcoinAddressList.cpp
 *  @author
 *  @date
 *  
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

/// 
/// 
/// @brief Constructor
///
/// @param parent
BitcoinAddressList::BitcoinAddressList(QObject *parent) :
    QAbstractItemModel(parent)
{
}


///
/// @brief Index address list
///
/// @param x
/// @param y
/// @param QModelIndex
///
/// @return createIndex(x, y)
/// @retval 
QModelIndex BitcoinAddressList::index(int x, int y, const QModelIndex&) const
{
    return createIndex(x, y);
}


/// 
/// @brief Parent
///
/// @param QModelIndex
///
/// @return QModelIndex()
//  @retval 
QModelIndex BitcoinAddressList::parent(const QModelIndex&) const
{
    return QModelIndex();
}

/// 
/// @brief Header data
///
/// @param section
/// @param orientation
/// @param role
///
/// @return Address
/// @return Nick
/// @return QVariant()
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
            }
        }
    }
    return QVariant();
}


///
/// @brief Column count
///
/// @param QModelIndex
///
/// @return 2
int BitcoinAddressList::columnCount(const QModelIndex&) const
{
    return 2;
}

//
/// @brief Row count
///
/// @param QModelIndex
///
/// @return  m_walletList.size()
int BitcoinAddressList::rowCount(const QModelIndex&) const
{
    return m_walletList.size();
}


/
/// @brief Data
///
/// @param index
/// @param role
///
/// @return Address
/// @return Nick
/// @return QVariant()
QVariant BitcoinAddressList::data( const QModelIndex& index, int role ) const
{
    if (role == Qt::DisplayRole && index.row() < m_walletList.size()){
        ZR::Wallet * wallet = m_walletList[index.row()];
        switch(index.column()){
            case 0:
                return QVariant( QString::fromStdString( wallet->getAddress() ) );
            case 1:
                return QVariant( QString::fromStdString( wallet->getNick() ) );
            default:
                return QVariant();
        }

    }
    return QVariant();
}


/// @brief Add wallet
///
/// @param wallet
void BitcoinAddressList::addWallet( ZR::Wallet * wallet )
{
    beginResetModel();
    m_walletList.push_back( wallet );
    endResetModel();
}

// EOF  
