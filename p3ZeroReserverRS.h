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


#include "RSZeroReserveItems.h"

#include "retroshare/rspeers.h"
#include "plugins/rspqiservice.h"



class RsPluginHandler;
class OrderBook;
class RsPeers;

class p3ZeroReserveRS : public RsPQIService
{
public:
    p3ZeroReserveRS(RsPluginHandler *pgHandler, OrderBook * bids, OrderBook * asks, RsPeers* peers);

    virtual int tick();
    bool sendOrder( const std::string& peer_id, OrderBook::Order * order );
    bool sendCredit( Credit * credit );
    void publishOrder( OrderBook::Order * order );
    std::string getOwnId(){ return m_peers->getOwnId(); }

private:

    void processIncoming();
    void sendPackets();
    void handleOrder( RsZeroReserveOrderBookItem *item );
    void handleCredit( RsZeroReserveCreditItem *item );

private:
    OrderBook * m_bids;
    OrderBook * m_asks;
    RsPeers * m_peers;
};

#endif // P3ZERORESERVERRS_H
