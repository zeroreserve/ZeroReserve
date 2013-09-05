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

#ifndef TMREMOTECOORDINATOR_H
#define TMREMOTECOORDINATOR_H

#include "TransactionManager.h"
#include "zrtypes.h"


class TmRemoteCoordinator : public TransactionManager
{
    TmRemoteCoordinator();
public:
    TmRemoteCoordinator(const ZR::VirtualAddress &addr);
    virtual int initCoordinator( Payment * payment );

private:
    ZR::VirtualAddress m_Destination;
};

#endif // TMREMOTECOORDINATOR_H
