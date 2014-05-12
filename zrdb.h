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

#ifndef ZRDB_H
#define ZRDB_H

#include "zrtypes.h"
#include "Credit.h"
#include "OrderBook.h"

#include "util/rsthreads.h"

#include <sqlite3.h>
#include <QDateTime>

#include <string>
#include <vector>

#include <stdlib.h>


class BtcContract;

/**
  Database class to save and load friend data and payment info. Uses sqlite3
  */

class ZrDB
{
    ZrDB();
    void init();
public:

    typedef struct {
        std::string currency;
        ZR::ZR_Number our_credit;  // credit with all peers
        ZR::ZR_Number credit;      // their credit with us
        ZR::ZR_Number outstanding; // their debt with us
        ZR::ZR_Number debt;        // our debt with them
        ZR::ZR_Number balance;
    } GrandTotal;

    typedef struct {
         QString id;
         QString currency;
         ZR::ZR_Number m_amount;
         QDateTime timestamp;
    } TxLogItem;

    typedef struct {
         ZR::WalletSecret secret;
         std::string nick;
         int type;
    } MyWallet;

    static ZrDB * Instance();
    void createPeerRecord( const Credit & peer_in );
    void deletePeerRecord( const std::string & uid, const Currency::CurrencySymbols & sym );
    void updatePeerCredit(const Credit & peer_in, const std::string & column, ZR::ZR_Number &value );
    void loadPeer( Credit & peer_out );
    void loadPeer( const std::string & id, Credit::CreditList & peer_out );
    bool peerExists( const Credit & peer_in );

    GrandTotal &loadGrandTotal( const std::string & currency );

    std::string getConfig( const std::string & key );
    void updateConfig( const std::string & key, const std::string & value );

    void addOrder( OrderBook::Order * order );
    void loadOrders(OrderBook::OrderList *orders_out );
    void updateOrder( OrderBook::Order * order );
    void deleteOrder( OrderBook::Order * order );
    void addToOrderList( OrderBook::Order * order );

    void close();

    void peerRecordExists(){ m_peer_record_exists = true; }
    void setPeerCredit( const std::string & credit, const std::string & our_credit, const std::string & balance , const std::string & allocation );
    void addPeerCredit( Credit * credit );
    void setConfigValue( const std::string & val ) { m_config_value = val; }
    void addToGrandTotal( char ** cols );
    void addToTxList( const TxLogItem & item );

    void openTxLog();
    void closeTxLog();
    void appendTx(const std::string & id, const std::string &currency, ZR::ZR_Number amount );
    void loadTxLog(std::list< TxLogItem > & txList );

    void beginTx();
    void commitTx();
    void rollbackTx();


    // TODO void logPayment() const;
    // TODO: void replayPaymentLog();
    // TODO: void backup() const;
    // TODO: void restore() const;

////////// Bitcoin Wallet //////////////
    ZR::RetVal storeMyWallet( const ZR::WalletSecret &secret, unsigned int type, const std::string &nick );
    ZR::RetVal addPeerWallet( const ZR::BitcoinAddress & address, const std::string & nick );
    void loadMyWallets( std::vector< MyWallet > & wallets );
    void addMyWallet( MyWallet & wallet );

    void addBtcContract( BtcContract * contract );
    void rmBtcContract(const ZR::TransactionId & btcTxId , int party );
    void loadBtcContracts();

private:
    void setConfig( const std::string & key, const std::string & value );
    void runQuery( const std::string & sql );


private:
    RsMutex m_peer_mutex;
    RsMutex m_config_mutex;
    RsMutex m_tx_mutex;

    sqlite3 *m_db;
    sqlite3 *m_txLog;

    // buffers for the callbacks
    bool m_peer_record_exists;
    std::string m_config_value;
    GrandTotal grandTotal;    
    Credit * m_credit;
    Credit::CreditList * m_creditList;
    std::list< TxLogItem > * m_txList;
    OrderBook::OrderList * m_orderList;
    std::vector< MyWallet > * m_wallets;

    static ZrDB * instance;
    static RsMutex creation_mutex;

public: // config parameters
    static const char * const TXLOGPATH;
    static const char * const DB_VERSION;       // integer
    static const char * const MINIMUM_FEE;
    static const char * const PERCENTAGE_FEE;
};

#endif // ZRDB_H
