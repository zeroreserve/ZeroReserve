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


TmContract::TmContract( const ZR::VirtualAddress & addr, const std::string & myId ) :
    TransactionManager( addr + ':' + myId )
{
}


///////////////////// TmContractCoordinator /////////////////////////////

TmContractCoordinator::TmContractCoordinator( const OrderBook::Order * order, const ZR::ZR_Number & amount, const std::string & myId ) :
    TmContract( order->m_order_id , myId ),
    m_Destination( order->m_order_id ),
    m_myId( myId )
{

}


ZR::RetVal TmContractCoordinator::init()
{
    std::cerr << "Zero Reserve: Setting Contract TX manager up as coordinator" << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    RSZRRemoteTxItem * item = new RSZRRemoteTxItem( m_Destination, QUERY, Router::SERVER, m_myId );
    item->setPayload( "Staat heisst das kälteste aller kalten Ungeheuer" );

    ZR::PeerAddress addr = Router::Instance()->nextHop( m_Destination );
    if( addr.empty() )
        return ZR::ZR_FAILURE;

    item->PeerId( addr );
    p3zr->sendItem( item );
    return ZR::ZR_SUCCESS;
}


ZR::RetVal TmContractCoordinator::processItem( RSZRRemoteTxItem * item )
{
    switch( item->getTxPhase() )
    {
    case VOTE_NO:
        return abortTx( item );
    case VOTE_YES:
        return doTx( item );
    default:
        throw std::runtime_error( "Unknown Transaction Phase");
    }
}

ZR::RetVal TmContractCoordinator::doTx( RSZRRemoteTxItem *item )
{

}

void TmContractCoordinator::rollback()
{

}


ZR::RetVal TmContractCoordinator::abortTx( RSZRRemoteTxItem *item )
{

}

///////////////////// TmContractCohortePayee /////////////////////////////


TmContractCohortePayee::TmContractCohortePayee( const ZR::VirtualAddress & addr, const std::string & myId ) :
    TmContract( addr, myId )
{

}


ZR::RetVal TmContractCohortePayee::init()
{
    std::cerr << "Zero Reserve: Setting Contract TX manager up as cohorte Payee" << std::endl;

    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmContractCohortePayee::doQuery( RSZRRemoteTxItem * item )
{
    std::cerr << "Zero Reserve: TmContractCohortePayee: Received Query" << std::endl;
    std::cerr << "Zero Reserve: Payload: " << item->getPayload() << std::endl;

    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmContractCohortePayee::doCommit( RSZRRemoteTxItem * item )
{
    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmContractCohortePayee::processItem( RSZRRemoteTxItem * item )
{
    switch( item->getTxPhase() )
    {
    case QUERY:
        return doQuery( item );
    case COMMIT:
        return doCommit( item );
    default:
        throw std::runtime_error( "Unknown Transaction Phase");
    }
}

void TmContractCohortePayee::rollback()
{

}


///////////////////// TmContractCohorteHop /////////////////////////////


TmContractCohorteHop::TmContractCohorteHop( const ZR::VirtualAddress & addr, const std::string & myId ) :
    TmContract( addr, myId )
{

}


ZR::RetVal TmContractCohorteHop::init()
{
    std::cerr << "Zero Reserve: Setting Contract TX manager up as cohorte Hop" << std::endl;

    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmContractCohorteHop::doQuery( RSZRRemoteTxItem * item )
{
    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmContractCohorteHop::doCommit( RSZRRemoteTxItem * item )
{
    return ZR::ZR_SUCCESS;
}

ZR::RetVal TmContractCohorteHop::doVote( RSZRRemoteTxItem * item )
{
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
    default:
        throw std::runtime_error( "Unknown Transaction Phase");
    }
}

void TmContractCohorteHop::rollback()
{

}

