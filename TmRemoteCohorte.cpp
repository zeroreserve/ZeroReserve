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

#include "TmRemoteCohorte.h"
#include "p3ZeroReserverRS.h"
#include "ZeroReservePlugin.h"
#include "Router.h"
#include "Payment.h"


TmRemoteCohorte::TmRemoteCohorte( const ZR::TransactionId & txId ) :
    TransactionManager( txId )
{
}

ZR::RetVal TmRemoteCohorte::init()
{  
    return ZR::ZR_SUCCESS;
}


ZR::RetVal TmRemoteCohorte::setup( RSZRRemoteTxInitItem * item )
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    RSZRRemoteTxInitItem * resendItem = new RSZRRemoteTxInitItem( m_TxId, VOTE_YES, Router::CLIENT );

    Payment::Request req = Payment::getMyRequest( item->getAddress() );
    if( req.isValid() ){   // we are the payee
        m_Phase = QUERY;
        m_IsHop = false;
        resendItem->PeerId( item->PeerId() );
    }
    else{ // we are a hop
        m_IsHop = true;
        ZR::PeerAddress nextAddr = Router::Instance()->nextHop( m_TxId );
        ZR::PeerAddress prevAddr = item->PeerId();
        std::pair< ZR::PeerAddress, ZR::PeerAddress > route( prevAddr, nextAddr );
        Router::Instance()->addTunnel( item->getAddress(), route );
        resendItem->PeerId( nextAddr );
    }

    p3zr->sendItem( resendItem );
    return ZR::ZR_SUCCESS;
}


ZR::RetVal TmRemoteCohorte::processItem( RSZRRemoteTxItem * item )
{
    RSZRRemoteTxItem * reply;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );

    // TODO: Timeout
    switch( item->getTxPhase() )
    {
    case QUERY:
    {
        RSZRRemoteTxInitItem * initItem = dynamic_cast< RSZRRemoteTxInitItem *> ( item );
        if( m_Phase != INIT || initItem == NULL )
            return abortTx( item );
        if( item->getDirection() == Router::SERVER )
            return setup( initItem );

        // process the return item
        m_Phase = QUERY;
        init();
//        RsZeroReserveInitTxItem * resendItem = new RsZeroReserveInitTxItem( *initItem );
//        resendItem->PeerId( addr );
//        p3zr->sendItem( resendItem );
        return ZR::ZR_SUCCESS;

    }
    case COMMIT:
        std::cerr << "Zero Reserve: TX Cohorte: Received Command: COMMIT" << std::endl;
        if( m_Phase != QUERY )
            return abortTx( item );
        m_Phase = COMMIT;
        reply = new RSZRRemoteTxItem( m_TxId, ACK_COMMIT, Router::SERVER );
//        reply->PeerId( addr );
        p3zr->sendItem( reply );
        return ZR::ZR_FINISH;
    case ABORT:
        return ZR::ZR_FINISH;
    default:
        throw std::runtime_error( "Unknown Transaction Phase");
    }
}

ZR::RetVal TmRemoteCohorte::abortTx(RSZRRemoteTxItem *item )
{
    return ZR::ZR_FAILURE;
}
