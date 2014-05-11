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

#include "p3ZeroReserverRS.h"
#include "Credit.h"
#include "Payment.h"
#include "zrtypes.h"
#include "Router.h"
#include "ZeroReservePlugin.h"
#include "ZeroReserveDialog.h"
#include "MyOrders.h"
#include "BtcContract.h"
#include "Currency.h"

#include "pqi/p3linkmgr.h"

#include <iostream>

// after getting data from 3 peers, we believe we're complete
static const int INIT_THRESHOLD = 3;

p3ZeroReserveRS::p3ZeroReserveRS( RsPluginHandler *pgHandler, OrderBook * bids, OrderBook * asks, RsPeers* peers ) :
        RsPQIService( RS_SERVICE_TYPE_ZERORESERVE_PLUGIN, CONFIG_TYPE_ZERORESERVE_PLUGIN, 0, pgHandler ),
        m_bids(bids),
        m_asks(asks),
        m_peers(peers),
        m_initialized( 0 )
{
    addSerialType(new RsZeroReserveSerialiser());
    pgHandler->getLinkMgr()->addMonitor( this );
}


void p3ZeroReserveRS::statusChange(const std::list< pqipeer > &plist)
{
    std::cerr << "Zero Reserve: Status changed:" << std::endl;
    if( m_initialized < INIT_THRESHOLD ){
        for (std::list< pqipeer >::const_iterator it = plist.begin(); it != plist.end(); it++ ){
            if( RS_PEER_CONNECTED & (*it).actions ){
                RsZeroReserveMsgItem * item = new RsZeroReserveMsgItem( RsZeroReserveMsgItem::REQUEST_ORDERBOOK, "" );
                item->PeerId( (*it).id );
                sendItem( item );
            }
        }
    }
    // give any newly connected peer updated credit info - we might have changed it.
    for (std::list< pqipeer >::const_iterator peerIt = plist.begin(); peerIt != plist.end(); peerIt++ ){
        if( RS_PEER_CONNECTED & (*peerIt).actions ){
            Credit::CreditList cl;
            Credit::getCreditList( cl, (*peerIt).id );
            for( Credit::CreditList::const_iterator creditIt = cl.begin(); creditIt != cl.end(); creditIt++){
                sendCredit( *creditIt );
            }
        }
    }
}



int p3ZeroReserveRS::tick()
{
    processIncoming();
    janitor();
    return 0;
}

void p3ZeroReserveRS::janitor()
{
    static time_t lastCleanup = 0;
    time_t t = time( 0 );
    if( t - lastCleanup < 10 ) return;  // cleanup once in 10 sec
    lastCleanup = t;

    m_asks->timeoutOrders();
    m_bids->timeoutOrders();

    TransactionManager::timeout();
    BtcContract::pollContracts();

    MyOrders::Instance()->match();
}

void p3ZeroReserveRS::processIncoming()
{
    RsItem *item = NULL;
    while(NULL != (item = recvItem())){
        switch( item->PacketSubType() )
        {
        case RsZeroReserveItem::ZERORESERVE_ORDERBOOK_ITEM:
            handleOrder( dynamic_cast<RsZeroReserveOrderBookItem*>( item ) );
            break;
        case RsZeroReserveItem::ZERORESERVE_TX_INIT_ITEM:
        case RsZeroReserveItem::ZERORESERVE_TX_ITEM:
            TransactionManager::handleTxItem( dynamic_cast<RsZeroReserveTxItem*>( item ) );
            break;
        case RsZeroReserveItem::ZERORESERVE_CREDIT_ITEM:
            handleCredit( dynamic_cast<RsZeroReserveCreditItem*>( item ) );
            break;
        case RsZeroReserveItem::ZERORESERVE_MSG_ITEM:
            handleMessage( dynamic_cast<RsZeroReserveMsgItem*>( item ) );
            break;
        case RsZeroReserveItem::ZR_REMOTE_TX_ITEM:
            TransactionManager::handleTxItem( dynamic_cast<RSZRRemoteTxItem*>( item ) );
            break;
        default:
            std::cerr << "Zero Reserve: Received Item unknown" << std::endl;
        }
        delete item;
    }
}


void p3ZeroReserveRS::sendOrderBook( const std::string & uid )
{
    {
        RsStackMutex askMutex( m_asks->m_order_mutex );
        for( OrderBook::OrderIterator it = m_asks->begin(); it != m_asks->end(); it++ ){
            sendOrder( uid, *it );
        }
    }
    {
        RsStackMutex bidMutex( m_bids->m_order_mutex );
        for( OrderBook::OrderIterator it = m_bids->begin(); it != m_bids->end(); it++ ){
            sendOrder( uid, *it );
        }
    }
    sendItem( new RsZeroReserveMsgItem( RsZeroReserveMsgItem::SENT_ORDERBOOK, "" ) );
}


