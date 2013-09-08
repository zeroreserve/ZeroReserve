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

#ifndef ROUTER_H
#define ROUTER_H

#include "zrtypes.h"

#include <map>
#include <string>


/**
 * @brief Interface class for routers
 *
 * A route is a possible tunnel, pointing to the source of some data, for example the origin of
 * an order. Each node on the network knows only the next hop of that route.
 *
 * A tunnel is a route that is currently in use. As it is 2-directional, each node knows the next
 * hop in both direction. Tunnels exist only as long as 2 distant nodes, i.e. nodes that are not
 * friends, need to communicate.
 */

class Router
{
public:
    typedef std::map< ZR::VirtualAddress, std::pair< std::string, std::string > > TunnelList;
    enum TunnelDirection {
        SERVER = 0,
        CLIENT
    };

    virtual void addRoute( const ZR::VirtualAddress & dest, const std::string & gateway ) = 0;

    /** Query next hop of a route */
    virtual const std::string nextHop( const ZR::VirtualAddress & dest ) = 0;
    virtual bool hasRoute( const ZR::VirtualAddress & dest ) = 0;

    /**
     * adds a virtual tunnel
     * @arg dest: the virtual address
     * @arg gateways: a pair of the IDs of the hop forward and the hop backward
     */
    virtual void addTunnel( const ZR::VirtualAddress & dest, std::pair< std::string, std::string > & gateways ){ m_Tunnels[ dest ] = gateways; }
    virtual ZR::RetVal getTunnel( const ZR::VirtualAddress & dest, std::pair< std::string, std::string > & gateways )
    {
        TunnelList::iterator it = m_Tunnels.find( dest );
        if( it == m_Tunnels.end() )
            return ZR::ZR_FAILURE;
        gateways = (*it).second;
        return ZR::ZR_SUCCESS;
    }

    static Router * Instance();


protected:
    TunnelList m_Tunnels;

    static Router * instance;
};

#endif // ROUTER_H
