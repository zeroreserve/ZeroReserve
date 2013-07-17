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

#include "util/rsthreads.h"

#include <sqlite3.h>
#include <string>

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

    static ZrDB * Instance();
    void storePeer( const Credit & peer_in );
    void loadPeer( const std::string & id, Credit & peer_out );
    void close();
    void peerRecordExists(){ m_peer_record_exists = true; }
    void setPeerCredit( const std::string & credit ) { m_credit->credit = credit; }

    // TODO void logPayment() const;
    // TODO: void replayPaymentLog();
    // TODO: void backup() const;
    // TODO: void restore() const;

private:
    RsMutex m_peer_mutex;
    Credit * m_credit;
    sqlite3 *m_db;
    bool m_peer_record_exists;

    static ZrDB * instance;
    static RsMutex creation_mutex;
};

#endif // ZRDB_H
