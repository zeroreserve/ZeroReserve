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
#include "MyOrders.h"


TmRemoteCohorte::TmRemoteCohorte( const ZR::TransactionId & txId ) :
    TransactionManager( txId ),
    m_PaymentReceiver( 0 ),
    m_PaymentSpender( 0 )
{
}

void TmRemoteCohorte::rollback()
{
    if( !m_IsHop )
        MyOrders::Instance()->rollback( dynamic_cast < PaymentReceiver *> ( m_PaymentReceiver ) );
}


ZR::RetVal TmRemoteCohorte::init()
{
    std::cerr << "Zero Reserve: TX Cohorte: Initializing... checking available funds" << std::endl;
    return ZR::ZR_SUCCESS;
}


ZR::RetVal TmRemoteCohorte::setup( RSZRRemoteTxInitItem * item )
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    m_PaymentReceiver = item->getPayment();
    m_Phase = QUERY;
    if( m_PaymentReceiver->getAmount() < ZR::ZR_Number(1, 1000000)){
        return abortTx( item );  // minimum is 1E-6 in any currency, in particular, this checks if amount < 0
    }

    Payment::Request req = Payment::getMyRequest( item->getAddress() );
    if( isPayee( item->getAddress() ) == ZR::ZR_SUCCESS ){   // we are the payee
        std::cerr << "Zero Reserve: TX Cohorte: Initializing payee role :: Amount: "
                  << m_PaymentReceiver->getAmount() << " " << m_PaymentReceiver->getCurrency() << std::endl;
        std::string payload = item->getPayload();
        TxPhase vote = ( m_PaymentReceiver->init( payload ) == ZR::ZR_FAILURE ) ? VOTE_NO : VOTE_YES;
        RSZRRemoteTxInitItem * resendItem = new RSZRRemoteTxInitItem( item->getAddress(), vote, Router::CLIENT, m_PaymentReceiver, item->getPayerId() );
        resendItem->setPayload( payload );
        m_IsHop = false;
        resendItem->PeerId( item->PeerId() );
        p3zr->sendItem( resendItem );
    }
    else{ // we are a hop
        m_IsHop = true;
        m_PaymentSpender = new PaymentSpender( Router::Instance()->nextHop( item->getAddress() ), m_PaymentReceiver->getAmount(), m_PaymentReceiver->getCurrency(), m_PaymentReceiver->getCategory() );
        forwardItem( item );
    }

    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmRemoteCohorte::isPayee( const ZR::VirtualAddress & addr )
{
    Payment::Request req = Payment::getMyRequest( addr );
    if( req.isValid() )
        return ZR::ZR_SUCCESS;

    if( MyOrders::Instance()->find( addr ) != MyOrders::Instance()->end() ){
        return ZR::ZR_SUCCESS;
    }
    return ZR::ZR_FAILURE;
}

ZR::RetVal TmRemoteCohorte::forwardItem( RSZRRemoteTxItem * item )
{
    std::cerr << "Zero Reserve: TX Cohorte: Passing on query" << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    RSZRRemoteTxInitItem * resendItem = new RSZRRemoteTxInitItem( item->getAddress() , item->getTxPhase(), item->getDirection(), m_PaymentReceiver, item->getPayerId() );
    resendItem->setPayload( item->getPayload() );
    ZR::PeerAddress nextAddr = Router::Instance()->nextHop( item->getAddress() );
    ZR::PeerAddress prevAddr = item->PeerId();
    std::pair< ZR::PeerAddress, ZR::PeerAddress > route( prevAddr, nextAddr );
    Router::Instance()->addTunnel( item->getAddress(), route );
    resendItem->PeerId( nextAddr );
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

        return ZR::ZR_FAILURE;
    }
    case VOTE_NO:
    case VOTE_YES:
    {
        std::cerr << "Zero Reserve: TX Cohorte: Received VOTE" << std::endl;
        RSZRRemoteTxInitItem * resendItem = new RSZRRemoteTxInitItem( item->getAddress(), item->getTxPhase(), item->getDirection(), m_PaymentReceiver, item->getPayerId() );
        std::pair< ZR::PeerAddress, ZR::PeerAddress > route;
        if( Router::Instance()->getTunnel( item->getAddress(), route ) == ZR::ZR_FAILURE )
            return ZR::ZR_FAILURE;
        resendItem->PeerId( route.first );
        p3zr->sendItem( resendItem );
        return ZR::ZR_SUCCESS;
    }
    case COMMIT:
        std::cerr << "Zero Reserve: TX Cohorte: Received Command: COMMIT" << std::endl;
        if( m_Phase != QUERY )
            return abortTx( item );
        m_Phase = COMMIT;
        if( m_IsHop ){
            return forwardItem( item );
        }
        else {
            std::cerr << "Zero Reserve: TX Cohorte: Payee committing" << std::endl;
            std::string payload = item->getPayload();
            m_PaymentReceiver->commit( m_TxId, payload );
            reply = new RSZRRemoteTxItem( item->getAddress(), ACK_COMMIT, Router::CLIENT, item->getPayerId() );
            reply->PeerId( item->PeerId() );
            p3zr->sendItem( reply );
            return ZR::ZR_FINISH;
        }
        break;
    case ACK_COMMIT: // we are always a hop
    {
        std::cerr << "Zero Reserve: TX Cohorte: Received ACK_COMMIT" << std::endl;
        if( m_Phase != COMMIT )
            return abortTx( item );
        reply = new RSZRRemoteTxItem( item->getAddress(), item->getTxPhase(), item->getDirection(), item->getPayerId() );
        std::pair< ZR::PeerAddress, ZR::PeerAddress > route;
        if( Router::Instance()->getTunnel( item->getAddress(), route ) == ZR::ZR_FAILURE )
            return ZR::ZR_FAILURE;
        reply->PeerId( route.first );
        m_PaymentReceiver->commit( m_TxId );
        m_PaymentSpender->commit( m_TxId );
        p3zr->sendItem( reply );
        return ZR::ZR_FINISH;
    }
    case ABORT:
    {
        std::cerr << "Zero Reserve: TX Cohorte: Received ABORT" << std::endl;
        if( item->getDirection() == Router::SERVER ){
            forwardItem( item );
            rollback();
            return ZR::ZR_FINISH;  // can die after passing on the message
        }
        else {
            return abortTx( item );
        }
    }
    default:
        throw std::runtime_error( "Unknown Transaction Phase");
    }
}

ZR::RetVal TmRemoteCohorte::abortTx(RSZRRemoteTxItem *item )
{
    std::cerr << "Zero Reserve: TX Cohorte: Sending ABORT" << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    RSZRRemoteTxItem * abortItem = new RSZRRemoteTxItem( item->getAddress(), ABORT, Router::CLIENT, item->getPayerId() );
    std::pair< ZR::PeerAddress, ZR::PeerAddress > route;
    if( Router::Instance()->getTunnel( item->getAddress(), route ) == ZR::ZR_FAILURE )
        return ZR::ZR_FAILURE;
    abortItem->PeerId( route.first );
    p3zr->sendItem( abortItem );
    return ZR::ZR_SUCCESS;  // only requesting ABORT from coordinator here. Need to stay alive
}
