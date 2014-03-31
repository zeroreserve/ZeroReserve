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

#ifndef ZRBITCOIN_H
#define ZRBITCOIN_H

#include "zrtypes.h"
#include "zrdb.h"

namespace ZR
{


class Wallet
{
public:
    Wallet() {}

    virtual ZR::BitcoinAddress getAddress() = 0;
    virtual ZR::ZR_Number getBalance() = 0;
    virtual ZR::RetVal persist() = 0;

    const std::string & getNick(){ return m_nick; }
    void setNick( const std::string & nick ){ m_nick = nick; }

protected:
    std::string m_nick;
};


class MyWallet : public Wallet
{
public:
    enum WalletType {
        BRAINWALLET,
        ELECTRUMSEED,
        WIFIMPORT,
        CASASCIUS,
        INVALID
    };

    MyWallet( WalletType wType ) : m_walletType( wType )
    {}
    virtual ~MyWallet(){}

    virtual ZR::WalletSeed seed() = 0;
    virtual void setSeed( const ZR::WalletSeed & seed ) = 0;

    virtual ZR::RetVal getSecret( ZR::WalletSecret & secret_out ) = 0;
    virtual std::string getPubKey() = 0;

protected:
    WalletType m_walletType;
};

class PeerWallet : public Wallet
{
    PeerWallet();
public:
    PeerWallet( const ZR::BitcoinAddress & address ) :
        m_Address( address )
    {}
    virtual ZR::BitcoinAddress getAddress(){ return m_Address; }
    virtual ZR::RetVal persist(){
        return ZrDB::Instance()->addPeerWallet( m_Address, m_nick );
    }

    virtual ZR::ZR_Number getBalance(){
        return 0;
    }

private:
    ZR::BitcoinAddress m_Address;
};


class Bitcoin
{
public:
    virtual ZR::RetVal commit() = 0;
    virtual ZR::RetVal start() = 0;
    virtual ZR::RetVal stop() = 0;
    virtual ZR::ZR_Number getBalance() = 0;

    virtual MyWallet * mkWallet( MyWallet::WalletType wType ) = 0;
    virtual void loadWallets( std::vector< ZR::MyWallet *> & wallets ) = 0;

    virtual unsigned int getConfirmations( const std::string & txId ) = 0;

    virtual void send( const std::string & dest, const ZR::ZR_Number & amount ) = 0;
    virtual ZR::BitcoinAddress registerMultiSig( const ZR::BitcoinPubKey & key1, const ZR::BitcoinPubKey & key2 ) = 0;
    virtual std::string settleMultiSig( const std::string & txId, const ZR::BitcoinPubKey & key ) = 0;
    virtual void initDeal( const std::string & pubKey , const ZR::ZR_Number & amount , std::string & myPubKey, std::string & txId ) = 0;

    static Bitcoin * Instance();

protected:

    static Bitcoin * instance;
};


}

#endif // ZRBITCOIN_H
