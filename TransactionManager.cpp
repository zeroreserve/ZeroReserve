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

#include "TransactionManager.h"
#include "RSZeroReserveItems.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"


bool TransactionManager::handleTxItem( RsZeroReserveTxItem * item )
{
    std::string id = item->PeerId();
    TxManagers::iterator it = currentTX.find( id );
    if( it == currentTX.end() ){
        if( item->getTxPhase() == INIT ){  // a new request to receive or forward payment
            RsZeroReserveInitTxItem * initItem = dynamic_cast<RsZeroReserveInitTxItem *>( item );
            if(!initItem) return false;

            TransactionManager *tm = new TransactionManager();
            currentTX[ id ] = tm;
            return tm->initCohort( initItem );
        }
        else {
            abortTx( item );
            return false;
        }
    }
    TransactionManager *tm = (*it).second;
    return tm->processItem( item );
}



TransactionManager::TransactionManager()
{
}

bool TransactionManager::initCohort( RsZeroReserveInitTxItem * item )
{
    role = (Role)item->getRole();
    coordinator = item->PeerId();
    // TODO: multi hop
    RsZeroReserveTxItem * reply = new RsZeroReserveTxItem( ACK );
    p3ZeroReserveRS * p3zs = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    p3zs->sendItem( reply ); // TODO: error  handling
    return true;
}

bool TransactionManager::initCoordinator( const std::string & payee, const std::string & amount )
{
    role = Manager;
    RsZeroReserveInitTxItem * initItem = new RsZeroReserveInitTxItem( INIT, amount);
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    p3zr->sendItem( initItem );
    return true;
}

bool TransactionManager::processItem( RsZeroReserveTxItem * item )
{
    switch( item->getTxPhase() )
    {
    case INIT:
        abortTx( item ); // we should never get here
        break;
    case QUERY:
        break;
    case VOTE:
        break;
    case COMMIT:
        break;
    case ACK:
        break;
    case ABORT:
        break;
    default:
        return false;
    }
    return true;
}

void TransactionManager::abortTx( RsZeroReserveTxItem * item )
{
    // TODO
}
