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


#include "BtcContract.h"
#include "ZRBitcoin.h"

const unsigned int BtcContract::reqConfirmations = 6;
std::vector< BtcContract* > BtcContract::contracts;


void BtcContract::loadContracts()
{

}

void BtcContract::pollContracts()
{
    for( ContractIterator it = contracts.begin(); it != contracts.end(); it++ ){
        (*it)->poll();
    }
}


BtcContract::BtcContract(const std::string & txId, const ZR::ZR_Number & fiatAmount, const std::string & currencySym , Party party):
    m_txId( txId ),
    m_fiatAmount( fiatAmount ),
    m_currencySym( currencySym ),
    m_party( party )
{
}

void BtcContract::poll()
{
    unsigned int confirmations = ZR::Bitcoin::Instance()->getConfirmations( m_txId );
    if( confirmations >= reqConfirmations ){
        execute();
    }
}

void BtcContract::persist()
{

}

void BtcContract::execute()
{

}
