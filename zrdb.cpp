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

#include "zrdb.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"
#include "BtcContract.h"

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
const char * const ZrDB::MINIMUM_FEE      = "MINIMUM_FEE";
const char * const ZrDB::PERCENTAGE_FEE   = "PERCENTAGE_FEE";



ZrDB * ZrDB::instance = 0;
RsMutex ZrDB::creation_mutex("creation_mutex");


static int btccontracts_callback(void * , int argc, char ** argv, char ** )
{
    if( argc != 9 ) return SQLITE_ERROR;

    ZR::ZR_Number btcAmount = ZR::ZR_Number::fromDecimalString( std::string( argv[ 1 ] ) );
    ZR::ZR_Number price = ZR::ZR_Number::fromDecimalString( std::string( argv[ 2 ] ) );
    std::string currencySym = argv[ 3 ];
    BtcContract::Party party = (BtcContract::Party)atoi( argv[ 4 ] );
    std::string counterParty = argv[ 5 ];
    qint64 creationtime = atoll( argv[7] );
    ZR::ZR_Number fee = ZR::ZR_Number::fromDecimalString( std::string( argv[ 8 ] ) );

    BtcContract * contract = new BtcContract(btcAmount, fee, price, currencySym, party, counterParty, creationtime );
    contract->setBtcTxId( argv[ 0 ] );
    contract->setBtcAddress( argv[ 6 ] );
    contract->activate();

    BtcContract::contracts.push_back( contract );
    return SQLITE_OK;
}

