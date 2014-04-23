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


#include "TmContract.h"
#include "BtcContract.h"
#include "p3ZeroReserverRS.h"
#include "ZeroReservePlugin.h"
#include "Router.h"
#include "RSZRRemoteItems.h"
#include "OrderBook.h"
#include "MyOrders.h"
#include "ZRBitcoin.h"



TmContract::TmContract( const ZR::VirtualAddress & addr, const std::string & myId ) :
    TransactionManager( addr + ':' + myId )
{
}


///////////////////// TmContractCoordinator /////////////////////////////

TmContractCoordinator::TmContractCoordinator( const OrderBook::Order * order, const ZR::ZR_Number & amount, const std::string & myId ) :
    TmContract( order->m_order_id , myId ),
    m_Destination( order->m_order_id ),
    m_myId( myId ),
    m_payer( NULL )
{
    ZR::PeerAddress addr = Router::Instance()->nextHop( m_Destination );
    if( !addr.empty() ){
        m_payer = new BtcContract( amount, order->m_price, Currency::currencySymbols[ order->m_currency ], BtcContract::SENDER, addr );
    }
}


ZR::RetVal TmContractCoordinator::init()
{
    std::cerr << "Zero Reserve: Setting Contract TX manager up as coordinator" << std::endl;
    if( m_payer == NULL) return ZR::ZR_FAILURE;

    ZR::BitcoinAddress btcAddr = ZR::Bitcoin::Instance()->newAddress();
    if( btcAddr.empty() ){
        std::cerr << "Zero Reserve: ERROR getting Bitcoin Address" << std::endl;
        return ZR::ZR_FAILURE;
    }
    m_payer->setBtcAddress( btcAddr );

    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    RSZRRemoteTxItem * item = new RSZRRemoteTxItem( m_Destination, QUERY, Router::SERVER, m_myId );
    std::string payload = m_payer->getFiatAmount().toStdString() + ':' + m_payer->getCurrencySym() + ':' +
            btcAddr + ':' + m_payer->getBtcAmount().toStdString();
    item->setPayload( payload );
    item->PeerId( m_payer->getCounterParty() );
    p3zr->sendItem( item );
    return ZR::ZR_SUCCESS;
}


ZR::RetVal TmContractCoordinator::processItem( RSZRRemoteTxItem * item )
{
    switch( item->getTxPhase() )
    {
    case VOTE_NO:
        rollback();
        return abortTx( item );
    case VOTE_YES:
        return doTx( item );
    case ABORT_REQUEST:
        rollback();
        return abortTx( item );
    default:
        throw std::runtime_error( "Unknown Transaction Phase");
    }
}

ZR::RetVal TmContractCoordinator::doTx( RSZRRemoteTxItem *item )
{
    // TODO: Store raw tx / id and check if destination addr is what we want and matches with the amount we get back
    std::cerr << "Zero Reserve: Payload: " << item->getPayload() << std::endl;

    std::vector< std::string > v_payload;
    split( item->getPayload(), v_payload );
    if( v_payload.size() != 2 ){
        std::cerr << "Zero Reserve: Payload ERROR: Protocol mismatch" << std::endl;
        return abortTx( item );
    }
    ZR::TransactionId btcTxId = v_payload[ 1 ];
    m_payer->setBtcTxId( btcTxId );

    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    RSZRRemoteTxItem * resendItem = new RSZRRemoteTxItem( m_Destination, COMMIT, Router::SERVER, m_myId );
    resendItem->setPayload( "" );
    resendItem->PeerId( m_payer->getCounterParty() );
    m_payer->activate();
    m_payer->persist();
    MyOrders::Instance()->updateOrders( m_payer->getBtcAmount(), m_TxId );

    p3zr->sendItem( resendItem );

    return ZR::ZR_FINISH;
}

void TmContractCoordinator::rollback()
{
    BtcContract::rmContract( m_payer );

    MyOrders::Instance()->rollback( m_TxId );
}


ZR::RetVal TmContractCoordinator::abortTx( RSZRRemoteTxItem *item )
{
    std::cerr << "Zero Reserve: TmContractCoordinator: Commanding ABORT of " << m_TxId << std::endl;

    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    RSZRRemoteTxItem * resendItem = new RSZRRemoteTxItem( m_Destination, ABORT, Router::SERVER, m_myId );
    ZR::PeerAddress addr = Router::Instance()->nextHop( m_Destination );
    if( addr.empty() )
        return ZR::ZR_FAILURE;

    resendItem->PeerId( addr );
    p3zr->sendItem( resendItem );
    return ZR::ZR_FAILURE;
}

///////////////////// TmContractCohortePayee /////////////////////////////


TmContractCohortePayee::TmContractCohortePayee( const ZR::VirtualAddress & addr, const std::string & myId ) :
    TmContract( addr, myId ),
    m_payee( NULL )
{

}


