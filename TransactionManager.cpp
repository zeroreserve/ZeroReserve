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
#include "TmLocalCohorte.h"
#include "TmContract.h"
#include "RSZeroReserveItems.h"
#include "RSZRRemoteItems.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"
#include "Payment.h"
#include "zrtypes.h"
#include "Router.h"
#include "MyOrders.h"

#include <stdexcept>
#include <sstream>



TransactionManager::TxManagers TransactionManager::currentTX;


/**
 * @brief Handle remote Transaction Items
 * @param item
 * @return
 */

int TransactionManager::handleTxItem( RSZRRemoteTxItem *item )
{
    ZR::RetVal retVal;
    std::cerr << "Zero Reserve: TX Manger handling incoming item - Destination: " << item->getAddress() << std::endl;

    ZR::VirtualAddress addr = item->getAddress();
    ZR::TransactionId txId = addr + ":" + item->getPayerId();
    std::cerr << "Zero Reserve: TransactionManager: TX ID = " << txId << std::endl;
    TransactionManager * tm;
    TxManagers::iterator it = currentTX.find( txId );
    if( it == currentTX.end() ){
        if( MyOrders::Instance()->find( addr ) != MyOrders::Instance()->end() ){
            tm = new TmContractCohortePayee( addr, item->getPayerId() );
        }
        else {
            tm = new TmContractCohorteHop( addr, item->getPayerId() );
        }
    }
    else {
        tm = (*it).second;
    }
    try{
        retVal = tm->processItem( item );
    }
    catch( std::runtime_error e){
        std::cerr << "Zero Reserve: Exception caught: " << e.what() << std::endl;
        return ZR::ZR_FAILURE;
    }

    if( retVal != ZR::ZR_SUCCESS ){
        delete tm;
    }
    return retVal;
}

void TransactionManager::split(const std::string & s, std::vector< std::string > & v, const char sep )
{
    std::stringstream ss( s );
    std::string tok;

    while (getline(ss, tok, sep )) {
        v.push_back( tok );
    }
}

/**
 * @brief Handle local Transaction Items
 * @param item
 * @return success or failure
 */

int TransactionManager::handleTxItem( RsZeroReserveTxItem * item )
{
    std::cerr << "Zero Reserve: TX Manger handling incoming item id = " << item->getTxId() << std::endl;
    ZR::TransactionId txId = item->getTxId();
    TransactionManager * tm;
    TxManagers::iterator it = currentTX.find( txId );
    if( it == currentTX.end() ){
        tm = new TmLocalCohorte( txId );
    }
    else {
        tm = (*it).second;
    }
    ZR::RetVal retVal = tm->processItem( item );
    if( retVal != ZR::ZR_SUCCESS ){
        delete tm;
    }
    return retVal;
}


void TransactionManager::timeout()
{
    for( TxManagers::iterator it = currentTX.begin(); it != currentTX.end(); it++ ){
        TransactionManager * tm = (*it).second;
        if( tm->isTimedOut() ){
            tm->rollback();
            delete tm;
        }
    }
}


TransactionManager::TransactionManager( const ZR::TransactionId & txId ) :
    m_TxId( txId ),
    m_Phase( INIT ),
    m_startOfPhase( QDateTime::currentMSecsSinceEpoch() )
{
    static const unsigned int defaultTimeOut = 3600000; // one hour

    std::cerr << "Zero Reserve: NEW TX Manager: " << m_TxId << std::endl;

    m_maxTime[ INIT ]          = defaultTimeOut;
    m_maxTime[ QUERY ]         = defaultTimeOut;
    m_maxTime[ VOTE_YES ]      = defaultTimeOut;
    m_maxTime[ VOTE_NO ]       = defaultTimeOut;
    m_maxTime[ COMMIT ]        = defaultTimeOut;
    m_maxTime[ ACK_COMMIT ]    = defaultTimeOut;
    m_maxTime[ ABORT ]         = defaultTimeOut;
    m_maxTime[ ABORT_REQUEST ] = defaultTimeOut;

    currentTX[ txId ] = this;
}

TransactionManager::~TransactionManager()
{
    std::cerr << "Zero Reserve: TX Manager: Cleaning up: " << m_TxId << std::endl;
    currentTX.erase( m_TxId );
}

bool TransactionManager::isTimedOut()
{
    return ( QDateTime::currentMSecsSinceEpoch() - m_startOfPhase > m_maxTime[ m_Phase ] );
}
