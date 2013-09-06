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
    std::cerr << "Zero Reserve: TX Manger handling incoming item - Destination: " << item->getAddress() << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    ZR::TransactionId txId = item->getAddress();
    ZR::PeerAddress addr = Router::Instance()->nextHop( txId );
    if( addr.empty() )
        return ZR::ZR_FAILURE;
    RSZRRemoteTxItem * resendItem = new RSZRRemoteTxItem( txId );
    resendItem->PeerId( addr );
    p3zr->sendItem( resendItem );
    return ZR::ZR_SUCCESS;
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
        RsZeroReserveInitTxItem * initItem = dynamic_cast< RsZeroReserveInitTxItem *> ( item );
        if( NULL == initItem )
            return ZR::ZR_FAILURE;
        tm = new TmLocalCohorte( initItem );
        tm->setTxId( txId );
        if( tm->init() == ZR::ZR_FAILURE ){
            delete tm;
            return ZR::ZR_FAILURE;
        }
        currentTX[ txId ] = tm;
    }
    tm = (*it).second;
    ZR::RetVal retVal = tm->processItem( item );
    if( retVal != ZR::ZR_SUCCESS ){
        currentTX.erase( txId );
        delete tm;
    }
    return retVal;
}



TransactionManager::TransactionManager() :
    m_Phase( INIT )
{
    // TODO: QTimer
}

TransactionManager::~TransactionManager()
{
    std::cerr << "Zero Reserve: TX Manager: Cleaning up." << std::endl;
}