void p3ZeroReserveRS::handleMessage( RsZeroReserveMsgItem *item )
{
    std::cerr << "Zero Reserve: Received Message Item" << std::endl;
    RsZeroReserveMsgItem::MsgType msgType = item->getType();
    switch( msgType ){
    case RsZeroReserveMsgItem::REQUEST_ORDERBOOK:
        sendOrderBook( item->PeerId() );
        break;
    case RsZeroReserveMsgItem::SENT_ORDERBOOK:
        m_initialized++;
        break;
    default:
        std::cerr << "Zero Reserve: Received unknown message" << std::endl;
    }
}


void p3ZeroReserveRS::handleOrder(RsZeroReserveOrderBookItem *item)
{
    std::cerr << "Zero Reserve: Received Orderbook Item" << std::endl;
    ZR::RetVal result;
    item->print( std::cerr );
    OrderBook::Order * order = new OrderBook::Order( *( item->getOrder() ) );

    if( !Router::Instance()->hasRoute( order->m_order_id ) ){
        Router::Instance()->addRoute( order->m_order_id, item->PeerId() );
    }

    if( order->m_orderType == OrderBook::Order::ASK ){
        result = m_asks->processOrder( order );
    }
    else{
        result = m_bids->processOrder( order );
    }

    if( ZR::ZR_SUCCESS == result ){
        publishOrder( order, item );
    }
    else{
        delete order;
    }
}

void p3ZeroReserveRS::handleCredit(RsZeroReserveCreditItem *item)
{
    std::cerr << "Zero Reserve: Received Credit Item" << std::endl;
    Credit * otherCredit = item->getCredit();
    Credit ourCredit( otherCredit->m_id, otherCredit->m_currency );
    ourCredit.loadPeer();

    if( ourCredit.m_our_credit != otherCredit->m_our_credit ){
        otherCredit->updateOurCredit();
    }
    if( -ourCredit.m_balance != otherCredit->m_balance ){
        g_ZeroReservePlugin->placeMsg( std::string( "Different balance: " ) + rsPeers->getPeerName( otherCredit->m_id ) + " has " + otherCredit->m_balance.toDecimalStdString()
                     + " we have " + ourCredit.m_balance.toDecimalStdString() );
    }
}

bool p3ZeroReserveRS::sendCredit( Credit * credit )
{
    std::cerr << "Zero Reserve: Sending Credit item to " << credit->m_id << std::endl;
    RsZeroReserveCreditItem * item = new RsZeroReserveCreditItem( credit );
    if(!item){
            std::cerr << "Cannot allocate RsZeroReserveCreditItem !" << std::endl;
            return false ;
    }
    sendItem( item );
    item->print( std::cerr, 16 );
    return true;
}

bool p3ZeroReserveRS::sendOrder( const std::string& peer_id, OrderBook::Order * order )
{
    std::cerr << "Zero Reserve: Sending order to " << peer_id << std::endl;
    RsZeroReserveOrderBookItem * item = new RsZeroReserveOrderBookItem( *order );
    if(!item){
            std::cerr << "Cannot allocate RsZeroReserveOrderBookItem !" << std::endl;
            return false ;
    }
    item->PeerId( peer_id );
    sendItem( item );
    return true;
}


void p3ZeroReserveRS::publishOrder( OrderBook::Order * order, RsZeroReserveOrderBookItem * item )
{
    std::list< std::string > sendList;
    m_peers->getOnlineList(sendList);
    for(std::list< std::string >::const_iterator it = sendList.begin(); it != sendList.end(); it++ ){
        if( (*it) == getOwnId() ) continue;
        if( item && (*it) == item->PeerId() ) continue; // don't return to sender

        Credit c( *it, Currency::currencySymbols[ order->m_currency ] );
        c.loadPeer();
        // do not route orders we cannot at least fill to 10%
        if( order->m_isMyOrder == OrderBook::Order::ASK && ( order->m_purpose != OrderBook::Order::CANCEL || order->m_purpose != OrderBook::Order::FILLED ) ){
            if( c.getMyAvailable() < order->m_amount * 0.1 ) continue;
        }
        else{
            if( c.getPeerAvailable() < order->m_amount * 0.1 ) continue;
        }
        sendOrder( *it, order );
    }
}

