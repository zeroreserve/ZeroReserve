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


#ifndef TRANSACTIONMANAGER_H
#define TRANSACTIONMANAGER_H

#include <zrtypes.h>

#include <map>
#include <string>
#include <vector>

class RsZeroReserveTxItem;
class RsZeroReserveInitTxItem;
class RSZRRemoteTxItem;

/**
  Manage multi hop transaction. The payer is the coordiantor, all in between
  nodes and the payee are cohorts. Cancel TX if an in-between node tries to
  cheat or if an in-between node goes away during the TX
  */

class TransactionManager
{
    TransactionManager( const TransactionManager &);
    TransactionManager();
public:
    typedef std::map< std::string, TransactionManager *> TxManagers;
    enum TxPhase {
        INIT = 0,
        QUERY,
        VOTE_YES,
        VOTE_NO,
        COMMIT,
        ACK_COMMIT,
        ABORT,
        ABORT_REQUEST,
        PHASE_NUMBER
    };
    enum Role {
         Coordinator = 0,
         Payee,
         Hop
    };

    TransactionManager( const ZR::TransactionId & txId );
    virtual ~TransactionManager();

    virtual ZR::RetVal init() = 0;

    static int handleTxItem( RsZeroReserveTxItem * item );
    static int handleTxItem( RSZRRemoteTxItem *item );

    static void timeout();

protected:

    virtual ZR::RetVal processItem( RsZeroReserveTxItem * item ){ return ZR::ZR_FAILURE; }
    virtual ZR::RetVal processItem( RSZRRemoteTxItem * item ){ return ZR::ZR_FAILURE; }
    virtual ZR::RetVal abortTx( RsZeroReserveTxItem * item ){ return ZR::ZR_FAILURE; }
    virtual ZR::RetVal abortTx( RSZRRemoteTxItem * item ){ return ZR::ZR_FAILURE; }

    virtual void rollback() = 0;
    virtual bool isTimedOut();


    const ZR::TransactionId m_TxId;
    TxPhase m_Phase;
    qint64 m_startOfPhase;
    /** maximum time of each phase */
    qint64 m_maxTime[ PHASE_NUMBER ];

    static TxManagers currentTX;

    static void split(const std::string & s, std::vector< std::string > & v, const char sep = ':' );
};

#endif // TRANSACTIONMANAGER_H
