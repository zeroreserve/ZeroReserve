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
#include "ZrPeer.h"

#include <iostream>
#include <vector>
#include <exception>
#include <QMessageBox>

#include "retroshare/rsinit.h"


ZrDB * ZrDB::instance = 0;



static int noop_callback(void *, int, char **, char **)
{
    return 0;
}

ZrDB::ZrDB()
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
    // TODO lock
    if( ZrDB::instance == 0 ) ZrDB::instance = new ZrDB();
    return ZrDB::instance;
}

void ZrDB::init()
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    std::string db_name = RsInit::RsConfigDirectory() + "/zeroreserve.db";
    std::vector < const char* > tables;
    tables.push_back( "create table if not exists peers ( id varchar(32), currency varchar(3), credit decimal(8,8), debt decimal(8,8) )");
    tables.push_back( "create table if not exists config ( key varchar(32), value varchar(80) )");
    tables.push_back( "create table if not exists payments ( payee varchar(32), currency varchar(3), amount decimal(8,8) )");

    tables.push_back( "create unique index if not exists id_curr on peers ( id, currency)");


    std::cerr << "Opening or creating DB " << db_name << std::endl;
    rc = sqlite3_open( db_name.c_str(), &db);
    if( rc ){
        std::cerr <<  "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        throw  "SQL Error: Cannot open database";

    }

    for(std::vector < const char* >::const_iterator it = tables.begin(); it != tables.end(); it++ ){
        rc = sqlite3_exec(db, *it, noop_callback, 0, &zErrMsg);
        if( rc!=SQLITE_OK ){
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
            throw "SQL Error: Cannot create table";

        }
    }
    sqlite3_close(db);
}


void ZrDB::storePeer( const ZrPeer & peer_in ) const
{

}


void ZrDB::loadPeer( const ZrPeer & peer_out ) const
{

}


