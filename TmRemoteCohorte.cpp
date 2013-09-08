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

TmRemoteCohorte::TmRemoteCohorte( const ZR::TransactionId & txId ) :
    TransactionManager( txId )
{
}

ZR::RetVal TmRemoteCohorte::init()
{
    return ZR::ZR_SUCCESS;
}


ZR::RetVal TmRemoteCohorte::processItem( RSZRRemoteTxItem * item )
{
    RSZRRemoteTxItem * reply;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
;
    ZR::PeerAddress addr = Router::Instance()->nextHop( m_TxId );
    if( addr.empty() )
        return ZR::ZR_FAILURE;

    // TODO: Timeout
    switch( item->getTxPhase() )
    {
    case QUERY:
    {
        RsZeroReserveInitTxItem * initItem = dynamic_cast< RsZeroReserveInitTxItem *> ( item );
        if( m_Phase != INIT || initItem == NULL )
            return abortTx( item );
        m_Phase = QUERY;
        RSZRRemoteTxItem * resendItem = new RSZRRemoteTxItem( m_TxId, VOTE_YES );
        resendItem->PeerId( addr );
        p3zr->sendItem( resendItem );
        return init();
    }
    case COMMIT:
        std::cerr << "Zero Reserve: TX Cohorte: Received Command: COMMIT" << std::endl;
        if( m_Phase != QUERY )
            return abortTx( item );
        m_Phase = COMMIT;
        reply = new RSZRRemoteTxItem( m_TxId, ACK_COMMIT );
        reply->PeerId( addr );
        p3zr->sendItem( reply );
        return ZR::ZR_FINISH;
    case ABORT:
        return ZR::ZR_FINISH;
    default:
        throw std::runtime_error( "Unknown Transaction Phase");
    }
    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmRemoteCohorte::abortTx(RSZRRemoteTxItem *item )
{

}