static int mywallets_callback(void * db, int argc, char ** argv, char ** )
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    if( argc == 3 ){
        ZrDB::MyWallet wallet;
        wallet.secret = argv[ 0 ];
        wallet.type = atoi( argv[ 1 ] );
        wallet.nick = argv[ 2 ];
        zrdb->addMyWallet( wallet );
    }
    else {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

static int orders_callback(void * db, int argc, char ** argv, char ** )
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    if( argc == 7 ){
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
    else {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

static int txlog_callback(void * db, int argc, char ** argv, char ** )
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    if(argc == 4){
        ZrDB::TxLogItem item;
        item.id = argv[0];
        item.currency = argv[1];
        item.m_amount = ZR::ZR_Number::fromDecimalString( std::string(argv[2]) );
        item.timestamp = QDateTime::fromString( argv[3], "yyyy-MM-dd HH:mm:ss" );

        zrdb->addToTxList( item );
    }
    else {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}


static int noop_callback(void * db, int, char **, char **)
{
    return SQLITE_OK;
}


static int store_peer_callback(void * db, int, char **, char **)
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    zrdb->peerRecordExists();
    return SQLITE_OK;
}

static int peer_credit_callback(void * db, int argc, char ** argv, char ** )
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    if( argc == 4 ){
        zrdb->setPeerCredit( argv[0], argv[1], argv[2], argv[3] );
    }
    else {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

static int peer_credits_callback(void * db, int argc, char ** argv, char ** )
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    if(argc == 6){
        Credit * credit = new Credit( argv[0], argv[1]);
        credit->m_credit = ZR::ZR_Number::fromDecimalString( std::string( argv[2] ) );
        credit->m_our_credit = ZR::ZR_Number::fromDecimalString( std::string( argv[3] ) );
        credit->m_balance = ZR::ZR_Number::fromDecimalString( std::string( argv[4] ) );
        credit->m_allocated = ZR::ZR_Number::fromDecimalString( std::string( argv[5] ) );
        zrdb->addPeerCredit( credit );
    }
    else {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

static int grandtotal_callback(void * db, int argc, char ** argv, char ** )
{
    if(argc != 3) return SQLITE_ERROR;
    ZrDB * zrdb = static_cast< ZrDB * >( db );

    zrdb->addToGrandTotal( argv );
    return SQLITE_OK;
}

static int peer_config_callback(void * db, int argc, char ** argv, char ** )
{
    ZrDB * zrdb = static_cast< ZrDB * >( db );
    if(argc == 1){
        zrdb->setConfigValue( argv[0] );
    }
    else {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

ZrDB::ZrDB() :
        m_peer_mutex("peer_mutex"),
        m_config_mutex("config_mutex"),
        m_tx_mutex ( "tx_mutex" )
{

}

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

void ZrDB::init()
{
    char *zErrMsg = 0;
    int rc;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    std::string pathname = RsInit::RsConfigDirectory() + "/" +
            p3zr->getOwnId() + "/zeroreserve";
    QDir zrdata ( QString::fromStdString(pathname) );

    if( !zrdata.mkpath( QString::fromStdString( pathname ) ) ){
        QMessageBox::critical(0, "Error", "Cannot create DB");
        return;
    }
    std::string db_name = pathname + "/zeroreserve.db";
    bool db_exists = QFile::exists(db_name.c_str() );

    std::cerr << "Opening or creating DB " << db_name << std::endl;
    rc = sqlite3_open( db_name.c_str(), &m_db);
    if( rc ){
        std::cerr <<  "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_close(m_db);
        throw  std::runtime_error("SQL Error: Cannot open database");
    }

    if( !db_exists ){
        std::cerr << "Populating " << db_name << std::endl;
        std::vector < std::string > tables;
        tables.push_back( "create table if not exists peers ( id varchar(32), currency varchar(3), our_credit decimal(12,8), credit decimal(12,8), balance decimal(12,8), allocation decimal(12,8) )");
        tables.push_back( "create table if not exists config ( key varchar(32), value varchar(160) )");
        tables.push_back( "create table if not exists payments ( payee varchar(32), currency varchar(3), amount decimal(12,8) )");
        tables.push_back( "create table if not exists myorders ( orderid varchar(32), ordertype int, amount decimal(12,8), price decimal(12,8), currency varchar(3), creationtime int, purpose int )");
        tables.push_back( "create table if not exists mywallet ( secret varchar(64), type int, nick varchar(64) )");
        tables.push_back( "create table if not exists peerwallet ( address varchar(34), nick varchar(64) )");
        tables.push_back( "create table if not exists btccontracts ( btcTxId varchar(64), btcAmount decimal(12,8), price decimal(12,8), currency varchar(3), party int, counterparty varchar(32), destAddress varchar(36), creationtime int, fee decimal(12,8) )");
        tables.push_back( "create unique index if not exists id_curr on peers ( id, currency)");
        for(std::vector < std::string >::const_iterator it = tables.begin(); it != tables.end(); it++ ){
            rc = sqlite3_exec(m_db, (*it).c_str(), noop_callback, this, &zErrMsg);
            if( rc!=SQLITE_OK ){
                std::cerr << "SQL error: " << zErrMsg << std::endl;
                sqlite3_free(zErrMsg);
                throw std::runtime_error("SQL Error: Cannot create table");
            }
        }
        setConfig( TXLOGPATH, pathname + "/zeroreserve.tx" );
        setConfig( DB_VERSION, "0" );
        setConfig( MINIMUM_FEE, "0/1" );
        setConfig( PERCENTAGE_FEE, "0/1" );
    }
    openTxLog();
}

void ZrDB::setConfig( const std::string & key, const std::string & value )
{
    RsStackMutex configMutex( m_config_mutex );
    std::string insert =  "insert into config values( '";
    insert += key + "', '" + value + "' )";
    runQuery( insert );
}

void ZrDB::updateConfig( const std::string & key, const std::string & value )
{
    RsStackMutex configMutex( m_config_mutex );
    std::string update =  "update config set value= '";
    update += value +"' where key = '" + key + "'";
    runQuery( update );
}

std::string ZrDB::getConfig( const std::string & key )
{
    char *zErrMsg = 0;
    RsStackMutex configMutex( m_config_mutex );
    m_config_value.clear();
    std::string select = "select value from config where key = '";
    select += key + "'";
    int rc = sqlite3_exec(m_db, select.c_str(), peer_config_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( std::string( "SQL Error: Cannot get value " ) + key );
    }
    return m_config_value;
}


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
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot store peer data" );
    }
    return grandTotal;
}

void ZrDB::addToGrandTotal( char ** cols )
{
    grandTotal.our_credit += atof( cols[0] );
    grandTotal.credit     += atof( cols[1] );
    ZR::ZR_Number peerbalance = atof ( cols[2] );
    grandTotal.balance    += peerbalance;
    if( peerbalance > 0 ){
        grandTotal.outstanding += peerbalance;
    }
    else {
        grandTotal.debt        -= peerbalance;
    }
}


bool ZrDB::peerExists( const Credit & peer_in )
{
    char *zErrMsg = 0;
    RsStackMutex peerMutex( m_peer_mutex );
    m_peer_record_exists = false;
    std::ostringstream select;
    select << "select * from peers where id = '" << peer_in.m_id
           << "' and currency = '" << peer_in.m_currency << "'";
    int rc = sqlite3_exec(m_db, select.str().c_str(), store_peer_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot store peer data" );
    }
    return m_peer_record_exists;
}

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

void ZrDB::runQuery(const std::string & sql )
{
    char *zErrMsg = 0;
    int rc = sqlite3_exec(m_db, sql.c_str(), noop_callback, this, &zErrMsg);
    if( rc != SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot update peer data" );
    }
}

void ZrDB::createPeerRecord( const Credit & peer_in )
{
    std::cerr << "Zero Reserve: Updating peer credit " << peer_in.m_id << std::endl;
    std::ostringstream insert;
    insert << "insert into peers (id, currency, our_credit, credit, balance, allocation) values( '"
           << peer_in.m_id << "', '" << peer_in.m_currency << "', 0, 0, 0, 0 )";

    runQuery( insert.str() );
}




void ZrDB::loadPeer( Credit & peer_out )
{
    RsStackMutex peerMutex( m_peer_mutex );
    m_credit = &peer_out;
    char *zErrMsg = 0;
    std::ostringstream select;
    select << "select credit, our_credit, balance, allocation from peers where id = '"
           << peer_out.m_id << "' and currency = '" << peer_out.m_currency << "'";
    std::string selectstr = select.str();
    int rc = sqlite3_exec(m_db, selectstr.c_str(), peer_credit_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot load peer data" );
    }
}

void ZrDB::loadPeer( const std::string & id, Credit::CreditList & peer_out )
{
    RsStackMutex peerMutex( m_peer_mutex );
    m_creditList = &peer_out;
    char *zErrMsg = 0;
    std::ostringstream select;
    select << "select id, currency, credit, our_credit, balance, allocation from peers where id = '" << id << "'";
    std::string selectstr = select.str();
    int rc = sqlite3_exec(m_db, selectstr.c_str(), peer_credits_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot load peer data" );
    }
}


void ZrDB::setPeerCredit( const std::string & credit, const std::string & our_credit, const std::string & balance, const std::string & allocation )
{
    m_credit->m_credit = ZR::ZR_Number::fromDecimalString( credit );
    m_credit->m_our_credit = ZR::ZR_Number::fromDecimalString( our_credit );
    m_credit->m_balance = ZR::ZR_Number::fromDecimalString( balance );
    m_credit->m_allocated = ZR::ZR_Number::fromDecimalString( allocation );
}

void ZrDB::addPeerCredit( Credit * credit )
{
    m_creditList->push_back( credit );
}

void ZrDB::openTxLog()
{
    char *zErrMsg = 0;
    std::string txLog = getConfig( TXLOGPATH );
    int rc = sqlite3_open( txLog.c_str(), &m_txLog );
    if( rc ){
        std::cerr <<  "Can't open transaction log: " << sqlite3_errmsg( m_txLog ) << std::endl;
        sqlite3_close(m_txLog);
        throw  std::runtime_error("SQL Error: Cannot open database");
    }
    rc = sqlite3_exec(m_txLog, "create table if not exists txlog ( uid varchar(32), currency varchar(3), amount decimal(12,8), txtime datetime default current_timestamp )", noop_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error("SQL Error: Cannot create table");
    }
}

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
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot append to TX log" );
    }
}

void ZrDB::loadTxLog( std::list< TxLogItem > & txList )
{
    char *zErrMsg = 0;
    RsStackMutex txMutex( m_tx_mutex );
    m_txList = &txList;
    int rc = sqlite3_exec(m_txLog, "select uid,currency,amount,txtime from txlog order by txtime desc", txlog_callback, this, &zErrMsg);
    if( rc != SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot read TX log" );
    }
}


void ZrDB::addToTxList( const TxLogItem & item )
{
    m_txList->push_back( item );
}

////////////////////////////////////////////////////////////////

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

void ZrDB::loadOrders( OrderBook::OrderList * orders_out )
{
    char *zErrMsg = 0;
    m_orderList = orders_out;
    int rc = sqlite3_exec(m_db, "select orderid, ordertype, amount, price, currency, creationtime, purpose from myorders", orders_callback, this, &zErrMsg);
    if( rc != SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot read Orders" );
    }
}

void ZrDB::updateOrder( OrderBook::Order * order )
{
    std::cerr << "Zero Reserve: Updating my orders " << order->m_order_id << std::endl;
    std::ostringstream update;
    update << "update myorders set amount = " << order->m_amount
           << " where orderid = '" << order->m_order_id << "'";

    runQuery( update.str() );
}

void ZrDB::deleteOrder( OrderBook::Order * order )
{
    std::cerr << "Zero Reserve: Updating my orders " << order->m_order_id << std::endl;
    std::ostringstream rmo;
    rmo << "delete from  myorders where orderid = '" << order->m_order_id << "'";
    runQuery( rmo.str() );
}


void ZrDB::addToOrderList( OrderBook::Order * order )
{
    m_orderList->push_back( order );
}


/////////////////////////// Wallet /////////////////////////////////////


ZR::RetVal ZrDB::storeMyWallet( const ZR::WalletSecret & secret, unsigned int type, const std::string & nick )
{
    std::cerr << "Zero Reserve: Inserting my wallet " << std::endl;
    std::ostringstream insert;
    insert << "insert into mywallet ( secret, type, nick ) values( '"
           << secret << "', '"
           << type << "', '"
           << nick << "' )";
    runQuery( insert.str() );
    return ZR::ZR_SUCCESS;
}


ZR::RetVal ZrDB::addPeerWallet( const ZR::BitcoinAddress & address, const std::string & nick )
{
    std::cerr << "Zero Reserve: Inserting peer wallet " << std::endl;
    std::ostringstream insert;
    insert << "insert into peerwallet ( address, nick ) values( '"
           << address << "', '"
           << nick << "' )";
    runQuery( insert.str() );
    return ZR::ZR_SUCCESS;
}


void ZrDB::loadMyWallets( std::vector< MyWallet > & wallets )
{
    char *zErrMsg = 0;
    m_wallets = &wallets;
    std::ostringstream select;
    select << "select secret, type, nick from mywallet";
    int rc = sqlite3_exec(m_db, select.str().c_str(), mywallets_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot store peer data" );
    }
}


void ZrDB::addMyWallet( MyWallet &wallet )
{
    m_wallets->push_back( wallet );
}


/////////////////////////// Contracts /////////////////////////////////////


void ZrDB::addBtcContract( BtcContract * contract )
{
    std::cerr << "Zero Reserve: Inserting contract " << std::endl;
    std::ostringstream insert;
    insert << "insert into btccontracts values( '"
           << contract->m_btcTxId << "', "
           << contract->m_btcAmount.toDecimalStdString() << ", "
           << contract->m_price.toDecimalStdString() << ", '"
           << contract->m_currencySym << "', "
           << (int)contract->m_party << ", '"
           << contract->m_counterParty << "', '"
           << contract->m_destAddress << "', "
           << contract->m_creationtime << ", "
           << contract->m_fee.toDecimalStdString() << " )";

    runQuery( insert.str() );
}

void ZrDB::rmBtcContract(const ZR::TransactionId & btcTxId, int party )
{
    std::cerr << "Zero Reserve: Deleting Contract " << btcTxId << std::endl;
    std::ostringstream rmc;
    rmc << "delete from  btccontracts where btcTxId = '" << btcTxId << "' and party = " << party;
    runQuery( rmc.str() );
}

void ZrDB::loadBtcContracts()
{
    char *zErrMsg = 0;
    std::ostringstream select;
    select << "select btcTxId, btcAmount, price, currency, party, counterparty, destAddress, creationtime, fee from btccontracts order by creationtime desc";
    std::string selectstr = select.str();
    int rc = sqlite3_exec(m_db, selectstr.c_str(), btccontracts_callback, this, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot load contract data" );
    }
    // TODO: Recalculate fund allocation to eliminate stale allocation from a crash
}



////////////////////////// Shutdown //////////////////////////////////////

void ZrDB::closeTxLog()
{
    sqlite3_close( m_txLog );
}

void ZrDB::close()
{
    sqlite3_close( m_db );
    closeTxLog();
}
