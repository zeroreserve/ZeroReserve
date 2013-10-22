/*!
 * \file zrdb.cpp
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

#include "zrdb.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"

#include "retroshare/rsinit.h"

#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>


const char * const ZrDB::TXLOGPATH        = "TXLOGPATH";
const char * const ZrDB::DB_VERSION       = "DB_VERSION";



ZrDB * ZrDB::instance = 0;
RsMutex ZrDB::creation_mutex("creation_mutex");



/**
 * @brief Orders Callback
 *
 * @param db
 * @param argc
 * @param argv
 * @param 
 *
 * @return 
 */
static int orders_callback(void * db, int argc, char ** argv, char ** )
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    if( argc == 7 )
    {
        OrderBook::Order * order = new OrderBook::Order( true );
        order->m_order_id = argv[0];
        order->m_orderType = OrderBook::Order::OrderType( atoi( argv[1] ) );
        order->m_amount = ZR::ZR_Number::fromDecimalString( std::string( argv[2]) );
        order->m_price = ZR::ZR_Number::fromDecimalString( std::string( argv[3]) );
        order->m_currency = Currency::getCurrencyBySymbol( argv[4] );
        order->m_timeStamp = atoll( argv[5] );
        order->m_purpose = OrderBook::Order::Purpose( atoi( argv[6] ) );

        zrdb->addToOrderList( order );
    }
    else 
    {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

/**
 * @brief Transaction log callback
 *
 * @param db
 * @param argc
 * @param argv
 * @param 
 *
 * @return 
 */
static int txlog_callback(void * db, int argc, char ** argv, char ** )
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    if(argc == 4)
    {
        ZrDB::TxLogItem item;
        item.id = argv[0];
        item.currency = argv[1];
        item.m_amount = ZR::ZR_Number::fromDecimalString( std::string(argv[2]) );
        item.timestamp = QDateTime::fromString( argv[3], "yyyy-MM-dd HH:mm:ss" );

        zrdb->addToTxList( item );
    }
    else 
    {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}


/**
 * @brief NOOP callback
 *
 * @param db
 * @param int
 * @param 
 * @param 
 *
 * @return 
 */
static int noop_callback(void * db, int, char **, char **)
{
    return SQLITE_OK;
}


/**
 * @brief Store peer callback
 *
 * @param db
 * @param int
 * @param 
 * @param 
 *
 * @return 
 */
static int store_peer_callback(void * db, int, char **, char **)
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    zrdb->peerRecordExists();
    return SQLITE_OK;
}

/**
 * @brief Peer credit callback
 *
 * @param db
 * @param argc
 * @param argv
 * @param 
 *
 * @return 
 */
