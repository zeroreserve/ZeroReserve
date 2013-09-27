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

#ifndef ZRLIBBITCOIN_H
#define ZRLIBBITCOIN_H

#include "ZRBitcoin.h"
#include "zrtypes.h"

#include <bitcoin/bitcoin.hpp>

#include <string>

class ZrLibBitcoin : public ZR::Bitcoin
{
public:
    ZrLibBitcoin();
    virtual ZR::RetVal start();
    virtual ZR::RetVal stop();
    virtual ZR::RetVal commit();
    virtual ZR::ZR_Number getBalance();

private:
    ZR::RetVal initChain( const std::string & pathname );

    bc::threadpool m_netPool;
    bc::threadpool m_diskPool;
    bc::threadpool m_memPool;

    bc::leveldb_blockchain m_blockChain;

};

#endif // ZRLIBBITCOIN_H