ZR::RetVal TmContractCohortePayee::init()
{
    std::cerr << "Zero Reserve: Setting Contract TX manager up as cohorte Payee" << std::endl;
    return ZR::ZR_SUCCESS;
}


ZR::RetVal TmContractCohortePayee::doQuery( RSZRRemoteTxItem * item )
{
    std::cerr << "Zero Reserve: TmContractCohortePayee: Received QUERY" << std::endl;

    if( m_Phase != INIT || item == NULL )
        return abortTx( item );
    m_Phase = QUERY;

    std::vector< std::string > v_payload;
    split( item->getPayload(), v_payload );
    if( v_payload.size() != 4 ){
        std::cerr << "Zero Reserve: Payload ERROR: Protocol mismatch" << std::endl;
        return abortTx( item );
    }

    ZR::ZR_Number fiatAmount = ZR::ZR_Number::fromFractionString( v_payload[0] );
    std::string currencySym = v_payload[ 1 ];
    ZR::BitcoinAddress destinationBtcAddr = v_payload[ 2 ];
    ZR::ZR_Number btcAmount = ZR::ZR_Number::fromFractionString( v_payload[3] );

    std::string outTxId;
    OrderBook::Order * order = MyOrders::Instance()->startExecute( fiatAmount, item->getAddress(), destinationBtcAddr, m_txHex, outTxId );

    if( order == NULL ) return abortTx( item );
    if( Currency::currencySymbols[ order->m_currency ] != currencySym ) return abortTx( item );

    m_payee = new BtcContract( fiatAmount / order->m_price, order->m_price, currencySym, BtcContract::RECEIVER, item->PeerId() );
    m_payee->setBtcAddress( destinationBtcAddr );
    m_payee->setBtcTxId( outTxId );

    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    TxPhase vote = VOTE_YES; // TODO: A vote is not limited to saying YES!!!
    RSZRRemoteTxItem * resendItem = new RSZRRemoteTxItem( item->getAddress(), vote, Router::CLIENT, item->getPayerId() );

    // return the final Bitcoin amount and the TX ID of the signed TX to the Hops and the payers. They already have the receiving address.
    resendItem->setPayload( m_payee->getBtcAmount().toStdString() + ':' + outTxId );
    resendItem->PeerId( item->PeerId() );
    p3zr->sendItem( resendItem );

    return ZR::ZR_SUCCESS;
}


ZR::RetVal TmContractCohortePayee::doCommit( RSZRRemoteTxItem * item )
{
    std::cerr << "Zero Reserve: TmContractCohortePayee: Received COMMIT for " << m_TxId << std::endl;

    if( m_Phase != QUERY || item == NULL )
        return abortTx( item );
    m_Phase = COMMIT;

    m_payee->activate();
    m_payee->persist();
    MyOrders::Instance()->finishExecute( item->getAddress(), m_payee->getBtcAmount(), m_txHex );

    return ZR::ZR_FINISH;
}


ZR::RetVal TmContractCohortePayee::processItem( RSZRRemoteTxItem * item )
{
    switch( item->getTxPhase() )
    {
    case QUERY:
        return doQuery( item );
    case COMMIT:
        return doCommit( item );
    case ABORT:
        rollback();
        return ZR::ZR_FAILURE;
    default:
        throw std::runtime_error( "Unknown Transaction Phase");
    }
}


ZR::RetVal TmContractCohortePayee::abortTx( RSZRRemoteTxItem *item )
{
    std::cerr << "Zero Reserve: TmContractCohortePayee: Requesting ABORT for " << m_TxId << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    RSZRRemoteTxItem * resendItem = new RSZRRemoteTxItem( item->getAddress(), ABORT_REQUEST, Router::CLIENT, item->getPayerId() );
    resendItem->PeerId( item->PeerId() );
    p3zr->sendItem( resendItem );
    return ZR::ZR_SUCCESS;
}

void TmContractCohortePayee::rollback()
{
    BtcContract::rmContract( m_payee );
}


///////////////////// TmContractCohorteHop /////////////////////////////


TmContractCohorteHop::TmContractCohorteHop( const ZR::VirtualAddress & addr, const std::string & myId ) :
    TmContract( addr, myId ),
    m_payer( NULL ),
    m_payee( NULL )
{

}


