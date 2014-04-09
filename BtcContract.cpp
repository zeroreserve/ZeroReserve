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
#include "TmContract.h"

#ifdef ZR_TESTNET
const unsigned int BtcContract::reqConfirmations = 1;
#else
const unsigned int BtcContract::reqConfirmations = 6;
#endif

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

///// End static functions


BtcContract::BtcContract( const ZR::ZR_Number & btcAmount, const ZR::ZR_Number & price , const std::string & currencySym , Party party):
    m_btcAmount( btcAmount ),
    m_price( price ),
    m_currencySym( currencySym ),
    m_party( party ),
    m_contractTx( NULL )
{
    contracts.push_back( this );
}

BtcContract::~BtcContract()
{
    for( ContractIterator it = contracts.begin(); it != contracts.end(); it++){
        if( (*it) == this )
            contracts.erase( it );
    }
}

void BtcContract::poll()
{
    unsigned int confirmations = ZR::Bitcoin::Instance()->getConfirmations( m_btcTxId );
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

void BtcContract::startTransaction( const ZR::VirtualAddress & addr, const std::string & myId )
{
}
