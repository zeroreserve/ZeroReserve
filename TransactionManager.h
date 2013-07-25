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

#include <map>
#include <string>

class RsZeroReserveTxItem;
class RsZeroReserveInitTxItem;

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
        INIT = 0,
        QUERY,
        VOTE,
        COMMIT,
        ACK_COMMIT,
        ABORT,
        ACK
    };
    enum Role {
         Manager = 0,
         Payee,
         Hop
    };

    TransactionManager();
    ~TransactionManager();

    bool initCoordinator( const std::string & payee, const std::string & amount, const std::string & currency );
    bool initCohort( RsZeroReserveInitTxItem * item );

    static bool handleTxItem( RsZeroReserveTxItem * item );

private:
    bool processItem( RsZeroReserveTxItem * item );
    static void abortTx( RsZeroReserveTxItem * item );

    Role role;
    std::string coordinator;

    static TxManagers currentTX;
};

#endif // TRANSACTIONMANAGER_H