ZR::RetVal TmContractCohorteHop::init()
{
    std::cerr << "Zero Reserve: Setting Contract TX manager up as cohorte Hop" << std::endl;
    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmContractCohorteHop::doQuery( RSZRRemoteTxItem * item )
{
    std::cerr << "Zero Reserve: TX Cohorte: Passing on QUERY" << std::endl;
    std::cerr << "Zero Reserve: Payload: " << item->getPayload() << std::endl;

    if( m_Phase != INIT || item == NULL )
        return abortTx( item );
    m_Phase = QUERY;

    mkTunnel( item );

    std::vector< std::string > v_payload;
    split( item->getPayload(), v_payload );
    if( v_payload.size() != 4 ){
        std::cerr << "Zero Reserve: Payload ERROR: Protocol mismatch" << std::endl;
        return abortTx( item );
    }
    ZR::ZR_Number fiatAmount = ZR::ZR_Number::fromFractionString( v_payload[0] );
    std::string currencySym = v_payload[ 1 ];
    ZR::BitcoinAddress destinationBtcAddr = v_payload[ 2 ];
    ZR::ZR_Number btcAmount = ZR::ZR_Number::fromFractionString( v_payload[3] );
    ZR::ZR_Number price = fiatAmount / btcAmount;

    // TODO: Check if fiat amount is within limits. If not, adjust. If 0, abort()

    std::pair< ZR::PeerAddress, ZR::PeerAddress > route;
    if( Router::Instance()->getTunnel( item->getAddress(), route ) == ZR::ZR_FAILURE )
        return ZR::ZR_FAILURE;
    m_payee = new BtcContract( btcAmount, price, currencySym, BtcContract::RECEIVER, route.first );
    m_payer = new BtcContract( btcAmount, price, currencySym, BtcContract::SENDER, route.second );

    m_payee->setBtcAddress( destinationBtcAddr );
    m_payer->setBtcAddress( destinationBtcAddr );

    forwardItem( item );
    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmContractCohorteHop::doCommit( RSZRRemoteTxItem * item )
{
    std::cerr << "Zero Reserve: TX Cohorte: Passing on COMMIT" << std::endl;

    if( m_Phase != VOTE_YES || item == NULL )
        return abortTx( item );
    m_Phase = COMMIT;

    m_payer->activate();
    m_payee->activate();
    m_payer->persist();
    m_payee->persist();

    forwardItem( item );
    return ZR::ZR_FINISH;
}

ZR::RetVal TmContractCohorteHop::doVote( RSZRRemoteTxItem * item )
{
    std::cerr << "Zero Reserve: TX Cohorte: Passing on VOTE" << std::endl;
    std::cerr << "Zero Reserve: Payload: " << item->getPayload() << std::endl;

    if( m_Phase != QUERY || item == NULL )
        return abortTx( item );
    m_Phase = item->getTxPhase();

    std::vector< std::string > v_payload;
    split( item->getPayload(), v_payload );
    if( v_payload.size() != 2 ){
        std::cerr << "Zero Reserve: Payload ERROR: Protocol mismatch" << std::endl;
        return abortTx( item );
    }
    ZR::TransactionId btcTxId = v_payload[ 1 ];
    m_payer->setBtcTxId( btcTxId );
    m_payee->setBtcTxId( btcTxId );
    // TODO: set amount

    forwardItem( item );
    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmContractCohorteHop::processItem( RSZRRemoteTxItem * item )
{
    switch( item->getTxPhase() )
    {
    case QUERY:
        return doQuery( item );
    case VOTE_NO:
    case VOTE_YES:
        return doVote( item );
    case COMMIT:
        return doCommit( item );
    case ABORT:
        forwardItem( item );
        rollback();
        return ZR::ZR_FAILURE;
    case ABORT_REQUEST:
        return forwardItem( item );
    default:
        throw std::runtime_error( "Unknown Transaction Phase");
    }
}


void TmContractCohorteHop::rollback()
{
    BtcContract::rmContract( m_payer );
    BtcContract::rmContract( m_payee );
}

// TODO: Should this be stored in the transaction so it would go away when the TX is finished?
void TmContractCohorteHop::mkTunnel( RSZRRemoteTxItem * item )
{
    ZR::PeerAddress nextAddr = Router::Instance()->nextHop( item->getAddress() );
    ZR::PeerAddress prevAddr = item->PeerId();
    std::pair< ZR::PeerAddress, ZR::PeerAddress > route( prevAddr, nextAddr );
    Router::Instance()->addTunnel( item->getAddress(), route );
}


ZR::RetVal TmContractCohorteHop::forwardItem( RSZRRemoteTxItem * item )
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    RSZRRemoteTxItem * resendItem = new RSZRRemoteTxItem( item->getAddress() , item->getTxPhase(), item->getDirection(), item->getPayerId() );
    resendItem->setPayload( item->getPayload() );

    std::pair< ZR::PeerAddress, ZR::PeerAddress > route;
    if( Router::Instance()->getTunnel( item->getAddress(), route ) == ZR::ZR_FAILURE )
        return ZR::ZR_FAILURE;
    if( item->getDirection() == Router::SERVER )
        resendItem->PeerId( route.second );
    else
        resendItem->PeerId( route.first );

    p3zr->sendItem( resendItem );
    return ZR::ZR_SUCCESS;
}


ZR::RetVal TmContractCohorteHop::abortTx( RSZRRemoteTxItem *item )
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    RSZRRemoteTxItem * resendItem = new RSZRRemoteTxItem( item->getAddress(), ABORT_REQUEST, Router::CLIENT, item->getPayerId() );
    resendItem->PeerId( item->PeerId() );
    p3zr->sendItem( resendItem );
}
