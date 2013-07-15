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
#include <iostream>


p3ZeroReserveRS::p3ZeroReserveRS( RsPluginHandler *pgHandler, OrderBook * bids, OrderBook * asks ) :
        RsPQIService( RS_SERVICE_TYPE_ZERORESERVE_PLUGIN, CONFIG_TYPE_ZERORESERVE_PLUGIN, 0, pgHandler ),
        m_bids(bids),
        m_asks(asks)
{
    addSerialType(new RsZeroReserveSerialiser());
}

int p3ZeroReserveRS::tick()
{
    processIncoming();
    return 0;
}

void p3ZeroReserveRS::processIncoming()
{
    RsItem *item = NULL;
    while(NULL != (item = recvItem())){
        switch( item->PacketSubType() )
        {
        case RS_PKT_SUBTYPE_ZERORESERVE_ORDERBOOKITEM:
            handleOrder( dynamic_cast<RsZeroReserveOrderBookItem*>( item ) );
            break;
        default:
            std::cerr << "Zero Reserve: Received Item unknown" << std::endl;
        }
        delete item;
    }
}


void p3ZeroReserveRS::handleOrder(RsZeroReserveOrderBookItem *item)
{
    std::cerr << "Zero Reserve: Received Item" << std::endl;
    OrderBook::Order * order = item->getOrder();
    if( order->m_orderType == OrderBook::Order::ASK ){
        m_asks->addOrder( order );
    }
    else{
        m_bids->addOrder( order );
    }
}

bool p3ZeroReserveRS::sendOrder( const std::string& peer_id, OrderBook::Order * order )
{
    std::cerr << "Zero Reserve: Sending order to " << peer_id << std::endl;
    RsZeroReserveOrderBookItem * item = new RsZeroReserveOrderBookItem( order );
    if(!item){
            std::cerr << "Cannot allocate RsZeroReserveOrderBookItem !" << std::endl;
            return false ;
    }
    item->PeerId( peer_id );
    sendItem( item );
}
