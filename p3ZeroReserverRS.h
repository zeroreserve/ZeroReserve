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

#ifndef P3ZERORESERVERRS_H
#define P3ZERORESERVERRS_H

#include "plugins/rspqiservice.h"

#include "RSZeroReserveItems.h"




class RsPluginHandler;
class OrderBook;

class p3ZeroReserveRS : public RsPQIService
{
public:
    p3ZeroReserveRS(RsPluginHandler *pgHandler, OrderBook * bids, OrderBook * asks);

    virtual int tick();
    bool sendOrder( const std::string& peer_id, OrderBook::Order * order );

private:

    void processIncoming();
    void sendPackets();
    void handleOrder(RsZeroReserveOrderBookItem *item);

};

#endif // P3ZERORESERVERRS_H
