#ifndef BITCOINADDRESSLIST_H
#define BITCOINADDRESSLIST_H

#include "ZRBitcoin.h"

#include <QAbstractItemModel>

#include <list>

class BitcoinAddressList : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit BitcoinAddressList(QObject *parent = 0);
    
signals:
    
public slots:


private:
    std::list< ZR::Wallet* > m_walletList;
    
};

#endif // BITCOINADDRESSLIST_H
