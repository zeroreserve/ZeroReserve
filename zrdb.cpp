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

#include <iostream>
#include <sstream>
#include <vector>
#include <exception>
#include <QMessageBox>

#include "retroshare/rsinit.h"


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

static int peer_credit_callback(void *, int argc, char ** argv, char ** colNames)
{
    if(argc == 1){
        ZrDB::Instance()->setPeerCredit( argv[0] );
    }
    return SQLITE_OK;
}


ZrDB::ZrDB() :
        m_peer_mutex("peer_mutex")
{
    try{
        init();
    }
    catch( const char * e){
        QMessageBox::critical(0, "SQL Error", e);
    }
}

ZrDB * ZrDB::Instance()
{
    RsMutex creationMutex( creation_mutex );
    if( ZrDB::instance == 0 ) ZrDB::instance = new ZrDB();
    return ZrDB::instance;
}

void ZrDB::init()
{
    char *zErrMsg = 0;
    int rc;
    std::string db_name = RsInit::RsConfigDirectory() + "/zeroreserve.db";
    std::vector < const char* > tables;
    tables.push_back( "create table if not exists peers ( id varchar(32), currency varchar(3), credit decimal(8,8), balance decimal(8,8) )");
    tables.push_back( "create table if not exists config ( key varchar(32), value varchar(80) )");
    tables.push_back( "create table if not exists payments ( payee varchar(32), currency varchar(3), amount decimal(8,8) )");
    tables.push_back( "create unique index if not exists id_curr on peers ( id, currency)");


    std::cerr << "Opening or creating DB " << db_name << std::endl;
    rc = sqlite3_open( db_name.c_str(), &m_db);
    if( rc ){
        std::cerr <<  "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_close(m_db);
        throw  "SQL Error: Cannot open database";

    }

    for(std::vector < const char* >::const_iterator it = tables.begin(); it != tables.end(); it++ ){
        rc = sqlite3_exec(m_db, *it, noop_callback, 0, &zErrMsg);
        if( rc!=SQLITE_OK ){
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
            throw "SQL Error: Cannot create table";
        }
    }
}


void ZrDB::storePeer( const ZrDB::Credit & peer_in )
{
    RsMutex peerMutex( m_peer_mutex );
    char *zErrMsg = 0;
    m_peer_record_exists = false;
    std::ostringstream select;
    select << "select * from peers where id = '" << peer_in.id
           << "' and currency = '" << peer_in.currency << "'";
    int rc = sqlite3_exec(m_db, select.str().c_str(), store_peer_callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        throw "SQL Error: Cannot store peer data";
    }
    if(m_peer_record_exists) {
        std::cerr << "Zero Reserve: Updating peer record" << std::endl;
    }
    else {
        std::cerr << "Zero Reserve: Creating new peer record" << std::endl;
        std::ostringstream insert;
        insert << "insert into peers (id, currency, credit, balance) values( '"
               << peer_in.id << "', '" << peer_in.currency << "', "
               << peer_in.credit << ", 0 )";
        int rc = sqlite3_exec(m_db, insert.str().c_str(), noop_callback, 0, &zErrMsg);
        if( rc!=SQLITE_OK ){
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
            throw "SQL Error: Cannot store peer data";
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
        throw "SQL Error: Cannot load peer data";
    }
}

void ZrDB::close()
{
    sqlite3_close( m_db );
}
