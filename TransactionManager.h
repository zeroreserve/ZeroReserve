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

class RsZeroReserveTxItem;
class RsZeroReserveInitTxItem;
class Credit;

/**
  Manage multi hop transaction. The payer is the coordiantor, all in between
  nodes and the payee are cohorts. Cancel TX if an in-between node tries to
  cheat or if an in-between node goes away during the TX
  */

class TransactionManager
{
public:
    typedef std::map< std::string, TransactionManager *> TxManagers;
    enum TxPhase {
        QUERY,
        VOTE_YES,
        VOTE_NO,
        COMMIT,
        ACK_COMMIT,
        ABORT
    };
    enum Role {
         Coordinator = 0,
         Payee,
         Hop
    };

    TransactionManager();
    ~TransactionManager();

    bool initCoordinator( const std::string & payee, const std::string & amount, const std::string & currency );

    static bool handleTxItem( RsZeroReserveTxItem * item );

private:
    void commit();
    bool newBalance( const std::string & s_amount );

    bool initCohort( RsZeroReserveInitTxItem * item );
    bool processItem( RsZeroReserveTxItem * item );
    void abortTx( RsZeroReserveTxItem * item );

    Role m_role;
    Credit * m_credit;
    ZR_Number m_newBalance;

    static TxManagers currentTX;
};

#endif // TRANSACTIONMANAGER_H
