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




#include "ZrDummyBitcoin.h"


ZR::Bitcoin * ZR::Bitcoin::instance = NULL;


ZR::Bitcoin * ZR::Bitcoin::Instance()
{
    if( instance == NULL ){
        instance = new ZrDummyBitcoin();
    }
    return instance;
}


ZrDummyBitcoin::ZrDummyBitcoin()
{
}

ZR::RetVal ZrDummyBitcoin::start()
{
    return ZR::ZR_SUCCESS;
}

ZR::RetVal ZrDummyBitcoin::stop()
{
    return ZR::ZR_SUCCESS;
}


ZR::RetVal ZrDummyBitcoin::commit()
{
    return ZR::ZR_SUCCESS;
}


ZR::ZR_Number ZrDummyBitcoin::getBalance()
{
    return 0;
}

ZR::Wallet * ZrDummyBitcoin::mkWallet( ZR::MyWallet::WalletType wType )
{
    return NULL;
}
