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

#ifndef TRACEROUTER_H
#define TRACEROUTER_H

#include "Router.h"

#include <string>
#include <map>

/**
 * @brief Very simple router class, gathering routing information from items that propagate through the net
 */

class TraceRouter : public Router
{
public:
    TraceRouter();

    typedef std::map< std::string, std::string > RoutingTable;

    virtual void addRoute( const std::string & dest, const std::string & gateway );
    virtual const std::string nextHop( const std::string & dest );

protected:
    /** map destinions to gateways */
    RoutingTable routingTable;
};

#endif // TRACEROUTER_H
