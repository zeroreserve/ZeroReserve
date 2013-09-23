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


#include "TmLocalCoordinator.h"
#include "p3ZeroReserverRS.h"
#include "ZeroReservePlugin.h"
#include "Payment.h"


unsigned int TmLocalCoordinator::sequence = 1;

const ZR::TransactionId TmLocalCoordinator::mkId()
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );

    std::ostringstream txId;
    txId << p3zr->getOwnId() << ":" << ++sequence;
    return txId.str();
}

TmLocalCoordinator::TmLocalCoordinator( Payment *payment ) :
    TransactionManager( mkId() ),
    m_payment( payment )
{
}

TmLocalCoordinator::~TmLocalCoordinator()
{
    delete m_payment;
}



void TmLocalCoordinator::rollback()
{

}

bool TmLocalCoordinator::isTimedOut()
{
   return false;
}



ZR::RetVal TmLocalCoordinator::init()
{
    std::cerr << "Zero Reserve: Setting TX manager up as coordinator. ID: " << m_TxId << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    if ( m_payment->init() == ZR::ZR_FAILURE ){
        std::cerr << "Zero Reserve: Error, not enough Credit " << std::endl;
        return ZR::ZR_FAILURE;
    }
    RsZeroReserveInitTxItem * initItem = new RsZeroReserveInitTxItem( m_payment );
    initItem->setTxId( m_TxId );
    p3zr->sendItem( initItem );
    return ZR::ZR_SUCCESS;
}


ZR::RetVal TmLocalCoordinator::processItem( RsZeroReserveTxItem * item )
{
    RsZeroReserveTxItem * reply;
    p3ZeroReserveRS * p3zr;

    // TODO: Timeout
    switch( item->getTxPhase() )
    {
    case VOTE_YES:
        std::cerr << "Zero Reserve: TX Coordinator: Received Vote: YES" << std::endl;
        reply = new RsZeroReserveTxItem( COMMIT );
        reply->PeerId( m_payment->getCounterparty() );
        reply->setTxId( m_TxId );
        p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
        p3zr->sendItem( reply );
        return ZR::ZR_SUCCESS;
    case VOTE_NO:
        return abortTx( item );
    case ACK_COMMIT:
        std::cerr << "Zero Reserve: TX Coordinator: Received Acknowledgement, Committing" << std::endl;
        m_payment->commit( m_TxId );
        return ZR::ZR_FINISH;
    case ABORT:
        return abortTx( item );
    default:
        throw std::runtime_error( "Unknown Transaction Phase");
    }
    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmLocalCoordinator::abortTx( RsZeroReserveTxItem * item )
{
    std::cerr << "Zero Reserve: TX Manger:Error happened. Aborting." << std::endl;
    return ZR::ZR_FAILURE;
}



