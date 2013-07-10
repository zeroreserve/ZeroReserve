/*
    This file is part of the Zero Reserve Plugin for Retroshare.

    Zero Reserve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zero Reserve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Zero Reserve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "p3ZeroReserverRS.h"

p3ZeroReserveRS::p3ZeroReserveRS( RsPluginHandler *pgHandler ) :
        RsPQIService( RS_SERVICE_TYPE_ZERORESERVE_PLUGIN, CONFIG_TYPE_ZERORESERVE_PLUGIN, 0, pgHandler )
{
}

int p3ZeroReserveRS::tick()
{
    return 0;
}
