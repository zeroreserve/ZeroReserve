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

    typedef struct {
        std::string currency;
        ZR::ZR_Number our_credit;  // credit with all peers
        ZR::ZR_Number credit;      // their credit with us
        ZR::ZR_Number outstanding; // their debt with us
        ZR::ZR_Number debt;        // our debt with them
        ZR::ZR_Number balance;
    } GrandTotal;

    static ZrDB * Instance();
    void createPeerRecord( const Credit & peer_in );
    void updatePeerCredit( const Credit & peer_in, const std::string & column, const std::string & value );
    void loadPeer( Credit & peer_out );
    void loadPeer( const std::string & id, Credit::CreditList & peer_out );
    bool peerExists( const Credit & peer_in );

    const GrandTotal & loadGrandTotal( const std::string & currency );

    std::string getConfig( const std::string & key );
    void updateConfig( const std::string & key, const std::string & value );

    void close();

// TODO: make these functios private by making callbacks private static members
    void peerRecordExists(){ m_peer_record_exists = true; }
    void setPeerCredit( const std::string & credit, const std::string & our_credit, const std::string & balance );
    void addPeerCredit( Credit * credit );
    void setConfigValue( const std::string & val ) { m_config_value = val; }
    void addToGrandTotal( char ** cols );

    // TODO void logPayment() const;
    // TODO: void replayPaymentLog();
    // TODO: void backup() const;
    // TODO: void restore() const;
private:
    void setConfig( const std::string & key, const std::string & value );
    void runQuery( const std::string sql );


private:
    RsMutex m_peer_mutex;
    RsMutex m_config_mutex;

    Credit * m_credit;
    Credit::CreditList * m_creditList;
    sqlite3 *m_db;

    bool m_peer_record_exists;
    std::string m_config_value;
    GrandTotal grandTotal;

    static ZrDB * instance;
    static RsMutex creation_mutex;

public: // config parameters
    static const char * const TXLOGPATH;
};

#endif // ZRDB_H
