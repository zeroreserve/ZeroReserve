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

#include "ZrSatoshiBitcoin.h"

#include "helpers.h"


using namespace nmcrpc;

ZrSatoshiBitcoin::ZrSatoshiBitcoin()
{
    try {
#ifdef WIN32
        std::string home = getenv ("APPDATA");
        m_settings.readConfig( home + "/bitcoin/bitcoin.conf" );
#else
        std::string home = getenv ("HOME");
        m_settings.readConfig( home + "/.bitcoin/bitcoin.conf" );
#endif
    }
    catch( std::runtime_error e ){
        std::cerr << "Zero Reserve: " << __func__ << ": Exception caught: " << e.what() << std::endl;
        print_stacktrace();
    }
}


ZR::RetVal ZrSatoshiBitcoin::commit()
{
    return ZR::ZR_SUCCESS;
}

ZR::RetVal ZrSatoshiBitcoin::start()
{
    return ZR::ZR_SUCCESS;
}

ZR::RetVal ZrSatoshiBitcoin::stop()
{
    return ZR::ZR_SUCCESS;
}

ZR::RetVal ZrSatoshiBitcoin::getinfo( BtcInfo & infoOut )
{
    JsonRpc rpc( m_settings );
    try{
        JsonRpc::JsonData res = rpc.executeRpc ( "getinfo" );
        infoOut.testnet = res[ "testnet" ].asBool();
        infoOut.balance = res[ "balance" ].asDouble();
        infoOut.version = res[ "version" ].asUInt();
    }
    catch( std::runtime_error e ){
        std::cerr << "Zero Reserve: " << __func__ << ": Exception caught: " << e.what() << std::endl;
        print_stacktrace();
        return ZR::ZR_FAILURE;
    }

    return ZR::ZR_SUCCESS;
}


ZR::ZR_Number ZrSatoshiBitcoin::getBalance()
{

    JsonRpc rpc( m_settings );
    JsonRpc::JsonData res = rpc.executeRpc ( "getinfo" );
    ZR::ZR_Number balance = res["balance"].asDouble();

    return balance;
}


ZR::MyWallet * ZrSatoshiBitcoin::mkWallet( ZR::MyWallet::WalletType wType )
{
    JsonRpc rpc( m_settings );
    try{
        if( wType == ZR::MyWallet::WIFIMPORT ){
            JsonRpc::JsonData res = rpc.executeRpc ( "getnewaddress" );
            ZR::BitcoinAddress address = res.asString();
            return new SatoshiWallet( address, 0 );
        }
    }
    catch( std::runtime_error e ){
        std::cerr << "Zero Reserve: " << __func__ << ": Exception caught: " << e.what() << std::endl;
        print_stacktrace();
    }
    return NULL;
}

void ZrSatoshiBitcoin::loadWallets( std::vector< ZR::MyWallet *> & wallets )
{
    JsonRpc rpc( m_settings );
    try{
        JsonRpc::JsonData res = rpc.executeRpc ("listaddressgroupings");
        for( nmcrpc::JsonRpc::JsonData::iterator it1 = res.begin(); it1 != res.end(); it1++ ){
            JsonRpc::JsonData res0 = *it1;
            assert (res0.isArray());
            for( nmcrpc::JsonRpc::JsonData::iterator it2 = res0.begin(); it2 != res0.end(); it2++ ){
                JsonRpc::JsonData res1 = *it2;
                JsonRpc::JsonData res2 = res1[ 0u ];
                ZR::BitcoinAddress address = res2.asString();

                JsonRpc::JsonData res21 = res1[ 1u ];
                ZR::ZR_Number balance = res21.asDouble();
                ZR::MyWallet * wallet = new SatoshiWallet( address, balance );
                wallets.push_back( wallet );
            }
        }
    }
    catch( std::runtime_error e ){
        std::cerr << "Zero Reserve: " << __func__ << ": Exception caught: " << e.what() << std::endl;
        print_stacktrace();
    }
}


void ZrSatoshiBitcoin::send( const std::string & dest, const ZR::ZR_Number & amount )
{
    JsonRpc rpc( m_settings );
    try{
        JsonRpc::JsonData res = rpc.executeRpc ("sendtoaddress", dest, amount.toDouble() );
    }
    catch( std::runtime_error e ){
        std::cerr << "Zero Reserve: " << __func__ << ": Exception caught: " << e.what() << std::endl;
        print_stacktrace();
    }
}




