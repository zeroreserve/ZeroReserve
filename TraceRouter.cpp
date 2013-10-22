/*!
 * \file TraceRoute.cpp
 * 
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

#include "TraceRouter.h"

/**
 * @brief TraceRouter constructor
 */
TraceRouter::TraceRouter()
{
}

/**
 * @brief Add Route
 *
 * @param dest
 * @param gateway
 */
void TraceRouter::addRoute( const ZR::VirtualAddress &dest, const std::string & gateway )
{
    routingTable[ dest ] = gateway;
}


/**
 * @brief Next hop
 *
 * @param dest
 *
 * @return 
 */
const std::string TraceRouter::nextHop( const ZR::VirtualAddress &dest )
{
    RoutingTable::const_iterator it = routingTable.find( dest );
    if( it != routingTable.end() )
    {
        return (*it).second;
    }

    return std::string();
}

/**
 * @brief has a route?
 *
 * @param dest
 *
 * @return If no route, return false, otherwise return true.
 * 
 * @retval false or true
 */
bool TraceRouter::hasRoute( const ZR::VirtualAddress & dest )
{
    if( routingTable.find( dest ) == routingTable.end() )
    {
        return false;
    }
    return true;
}

//   EOF   
