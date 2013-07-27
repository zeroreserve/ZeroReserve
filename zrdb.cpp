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

#include "retroshare/rsinit.h"

#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>




ZrDB * ZrDB::instance = 0;
RsMutex ZrDB::creation_mutex("creation_mutex");


static int noop_callback(void *, int, char **, char **)
{
    return SQLITE_OK;
}


static int store_peer_callback(void *, int, char **, char **)
{
    ZrDB::Instance()->peerRecordExists();
    return SQLITE_OK;
}

static int peer_credit_callback(void *, int argc, char ** argv, char ** )
{
    if(argc == 1){
        ZrDB::Instance()->setPeerCredit( argv[0] );
    }
    else {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

static int grandtotal_callback(void *, int argc, char ** argv, char ** )
{
    if(argc != 3) return SQLITE_ERROR;

    ZrDB::Instance()->addToGrandTotal( argv );
    return SQLITE_OK;
}

static int peer_config_callback(void *, int argc, char ** argv, char ** )
{
    if(argc == 1){
        ZrDB::Instance()->setConfigValue( argv[0] );
    }
    else {
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

ZrDB::ZrDB() :
        m_peer_mutex("peer_mutex"),
        m_config_mutex("config_mutex")
{

}

ZrDB * ZrDB::Instance()
{
    RsMutex creationMutex( creation_mutex );
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
    std::string txlog;
    if( db_exists ){
        txlog = getConfig( "TXLOG" );
    }
    else {
        std::cerr << "Populating " << db_name << std::endl;
        std::vector < std::string > tables;
        tables.push_back( "create table if not exists peers ( id varchar(32), currency varchar(3), our_credit decimal(12,8), credit decimal(12,8), balance decimal(12,8) )");
        tables.push_back( "create table if not exists config ( key varchar(32), value varchar(160) )");
        tables.push_back( "create table if not exists payments ( payee varchar(32), currency varchar(3), amount decimal(12,8) )");
        tables.push_back( "create unique index if not exists id_curr on peers ( id, currency)");
        for(std::vector < std::string >::const_iterator it = tables.begin(); it != tables.end(); it++ ){
            rc = sqlite3_exec(m_db, (*it).c_str(), noop_callback, 0, &zErrMsg);
            if( rc!=SQLITE_OK ){
                std::cerr << "SQL error: " << zErrMsg << std::endl;
                sqlite3_free(zErrMsg);
                throw std::runtime_error("SQL Error: Cannot create table");
            }
        }

        txlog = QFileDialog::getExistingDirectory( 0, "Select directory for Transaction log", QString::fromStdString( pathname ) ).toStdString()
                + "/zeroreserve.tx";
        setConfig( "TXLOG", txlog );
    }
    // TODO: Do something with the tx log
}

void ZrDB::setConfig( const std::string & key, const std::string & value )
{
    char *zErrMsg = 0;
    RsMutex configMutex( m_config_mutex );
    std::string insert =  "insert into config values( '";
    insert += key + "', '" + value + "' )";
    int rc = sqlite3_exec(m_db, insert.c_str(), noop_callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot store config data" );
    }
}

void ZrDB::updateConfig( const std::string & key, const std::string & value )
{
    char *zErrMsg = 0;
    RsMutex configMutex( m_config_mutex );
    std::string update =  "update config set value= '";
    update += value +"' where key = '" + key + "'";
    int rc = sqlite3_exec(m_db, update.c_str(), noop_callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot update config data" );
    }
}

std::string ZrDB::getConfig( const std::string & key )
{
    char *zErrMsg = 0;
    RsMutex configMutex( m_config_mutex );
    m_config_value.clear();
    std::string select = "select value from config where key = '";
    select += key + "'";
    int rc = sqlite3_exec(m_db, select.c_str(), peer_config_callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( std::string( "SQL Error: Cannot get value " ) + key );
    }
    return m_config_value;
}


const ZrDB::GrandTotal & ZrDB::loadGrandTotal( const std::string & currency )
{
    char *zErrMsg = 0;
    RsMutex peerMutex( m_peer_mutex );
    grandTotal.currency =  currency;
    grandTotal.our_credit = 0;
    grandTotal.credit = 0;
    grandTotal.outstanding = 0;
    grandTotal.debt = 0;
    grandTotal.balance = 0;

    std::ostringstream select;
    select << "select our_credit, credit, balance from peers where currency = '" << currency << "'";
    int rc = sqlite3_exec(m_db, select.str().c_str(), grandtotal_callback, 0, &zErrMsg);
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
    ZR_Number peerbalance = atof ( cols[2] );
    grandTotal.balance    += peerbalance;
    if( peerbalance > 0 ) grandTotal.outstanding += peerbalance;
    if( peerbalance < 0 ) grandTotal.debt        += peerbalance;
}


void ZrDB::storePeer( const ZrDB::Credit & peer_in )
{
    char *zErrMsg = 0;
    RsMutex peerMutex( m_peer_mutex );
    m_peer_record_exists = false;
    std::ostringstream select;
    select << "select * from peers where id = '" << peer_in.id
           << "' and currency = '" << peer_in.currency << "'";
    int rc = sqlite3_exec(m_db, select.str().c_str(), store_peer_callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot store peer data" );
    }
    if(m_peer_record_exists) {
        std::cerr << "Zero Reserve: Updating peer record " << peer_in.id << std::endl;
        std::ostringstream update;
        update << "update peers set" <<
                  " credit = " << peer_in.credit <<
                  " where id = '" << peer_in.id << "'" <<
                  " and currency = '" << peer_in.currency << "'";
        int rc = sqlite3_exec(m_db, update.str().c_str(), noop_callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
            throw std::runtime_error( "SQL Error: Cannot update peer data" );
        }
    }
    else {
        std::cerr << "Zero Reserve: Creating new peer record " << peer_in.id << std::endl;
        std::ostringstream insert;
        insert << "insert into peers (id, currency, our_credit, credit, balance) values( '"
               << peer_in.id << "', '" << peer_in.currency << "', 0, "
               << peer_in.credit << ", 0 )";
        int rc = sqlite3_exec(m_db, insert.str().c_str(), noop_callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
            throw std::runtime_error( "SQL Error: Cannot store peer data" );
        }
    }
}


void ZrDB::loadPeer( const std::string & id, ZrDB::Credit & peer_out )
{
    RsMutex peerMutex( m_peer_mutex );
    m_credit = &peer_out;
    char *zErrMsg = 0;
    std::ostringstream select;
    select << "select credit from peers where id = '"
           << id << "' and currency = '" << peer_out.currency << "'";
    std::string selectstr = select.str();
    int rc = sqlite3_exec(m_db, selectstr.c_str(), peer_credit_callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw std::runtime_error( "SQL Error: Cannot load peer data" );
    }
}

void ZrDB::close()
{
    sqlite3_close( m_db );
}
