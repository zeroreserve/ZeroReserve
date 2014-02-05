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



#ifndef ZRDUMMYBITCOIN_H
#define ZRDUMMYBITCOIN_H

#include "zrtypes.h"
#include "ZRBitcoin.h"


class ZrDummyBitcoin : public ZR::Bitcoin
{
public:
    ZrDummyBitcoin();

    virtual ZR::RetVal start();
    virtual ZR::RetVal stop();
    virtual ZR::RetVal commit();
    virtual ZR::ZR_Number getBalance();
    virtual ZR::Wallet * mkWallet( ZR::MyWallet::WalletType wType );

};

#endif // ZRDUMMYBITCOIN_H
