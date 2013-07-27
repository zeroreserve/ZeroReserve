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

#include "util/rsthreads.h"

#include <sqlite3.h>
#include <string>

#include <stdlib.h>

/**
  Database class to save and load friend data and payment info. Uses sqlite3
  */

class ZrDB
{
    ZrDB();
    void init();
public:

    typedef struct  {
        std::string id;
        std::string currency;
        std::string our_credit; // our credit with peer
        std::string credit;     // their credit with us
        std::string balance;    // negative means we owe them money
    } Credit;

    typedef struct {
        std::string currency;
        ZR_Number our_credit;  // credit with all peers
        ZR_Number credit;      // their credit with us
        ZR_Number outstanding; // their debt with us
        ZR_Number debt;        // our debt with them
        ZR_Number balance;
    } GrandTotal;

    static ZrDB * Instance();
    void storePeer( const Credit & peer_in );
    void loadPeer( const std::string & id, Credit & peer_out );
    const GrandTotal & loadGrandTotal( const std::string & currency );

    void close();
    void peerRecordExists(){ m_peer_record_exists = true; }
    void setPeerCredit( const std::string & credit ) { m_credit->credit = credit; }
    void setConfigValue( const std::string & val ) { m_config_value = val; }
    void addToGrandTotal( char ** cols );

    std::string getConfig( const std::string & key );
    void updateConfig( const std::string & key, const std::string & value );

    // TODO void logPayment() const;
    // TODO: void replayPaymentLog();
    // TODO: void backup() const;
    // TODO: void restore() const;
private:
    void setConfig( const std::string & key, const std::string & value );

private:
    RsMutex m_peer_mutex;
    RsMutex m_config_mutex;

    Credit * m_credit;
    sqlite3 *m_db;

    bool m_peer_record_exists;
    std::string m_config_value;
    GrandTotal grandTotal;

    static ZrDB * instance;
    static RsMutex creation_mutex;
};

#endif // ZRDB_H
