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
#include "Payment.h"
#include "zrdb.h"

#ifdef ZR_TESTNET
const unsigned int BtcContract::reqConfirmations = 2;
#else
const unsigned int BtcContract::reqConfirmations = 6;
#endif

std::list< BtcContract* > BtcContract::contracts;
RsMutex BtcContract::m_contractMutex("ContractMutex");



void BtcContract::pollContracts()
{
    for( ContractIterator it = contracts.begin(); it != contracts.end(); it++ ){
        if( (*it)->poll() ){
            rmContract( *it );
        }
    }
}

void BtcContract::rmContract( BtcContract * contract )
{
    RsStackMutex contractMutex( m_contractMutex );

    for( ContractIterator it = contracts.begin(); it != contracts.end(); it++){
        BtcContract * c = *it;
        if( c->m_counterParty == contract->m_counterParty && c->m_btcTxId == contract->m_btcTxId ){
            if( !contract->m_btcTxId.empty() ){
                ZrDB::Instance()->rmBtcContract( contract->m_btcTxId );
            }
            RsStackMutex contractMutex( m_contractMutex );
            contracts.erase( it );
            delete contract;
            break;
        }
    }
}

void BtcContract::rmContract( const ZR::TransactionId & id )
{
    RsStackMutex contractMutex( m_contractMutex );

    for( ContractIterator it = contracts.begin(); it != contracts.end(); it++){
        if( (*it)->m_btcTxId == id ){
            BtcContract * contract = *it;
            if( !contract->m_btcTxId.empty() ){
                ZrDB::Instance()->rmBtcContract( contract->m_btcTxId );
            }
            RsStackMutex contractMutex( m_contractMutex );
            contracts.erase( it );
            delete contract;
            break;
        }
    }
}

///// End static functions


BtcContract::BtcContract( const ZR::ZR_Number & btcAmount, const ZR::ZR_Number & fee, const ZR::ZR_Number & price , const std::string & currencySym , Party party, const std::string & counterParty, const qint64 creationtime ):
    m_btcAmount( btcAmount ),
    m_price( price ),
    m_currencySym( currencySym ),
    m_party( party ),
    m_counterParty( counterParty ),
    m_activated( false ),
    m_fee( fee )
{
    if( creationtime == 0 ){
        m_creationtime = QDateTime::currentMSecsSinceEpoch();  // newly created contract gets a current timestamp
    }
    else{
        m_creationtime = creationtime;  // contract loaded from DB
    }

    RsStackMutex contractMutex( m_contractMutex );

    if( party == RECEIVER ){
        ZR::ZR_Number fiatAmount = btcAmount * price;
        Credit c( counterParty, currencySym );
        c.loadPeer();
        if( c.getMyAvailable() <= fee ) throw std::runtime_error( "Insufficient funds" );
        if( c.getMyAvailable() < fiatAmount + fee ){
            fiatAmount = c.getPeerAvailable() - fee;
            m_btcAmount = fiatAmount / price;
        }
        c.allocate( fiatAmount );
    }

    contracts.push_back( this );
}

BtcContract::~BtcContract()
{
}

bool BtcContract::poll()
{
    if( !m_activated ) return false; // not yet active

    unsigned int confirmations = ZR::Bitcoin::Instance()->getConfirmations( m_btcTxId );
    std::cerr << "Zero Reserve: Contract: " << m_btcTxId << " : " << confirmations << " confirmations." << std::endl;
    if( confirmations >= reqConfirmations ){
        // TODO: Check BTC Address and amount
        execute();
        return true;
    }
    return false;
}

void BtcContract::persist()
{
    ZrDB::Instance()->addBtcContract( this );
}


void BtcContract::execute()
{
    std::cerr << "Zero Reserve: Contract: Settling for ID: " << m_btcTxId << std::endl;
    if( m_party == SENDER ){
        PaymentSpender p( m_counterParty, m_btcAmount * m_price, m_currencySym, Payment::PAYMENT );
        p.commit();
    }
    else {
        PaymentReceiver p( m_counterParty, m_btcAmount * m_price, m_currencySym, Payment::PAYMENT );
        p.commit();
    }
    ZrDB::Instance()->rmBtcContract( m_btcTxId );
}


