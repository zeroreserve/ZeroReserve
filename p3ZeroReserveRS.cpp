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


p3ZeroReserveRS::p3ZeroReserveRS( RsPluginHandler *pgHandler ) :
        RsPQIService( RS_SERVICE_TYPE_ZERORESERVE_PLUGIN, CONFIG_TYPE_ZERORESERVE_PLUGIN, 0, pgHandler )
{
}

int p3ZeroReserveRS::tick()
{
    std::cerr << "Zero Reserve: tick()" << std::endl;
    processIncoming();
    sendPackets();
    return 0;
}

void p3ZeroReserveRS::processIncoming()
{
    RsItem *item = NULL;
    while(NULL != (item = recvItem())){
        handleOrder( dynamic_cast<RsZeroReserveOrderBookItem*>( item ) );
        delete item;
    }
}

void p3ZeroReserveRS::sendPackets()
{

}


void p3ZeroReserveRS::handleOrder(RsZeroReserveOrderBookItem *item)
{

}

bool p3ZeroReserveRS::sendOrder( const std::string& peer_id, const OrderBook::Order * order )
{
    RsZeroReserveOrderBookItem * item = new RsZeroReserveOrderBookItem( order );
    if(!item){
            std::cerr << "Cannot allocate RsZeroReserveOrderBookItem !" << std::endl;
            return false ;
    }
    sendItem( item );
}