static int peer_credit_callback(void * db, int argc, char ** argv, char ** )
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    if(argc == 3)
    {
        zrdb->setPeerCredit( argv[0], argv[1], argv[2] );
    }
    else 
    {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

/**
 * @brief Peer credits callback
 *
 * @param db
 * @param argc
 * @param argv
 * @param 
 *
 * @return 
 */
static int peer_credits_callback(void * db, int argc, char ** argv, char ** )
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    if(argc == 5)
    {
        Credit * credit = new Credit( argv[0], argv[1]);
        credit->m_credit = ZR::ZR_Number::fromDecimalString( std::string( argv[2] ) );
        credit->m_our_credit = ZR::ZR_Number::fromDecimalString( std::string( argv[3] ) );
        credit->m_balance = ZR::ZR_Number::fromDecimalString( std::string( argv[4] ) );
        zrdb->addPeerCredit( credit );
    }
    else 
    {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

/**
 * @brief Grand total callback
 *
 * @param db
 * @param argc
 * @param argv
 * @param 
 *
 * @return 
 */
static int grandtotal_callback(void * db, int argc, char ** argv, char ** )
{
    if(argc != 3) return SQLITE_ERROR;
    ZrDB * zrdb = static_cast< ZrDB * >( db );

    zrdb->addToGrandTotal( argv );
    return SQLITE_OK;
}

/**
 * @brief Peer configuration callback
 *
 * @param db
 * @param argc
 * @param argv
 * @param 
 *
 * @return 
 */
static int peer_config_callback(void * db, int argc, char ** argv, char ** )
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    if(argc == 1)
    {
        zrdb->setConfigValue( argv[0] );
    }
    else 
    {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

/**
 * @brief Constructor
 */
ZrDB::ZrDB() :
        m_peer_mutex("peer_mutex"),
        m_config_mutex("config_mutex"),
        m_tx_mutex ( "tx_mutex" )
{

}

/**
 * @brief Create an instance of the database
 *
 * @return 
 */
ZrDB * ZrDB::Instance()
{
    RsStackMutex creationMutex( creation_mutex );
    if( !ZrDB::instance ) {
        ZrDB::instance = new ZrDB();
        try{
            ZrDB::instance->init();
        }
        catch( std::exception e ){
            QMessageBox::critical(0, "SQL Error", e.what() );
            delete ZrDB::instance;
            ZrDB::instance = 0;
        }
    }
    return ZrDB::instance;
}

/**
 * @brief Initialise
 * 
 */
void ZrDB::init()
{
    char *zErrMsg = 0;
    int rc;

    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    std::string pathname = RsInit::RsConfigDirectory() + "/" +
            p3zr->getOwnId() + "/zeroreserve";
    QDir zrdata ( QString::fromStdString(pathname) );

    if( !zrdata.mkpath( QString::fromStdString( pathname ) ) )
    {
        QMessageBox::critical(0, "Error", "Cannot create DB");
        return;
    }
    std::string db_name = pathname + "/zeroreserve.db";
    bool db_exists = QFile::exists(db_name.c_str() );

    std::cerr << "Opening or creating DB " << db_name << std::endl;
    rc = sqlite3_open( db_name.c_str(), &m_db);

    // unable to open database
    if( rc )
    {
        std::cerr <<  "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_close(m_db);
        throw  std::runtime_error("SQL Error: Cannot open database");
    }

    if( !db_exists )
    {
        std::cerr << "Populating " << db_name << std::endl;
        std::vector < std::string > tables;
        tables.push_back( "create table if not exists peers ( id varchar(32), currency varchar(3), our_credit decimal(12,8), credit decimal(12,8), balance decimal(12,8) )");
        tables.push_back( "create table if not exists config ( key varchar(32), value varchar(160) )");
        tables.push_back( "create table if not exists payments ( payee varchar(32), currency varchar(3), amount decimal(12,8) )");
        tables.push_back( "create table if not exists myorders ( orderid varchar(32), ordertype int, amount decimal(12,8), price decimal(12,8), currency varchar(3), creationtime int, purpose int )");
        tables.push_back( "create table if not exists mywallet ( secret varchar(64), type int, nick varchar(64) )");
        tables.push_back( "create table if not exists peerwallet ( address varchar(34), nick varchar(64) )");
        tables.push_back( "create unique index if not exists id_curr on peers ( id, currency)");
        for(std::vector < std::string >::const_iterator it = tables.begin(); it != tables.end(); it++ )
        {
            rc = sqlite3_exec(m_db, (*it).c_str(), noop_callback, this, &zErrMsg);
            if( rc!=SQLITE_OK )
            {
                std::cerr << "SQL error: " << zErrMsg << std::endl;
                sqlite3_free(zErrMsg);
                throw std::runtime_error("SQL Error: Cannot create table");
            }
        }
        setConfig( TXLOGPATH, pathname + "/zeroreserve.tx" );
        setConfig( DB_VERSION, "0" );
    }
    openTxLog();
}

/**
 * @brief Set the configuration value
 *
 * @param key
 * @param value
 */
void ZrDB::setConfig( const std::string & key, const std::string & value )
{
    RsStackMutex configMutex( m_config_mutex );
    std::string insert =  "insert into config values( '";
    insert += key + "', '" + value + "' )";
    runQuery( insert );
}

/**
 * @brief Update configuration
 *
 * @param key
 * @param value
 */
void ZrDB::updateConfig( const std::string & key, const std::string & value )
{
    RsStackMutex configMutex( m_config_mutex );
    std::string update =  "update config set value= '";
    update += value +"' where key = '" + key + "'";
    runQuery( update );
}

/**
 * @brief Get configuration of database
 *
 * @param key
 *
 * @return 
 */
std::string ZrDB::getConfig( const std::string & key )
{
    char *zErrMsg = 0;
    RsStackMutex configMutex( m_config_mutex );
    m_config_value.clear();
    std::string select = "select value from config where key = '";
    select += key + "'";
    int rc = sqlite3_exec(m_db, select.c_str(), peer_config_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( std::string( "SQL Error: Cannot get value " ) + key );
    }
    return m_config_value;
}


/**
 * @brief Load grand total from the database
 *
 * @param currency
 *
 * @return 
 */
ZrDB::GrandTotal & ZrDB::loadGrandTotal( const std::string & currency )
{
    char *zErrMsg = 0;
    RsStackMutex peerMutex( m_peer_mutex );
    grandTotal.currency =  currency;
    grandTotal.our_credit = 0;
    grandTotal.credit = 0;
    grandTotal.outstanding = 0;
    grandTotal.debt = 0;
    grandTotal.balance = 0;

    std::ostringstream select;
    select << "select our_credit, credit, balance from peers where currency = '" << currency << "'";
    int rc = sqlite3_exec(m_db, select.str().c_str(), grandtotal_callback, this, &zErrMsg);

    if( rc!=SQLITE_OK )
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot store peer data" );
    }
    return grandTotal;
}

/**
 * @brief Add to grand total
 *
 * @param cols
 */
void ZrDB::addToGrandTotal( char ** cols )
{
    grandTotal.our_credit += atof( cols[0] );
    grandTotal.credit     += atof( cols[1] );
    ZR::ZR_Number peerbalance = atof ( cols[2] );
    grandTotal.balance    += peerbalance;
    
    if( peerbalance > 0 )
    {
        grandTotal.outstanding += peerbalance;
    }
    else 
    {
        grandTotal.debt        -= peerbalance;
    }
}


/**
 * @brief Peer exists ?
 *
 * @param peer_in
 *
 * @return 
 */
bool ZrDB::peerExists( const Credit & peer_in )
{
    char *zErrMsg = 0;
    RsStackMutex peerMutex( m_peer_mutex );
    m_peer_record_exists = false;
    std::ostringstream select;
    select << "select * from peers where id = '" << peer_in.m_id
           << "' and currency = '" << peer_in.m_currency << "'";
    int rc = sqlite3_exec(m_db, select.str().c_str(), store_peer_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot store peer data" );
    }
    return m_peer_record_exists;
}

/**
 * @brief Update peer credit
 *
 * @param peer_in
 * @param column
 * @param value
 */
void ZrDB::updatePeerCredit( const Credit & peer_in, const std::string & column, ZR::ZR_Number & value )
{
    RsStackMutex peerMutex( m_peer_mutex );
    std::cerr << "Zero Reserve: Updating peer credit " << peer_in.m_id << std::endl; 
    std::ostringstream update;
    update << "update peers set " <<
              column << " = " << value.toDouble() <<
              " where id = '" << peer_in.m_id << "'" <<
              " and currency = '" << peer_in.m_currency << "'";
    runQuery( update.str() );
}

/**
 * @brief Run a query
 *
 * @param sql
 */
void ZrDB::runQuery(const std::string & sql )
{
    char *zErrMsg = 0;
    int rc = sqlite3_exec(m_db, sql.c_str(), noop_callback, this, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot update peer data" );
    }
}

/**
 * @brief Create a peer record
 *
 * @param peer_in
 */
void ZrDB::createPeerRecord( const Credit & peer_in )
{
    std::cerr << "Zero Reserve: Updating peer credit " << peer_in.m_id << std::endl;
    std::ostringstream insert;
    insert << "insert into peers (id, currency, our_credit, credit, balance) values( '"
           << peer_in.m_id << "', '" << peer_in.m_currency << "', 0, 0, 0 )";

    runQuery( insert.str() );
}




/**
 * @brief Load peer record
 *
 * @param peer_out
 */
void ZrDB::loadPeer( Credit & peer_out )
{
    RsStackMutex peerMutex( m_peer_mutex );
    m_credit = &peer_out;
    char *zErrMsg = 0;
    std::ostringstream select;
    select << "select credit, our_credit, balance from peers where id = '"
           << peer_out.m_id << "' and currency = '" << peer_out.m_currency << "'";
    std::string selectstr = select.str();
    int rc = sqlite3_exec(m_db, selectstr.c_str(), peer_credit_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot load peer data" );
    }
}

/**
 * @brief Load peer overloaded
 *
 * @param id
 * @param peer_out
 */
void ZrDB::loadPeer( const std::string & id, Credit::CreditList & peer_out )
{
    RsStackMutex peerMutex( m_peer_mutex );
    m_creditList = &peer_out;
    char *zErrMsg = 0;
    std::ostringstream select;
    select << "select id, currency, credit, our_credit, balance from peers where id = '" << id << "'";
    std::string selectstr = select.str();
    int rc = sqlite3_exec(m_db, selectstr.c_str(), peer_credits_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot load peer data" );
    }
}


/**
 * @brief Set peer credit
 *
 * @param credit
 * @param our_credit
 * @param balance
 */
void ZrDB::setPeerCredit( const std::string & credit, const std::string & our_credit, const std::string & balance )
{
    m_credit->m_credit = ZR::ZR_Number::fromDecimalString( credit );
    m_credit->m_our_credit = ZR::ZR_Number::fromDecimalString( our_credit );
    m_credit->m_balance = ZR::ZR_Number::fromDecimalString( balance );
}

/**
 * @brief Add peer credit
 *
 * @param credit
 */
void ZrDB::addPeerCredit( Credit * credit )
{
    m_creditList->push_back( credit );
}

/**
 * @brief Open transaction log
 * 
 */
void ZrDB::openTxLog()
{
    char *zErrMsg = 0;
    std::string txLog = getConfig( TXLOGPATH );
    int rc = sqlite3_open( txLog.c_str(), &m_txLog );

    if( rc )
    {
        std::cerr <<  "Can't open transaction log: " << sqlite3_errmsg( m_txLog ) << std::endl;
        sqlite3_close(m_txLog);
        throw  std::runtime_error("SQL Error: Cannot open database");
    }

    rc = sqlite3_exec(m_txLog, "create table if not exists txlog ( uid varchar(32), currency varchar(3), amount decimal(12,8), txtime datetime default current_timestamp )", noop_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error("SQL Error: Cannot create table");
    }
}

/**
 * @brief Add transaction to the database log
 *
 * @param id
 * @param currency
 * @param amount
 */
void ZrDB::appendTx(const std::string & id, const std::string & currency, ZR::ZR_Number amount )
{
    char *zErrMsg = 0;
    std::cerr << "Zero Reserve: Appending to TX log " << id << ". " << amount << std::endl;
    std::ostringstream insert;
    insert << "insert into txlog ( uid, currency, amount ) values( '"
           << id << "', '"
           << currency << "', "
           << amount.toDouble() << " )";
    int rc = sqlite3_exec(m_txLog, insert.str().c_str(), noop_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot append to TX log" );
    }
}

/**
 * @brief Load transaction log
 *
 * @param txList
 */
void ZrDB::loadTxLog( std::list< TxLogItem > & txList )
{
    char *zErrMsg = 0;
    RsStackMutex txMutex( m_tx_mutex );
    m_txList = &txList;

    int rc = sqlite3_exec(m_txLog, "select uid,currency,amount,txtime from txlog order by txtime desc", txlog_callback, this, &zErrMsg);
    
    if( rc != SQLITE_OK )
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot read TX log" );
    }
}


/**
 * @brief Add to transaction list
 *
 * @param item
 */
void ZrDB::addToTxList( const TxLogItem & item )
{
    m_txList->push_back( item );
}

////////////////////////////////////////////////////////////////

/**
 * @brief Add order to myorders
 *
 * @param order
 */
void ZrDB::addOrder( OrderBook::Order * order )
{
    std::ostringstream insert;
    insert << "insert into myorders ( orderid, ordertype, amount, price, currency, creationtime, purpose ) values( '"
           << order->m_order_id << "', "
           << order->m_orderType << ", "
           << order->m_amount.toDouble() << ", "
           << order->m_price.toDouble() << ", '"
           << Currency::currencySymbols[ order->m_currency ] << "', "
           << order->m_timeStamp << ", "
           << order->m_purpose << " )";

    runQuery( insert.str() );
}

/**
 * @brief Load orders from my orders
 *
 * @param orders_out
 */
void ZrDB::loadOrders( OrderBook::OrderList * orders_out )
{
    char *zErrMsg = 0;
    m_orderList = orders_out;
    int rc = sqlite3_exec(m_db, "select orderid, ordertype, amount, price, currency, creationtime, purpose from myorders", orders_callback, this, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot read Orders" );
    }
}

/**
 * @brief Update my orders
 *
 * @param order
 */
void ZrDB::updateOrder( OrderBook::Order * order )
{
    std::cerr << "Zero Reserve: Updating my orders " << order->m_order_id << std::endl;
    std::ostringstream update;
    update << "update myorders set amount = " << order->m_amount
           << " where orderid = '" << order->m_order_id << "'";

    runQuery( update.str() );
}

/**
 * @brief Delete an order from my orders
 *
 * @param order
 */
void ZrDB::deleteOrder( OrderBook::Order * order )
{
    std::cerr << "Zero Reserve: Updating my orders " << order->m_order_id << std::endl;
    std::ostringstream rmo;
    rmo << "delete from  myorders where orderid = '" << order->m_order_id << "'";
    runQuery( rmo.str() );
}


/**
 * @brief Add order to order list
 * 
 * The order is being pushed back into the vector
 *
 * @param order
 */
void ZrDB::addToOrderList( OrderBook::Order * order )
{
    m_orderList->push_back( order );
}


/////////////////////////// Wallet /////////////////////////////////////


/**
 * @brief Add my wallet
 *
 * @param secret
 * @param type
 * @param nick
 */
void ZrDB::addMyWallet( const ZR::WalletSecret & secret, unsigned int type, const std::string & nick )
{
    std::cerr << "Zero Reserve: Inserting my wallet " << std::endl;
    char *zErrMsg = 0;
    std::ostringstream insert;
    insert << "insert into mywallet ( secret, type, nick ) values( '"
           << secret << "', '"
           << type << "', '"
           << nick << "' )";
    int rc = sqlite3_exec( m_db, insert.str().c_str(), noop_callback, this, &zErrMsg );
    if( rc != SQLITE_OK )
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot insert my wallet" );
    }
}


/**
 * @brief Add peer wallet
 *
 * @param address
 * @param nick
 */
void ZrDB::addPeerWallet( const ZR::BitcoinAddress & address, const std::string & nick )
{
    std::cerr << "Zero Reserve: Inserting peer wallet " << std::endl;
    char *zErrMsg = 0;
    std::ostringstream insert;
    insert << "insert into peerwallet ( address, nick ) values( '"
           << address << "', '"
           << nick << "' )";
    int rc = sqlite3_exec( m_db, insert.str().c_str(), noop_callback, this, &zErrMsg );
    if( rc != SQLITE_OK )
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot insert peer wallet" );
    }
}

////////////////////////////////////////////////////////////////

/**
 * @brief Close transaction log
 */
void ZrDB::closeTxLog()
{
    sqlite3_close( m_txLog );
}

/**
 * @brief Close the database
 * 
 */
void ZrDB::close()
{
    sqlite3_close( m_db );
    closeTxLog();
}

// EOF   
