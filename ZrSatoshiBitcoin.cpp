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


using namespace nmcrpc;

ZrSatoshiBitcoin::ZrSatoshiBitcoin()
{
    std::string home = getenv ("HOME");
    m_settings.readConfig( home + "/.bitcoin/bitcoin.conf" );
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

ZR::ZR_Number ZrSatoshiBitcoin::getBalance()
{

    JsonRpc rpc( m_settings );
    JsonRpc::JsonData res = rpc.executeRpc ( "getinfo" );
    ZR::ZR_Number balance = res["balance"].asDouble();

    return balance;
}


ZR::MyWallet * ZrSatoshiBitcoin::mkWallet( ZR::MyWallet::WalletType wType )
{
    if( wType == ZR::MyWallet::WIFIMPORT ){
        JsonRpc rpc( m_settings );
        JsonRpc::JsonData res = rpc.executeRpc ( "getnewaddress" );
        ZR::BitcoinAddress address = res.asString();
        return new SatoshiWallet( address, 0 );
    }
    return NULL;
}

void ZrSatoshiBitcoin::loadWallets( std::vector< ZR::MyWallet *> & wallets )
{
    JsonRpc rpc( m_settings );
    JsonRpc::JsonData res = rpc.executeRpc ("listaddressgroupings");
//    assert (res.isArray());
    for( nmcrpc::JsonRpc::JsonData::iterator it1 = res.begin(); it1 != res.end(); it1++ ){
        JsonRpc::JsonData res0 = *it1;
        assert (res0.isArray());
        for( nmcrpc::JsonRpc::JsonData::iterator it2 = res0.begin(); it2 != res0.end(); it2++ ){
            JsonRpc::JsonData res1 = *it2;
//            assert (res1.isArray());

            JsonRpc::JsonData res2 = res1[ 0u ];
//            assert (res2.isString());
            ZR::BitcoinAddress address = res2.asString();

            JsonRpc::JsonData res21 = res1[ 1u ];
//            assert ( res21.isDouble() );
            ZR::ZR_Number balance = res21.asDouble();
            ZR::MyWallet * wallet = new SatoshiWallet( address, balance );
            wallets.push_back( wallet );
        }
    }

    return;
}


void ZrSatoshiBitcoin::send( const std::string & dest, const ZR::ZR_Number & amount )
{
    JsonRpc rpc( m_settings );
    std::vector<JsonRpc::JsonData> params;
    params.push_back( dest );
    params.push_back( amount.toDouble() );
    JsonRpc::JsonData res = rpc.executeRpcList ("sendtoaddress", params );
}




const ZR::BitcoinTxHex ZrSatoshiBitcoin::mkRawTx( const ZR::ZR_Number & btcAmount, const ZR::BitcoinAddress & sendAddr, const ZR::BitcoinAddress & recvAddr ) const
{
    ZR::BitcoinTxHex tx;
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
        tx = res1.asString();
    }
    catch( nmcrpc::JsonRpc::RpcError e ){
        std::cerr << "Zero Reserve: Exception caught: " << e.what() << std::endl;
    }
    return tx;
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
    catch( nmcrpc::JsonRpc::RpcError e ){
        std::cerr << "Zero Reserve: Exception caught: " << e.what() << std::endl;
    }
    return addr;
}


const ZR::BitcoinAddress ZrSatoshiBitcoin::newAddress() const
{
    std::string addr;
    try{
        JsonRpc rpc( m_settings );
        JsonRpc::JsonData resAddr = rpc.executeRpc ( "getnewaddress" );
        addr = resAddr.asString();
    }
    catch( nmcrpc::JsonRpc::RpcError e ){
        std::cerr << "Zero Reserve: Exception caught: " << e.what() << std::endl;
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
    catch( nmcrpc::JsonRpc::RpcError e ){
        std::cerr << "Zero Reserve: Exception caught: " << e.what() << std::endl;
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


