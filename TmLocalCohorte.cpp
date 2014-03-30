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


#include "TmLocalCohorte.h"
#include "RSZeroReserveItems.h"
#include "Payment.h"
#include "p3ZeroReserverRS.h"
#include "ZeroReservePlugin.h"


TmLocalCohorte::TmLocalCohorte( const ZR::TransactionId & txId ) :
    TransactionManager( txId )
{
}

TmLocalCohorte::~TmLocalCohorte()
{
    delete m_payment;
}


void TmLocalCohorte::rollback()
{

}




ZR::RetVal TmLocalCohorte::init()
{

    std::cerr << "Zero Reserve: TX Manager: Payment request for " << m_payment->getAmount() << " "
              << m_payment->getCurrency()
              << " received - Setting TX manager up as cohorte" << std::endl;

    RsZeroReserveTxItem * reply;
    ZR::RetVal retval;

    std::string dummy;
    if ( m_payment->init( dummy ) == ZR::ZR_FAILURE ){
        std::cerr << "Zero Reserve: initCohort(): Insufficient Credit - voting no" << std::endl;
        reply = new RsZeroReserveTxItem( VOTE_NO );
        retval = ZR::ZR_FAILURE;
    }
    else {
        reply = new RsZeroReserveTxItem( VOTE_YES );
        retval = ZR::ZR_SUCCESS;
    }

    reply->PeerId( m_payment->getCounterparty() );
    reply->setTxId( m_TxId );

    p3ZeroReserveRS * p3zs = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    p3zs->sendItem( reply ); // TODO: error  handling
    return retval;
}





ZR::RetVal TmLocalCohorte::processItem( RsZeroReserveTxItem * item )
{
    RsZeroReserveTxItem * reply;
    p3ZeroReserveRS * p3zs;

    // TODO: Timeout
    switch( item->getTxPhase() )
    {
    case QUERY:
    {
        RsZeroReserveInitTxItem * initItem = dynamic_cast< RsZeroReserveInitTxItem *> ( item );
        if( m_Phase != INIT || initItem == NULL )
            return abortTx( item );
        m_Phase = QUERY;
        m_payment = initItem->getPayment();
        return init();
    }
    case COMMIT:
        std::cerr << "Zero Reserve: TX Cohorte: Received Command: COMMIT" << std::endl;
        if( m_Phase != QUERY )
            return abortTx( item );
        m_Phase = COMMIT;
        reply = new RsZeroReserveTxItem( ACK_COMMIT );
        reply->PeerId( m_payment->getCounterparty() );
        reply->setTxId( m_TxId );
        p3zs = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
        p3zs->sendItem( reply );
        m_payment->commit( m_TxId );
        return ZR::ZR_FINISH;
    case ABORT:
        return ZR::ZR_FINISH;
    default:
        throw std::runtime_error( "Unknown Transaction Phase");
    }
    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmLocalCohorte::abortTx( RsZeroReserveTxItem * item )
{
     std::cerr << "Zero Reserve: TX Manger:Error happened. Aborting." << std::endl;
     RsZeroReserveTxItem * reply = new RsZeroReserveTxItem( ABORT );
     p3ZeroReserveRS * p3zs = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
     reply->PeerId( m_payment->getCounterparty() );
     reply->setTxId( m_TxId );
     p3zs->sendItem( reply );
     return ZR::ZR_FAILURE;
}



