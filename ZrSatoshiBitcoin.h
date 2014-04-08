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

#ifndef ZRSATOSHIBITCOIN_H
#define ZRSATOSHIBITCOIN_H

#include "ZRBitcoin.h"

#include "JsonRpc.hpp"


class ZrSatoshiBitcoin : public ZR::Bitcoin
{
public:
    ZrSatoshiBitcoin();

    virtual ZR::RetVal commit();
    virtual ZR::RetVal start();
    virtual ZR::RetVal stop();
    virtual ZR::ZR_Number getBalance();

    virtual ZR::MyWallet * mkWallet( ZR::MyWallet::WalletType wType );
    virtual void loadWallets( std::vector< ZR::MyWallet *> & wallets );

    virtual unsigned int getConfirmations( const std::string & txId );

    virtual void send( const std::string & dest, const ZR::ZR_Number & amount );

    virtual ZR::BitcoinAddress registerMultiSig(const ZR::BitcoinPubKey & key1, const ZR::BitcoinPubKey & key2 );
    virtual std::string settleMultiSig( const std::string & txId, const ZR::ZR_Number & amount, const ZR::BitcoinAddress & multiSigAddr );
    virtual void finalizeMultiSig( const std::string & tx );

    virtual void initDeal(const ZR::BitcoinPubKey & pubKey , const ZR::ZR_Number & amount , ZR::BitcoinPubKey & myPubKey, std::string & txId );

    virtual const ZR::BitcoinAddress newAddress() const;
    virtual const ZR::BitcoinTxHex mkRawTx( const ZR::ZR_Number & btcAmount, const ZR::BitcoinAddress & sendAddr, const ZR::BitcoinAddress & recvAddr ) const;
    virtual ZR::BitcoinAddress mkOrderAddress( const ZR::ZR_Number & amount );


public:
    nmcrpc::RpcSettings m_settings;
};



class SatoshiWallet : public ZR::MyWallet
{
public:
    SatoshiWallet( const ZR::BitcoinAddress & address, const ZR::ZR_Number & balance ) :
        ZR::MyWallet( WIFIMPORT ),
        m_Address( address ),
        m_Balance( balance )
    {}
    virtual ZR::BitcoinAddress getAddress(){ return m_Address; }
    virtual ZR::ZR_Number getBalance(){ return m_Balance; }
    virtual std::string getPubKey();
    virtual ZR::RetVal persist(){ return ZR::ZR_SUCCESS; }
    virtual ZR::WalletSeed seed(){ return ""; }
    virtual void setSeed( const ZR::WalletSeed & ){}
    virtual ZR::RetVal getSecret( ZR::WalletSecret & ){ return ZR::ZR_SUCCESS; }


private:
    ZR::BitcoinAddress m_Address;
    ZR::ZR_Number m_Balance;
};


#endif // ZRSATOSHIBITCOIN_H