ZR::RetVal ZrSatoshiBitcoin::mkRawTx( const ZR::ZR_Number & btcAmount, const ZR::BitcoinAddress & sendAddr, const ZR::BitcoinAddress & recvAddr, ZR::BitcoinTxHex & outTx, ZR::TransactionId & outId ) const
{
    try{
        JsonRpc rpc( m_settings );
        JsonRpc::JsonData addrArray( Json::arrayValue );
        addrArray.append( sendAddr );
        JsonRpc::JsonData res = rpc.executeRpc ( "listunspent", 0, 999999, addrArray );
        std::string txId = res[0u][ "txid" ].asString();
        unsigned int vout = res[0u][ "vout" ].asUInt();

        JsonRpc::JsonData txArray( Json::arrayValue );
        JsonRpc::JsonData txObj;
        txObj[ Json::StaticString( "txid" ) ] = txId;
        txObj[ Json::StaticString( "vout" ) ] = vout;
        txArray.append( txObj );

        JsonRpc::JsonData dest;
        dest[ Json::StaticString( recvAddr.c_str() ) ] = btcAmount.toDouble();
        JsonRpc::JsonData res1 = rpc.executeRpc ("createrawtransaction", txArray, dest );
        std::string rawTx = res1.asString();
        JsonRpc::JsonData res2 = rpc.executeRpc ( "signrawtransaction", rawTx );
        if( !res2[ "complete" ] ) return ZR::ZR_FAILURE;
        outTx = res2[ "hex" ].asString();
        JsonRpc::JsonData res3 = rpc.executeRpc ( "decoderawtransaction", outTx );
        outId = res3[ "txid" ].asString();
    }
    catch( std::runtime_error e ){
        std::cerr << "Zero Reserve: " << __func__ << ": Exception caught: " << e.what() << std::endl;
        print_stacktrace();
        return ZR::ZR_FAILURE;
    }
    return ZR::ZR_SUCCESS;
}


ZR::BitcoinAddress ZrSatoshiBitcoin::mkOrderAddress( const ZR::ZR_Number & amount )
{
    std::string addr = newAddress();

    try{
        JsonRpc rpc( m_settings );
        JsonRpc::JsonData res = rpc.executeRpc ( "sendtoaddress", addr, JsonRpc::JsonData( amount.toDouble() ) );
        // TODO: Error handling
        std::string id = res.asString();

        // make sure the Satoshi client does not touch "id"
        // this will go away if the Satoshi client is restarted
        // may be unnecessary for other implementations of this class, e.g. libbitcoin
        JsonRpc::JsonData addrArray( Json::arrayValue );
        addrArray.append( addr );
        JsonRpc::JsonData res2 = rpc.executeRpc ( "listunspent", 0, 999999, addrArray );
        unsigned int vout = res2[0u][ "vout" ].asUInt();
        JsonRpc::JsonData lockObjArray( Json::arrayValue );
        JsonRpc::JsonData lockObj;
        lockObj[ "txid" ] = id;
        lockObj[ "vout" ] = vout;
        lockObjArray.append( lockObj );
        JsonRpc::JsonData res1 = rpc.executeRpc ( "lockunspent", true, lockObjArray );
    }
    catch( std::runtime_error e ){
        std::cerr << "Zero Reserve: " << __func__ << ": Exception caught: " << e.what() << std::endl;
        print_stacktrace();
        return "";
    }
    return addr;
}


ZR::RetVal ZrSatoshiBitcoin::sendRaw( const ZR::BitcoinTxHex & txHex )
{
    try{
        JsonRpc rpc( m_settings );
        JsonRpc::JsonData res1 = rpc.executeRpc ( "sendrawtransaction", txHex );
    }
    catch( std::runtime_error e ){
        std::cerr << "Zero Reserve: " << __func__ << ": Exception caught: " << e.what() << std::endl;
        print_stacktrace();
        return ZR::ZR_FAILURE;
    }
    return ZR::ZR_SUCCESS;
}


const ZR::BitcoinAddress ZrSatoshiBitcoin::newAddress() const
{
    std::string addr;
    try{
        JsonRpc rpc( m_settings );
        JsonRpc::JsonData resAddr = rpc.executeRpc ( "getnewaddress" );
        addr = resAddr.asString();
    }
    catch( std::runtime_error e ){
        std::cerr << "Zero Reserve: " << __func__ << ": Exception caught: " << e.what() << std::endl;
        print_stacktrace();
    }
    return addr;
}


unsigned int ZrSatoshiBitcoin::getConfirmations( const std::string & txId )
{
    try{
        JsonRpc rpc( m_settings );
        JsonRpc::JsonData res = rpc.executeRpc ( "getrawtransaction", txId, 1 );
        unsigned int confirmations = res[ "confirmations" ].asUInt();
        return confirmations;
    }
    catch( std::runtime_error e ){
        std::cerr << "Zero Reserve: " << __func__ << ": Exception caught: " << e.what() << std::endl;
        print_stacktrace();
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////

ZR::Bitcoin * ZR::Bitcoin::instance = NULL;


ZR::Bitcoin * ZR::Bitcoin::Instance()
{
    if( instance == NULL ){
        instance = new ZrSatoshiBitcoin();
    }
    return instance;
}


std::string SatoshiWallet::getPubKey()
{
    ZrSatoshiBitcoin * bitcoin = dynamic_cast<ZrSatoshiBitcoin*> ( ZR::Bitcoin::Instance() );
    JsonRpc rpc(bitcoin->m_settings );
    JsonRpc::JsonData res1 = rpc.executeRpc( "validateaddress", m_Address );
    std::string myPubKey = res1[ "pubkey" ].asString();
    return myPubKey;
}


