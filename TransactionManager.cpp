/*!
 * \file TransactionManager.cpp
 * 
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
#include "TmRemoteCohorte.h"
#include "RSZeroReserveItems.h"
#include "RSZRRemoteItems.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"
#include "Payment.h"
#include "zrtypes.h"
#include "Router.h"

#include <stdexcept>
#include <sstream>



TransactionManager::TxManagers TransactionManager::currentTX;


/**
 * @brief Handle remote Transaction Items
 * @param item
 * @return
 */

int TransactionManager::handleTxItem( RSZRRemoteTxItem * item )
{
    ZR::RetVal retVal;
    std::cerr << "Zero Reserve: TX Manger handling incoming item - Destination: " << item->getAddress() << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    ZR::TransactionId txId = item->getAddress() + ":" + item->getPayerId();
    std::cerr << "\n\nXXXXXXXXXXXXXXXX     " << txId << std::endl;
    TransactionManager * tm;
    TxManagers::iterator it = currentTX.find( txId );
    if( it == currentTX.end() )
    {
        tm = new TmRemoteCohorte( txId );
    }
    else 
    {
        tm = (*it).second;
    }
    try{
        retVal = tm->processItem( item );
    }
    catch( std::runtime_error e){
        std::cerr << "Zero Reserve: Exception caught: " << e.what() << std::endl;
        return ZR::ZR_FAILURE;
    }

    if( retVal != ZR::ZR_SUCCESS )
    {
        delete tm;
    }
    return retVal;
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
    if( it == currentTX.end() )
    {
        tm = new TmLocalCohorte( txId );
    }
    else 
    {
        tm = (*it).second;
    }
    ZR::RetVal retVal = tm->processItem( item );
    if( retVal != ZR::ZR_SUCCESS )
    {
        delete tm;
    }
    return retVal;
}


/**
 * @brief Timeout
 * 
 */
void TransactionManager::timeout()
{
    for( TxManagers::iterator it = currentTX.begin(); it != currentTX.end(); it++ )
    {
        TransactionManager * tm = (*it).second;
        if( tm->isTimedOut() )
        {
            tm->rollback();
            delete tm;
        }
    }
}


/**
 * @brief Transaction manager for a transaction id
 *
 * @param txId - transaction id
 * 
 */
TransactionManager::TransactionManager( const ZR::TransactionId & txId ) :
    m_TxId( txId ),
    m_Phase( INIT ),
    m_startOfPhase( QDateTime::currentMSecsSinceEpoch() )
{
    m_maxTime[ INIT ]       = 5000;
    m_maxTime[ QUERY ]      = 5000;
    m_maxTime[ VOTE_YES ]   = 5000;
    m_maxTime[ VOTE_NO ]    = 5000;
    m_maxTime[ COMMIT ]     = 5000;
    m_maxTime[ ACK_COMMIT ] = 5000;
    m_maxTime[ ABORT ]      = 5000;

    currentTX[ txId ] = this;
}

/**
 * @brief Destructor
 */
TransactionManager::~TransactionManager()
{
    std::cerr << "Zero Reserve: TX Manager: Cleaning up." << std::endl;
    currentTX.erase( m_TxId );
}

/**
 * @brief is timeout?
 * 
 *
 * @return 
 */
bool TransactionManager::isTimedOut()
{
    return ( QDateTime::currentMSecsSinceEpoch() - m_startOfPhase > m_maxTime[ m_Phase ] );
}


// EOF   
