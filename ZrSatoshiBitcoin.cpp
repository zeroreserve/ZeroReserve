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


std::string ZrSatoshiBitcoin::settleMultiSig( const std::string & txId, const ZR::BitcoinPubKey & key )
{
    try{
        JsonRpc rpc( m_settings );
        JsonRpc::JsonData tx( Json::arrayValue );

        JsonRpc::JsonData txObj;
        txObj[ Json::StaticString( "txid" ) ] = txId;
        tx.append( txObj );

        JsonRpc::JsonData voutObj;
        voutObj[ Json::StaticString( "vout" ) ] = 1;   // FIXME!!!!
        tx.append( voutObj );

        JsonRpc::JsonData addr = rpc.executeRpc ( "getnewaddress" );
        JsonRpc::JsonData dest;
        dest[ Json::StaticString( addr.asString().c_str() ) ] = 0.01;  // FIXME!!!!

        JsonRpc::JsonData res = rpc.executeRpc ("createrawtransaction", tx, dest );
        std::string rawTx = res.asString();
        JsonRpc::JsonData res1 = rpc.executeRpc ("signrawtransaction", res );
        std::string signedRawTx = res1.asString();
        return signedRawTx;
    }
    catch( nmcrpc::JsonRpc::RpcError e ){
        std::cerr << "Zero Reserve: Exception caught: " << e.what() << std::endl;
    }
    return "";
}


ZR::BitcoinAddress ZrSatoshiBitcoin::registerMultiSig(const ZR::BitcoinPubKey &key1, const ZR::BitcoinPubKey &key2 )
{
    std::cerr << "Zero Reserve: Creating new Multisig Address from myKey: " << key1 << std::endl;
    std::cerr << "                                              otherKey: " << key2 << std::endl;
    try{
        JsonRpc rpc( m_settings );
        JsonRpc::JsonData keys( Json::arrayValue );
        keys.append( key1 );
        keys.append( key2 );
        JsonRpc::JsonData res2 = rpc.executeRpc( "addmultisigaddress", 2, keys );
        std::cerr << "Zero Reserve: New Multisig Address: " << res2.asString() << std::endl;
        return res2.asString();
    }
    catch( nmcrpc::JsonRpc::RpcError e ){
        std::cerr << "Zero Reserve: Exception caught: " << e.what() << std::endl;
    }
    return "";
}


void ZrSatoshiBitcoin::initDeal( const ZR::BitcoinPubKey & pubKey, const ZR::ZR_Number & amount, ZR::BitcoinPubKey & myPubKey, std::string & txId )
{
    try{
        JsonRpc rpc( m_settings );
        JsonRpc::JsonData res = rpc.executeRpc ( "getnewaddress" );
        ZR::BitcoinAddress address = res.asString();

        JsonRpc::JsonData res1 = rpc.executeRpc( "validateaddress", address );
        myPubKey = res1[ "pubkey" ].asString();

        ZR::BitcoinAddress multisigAddress = registerMultiSig( myPubKey, pubKey );

        JsonRpc::JsonData res3 = rpc.executeRpc( "sendtoaddress", multisigAddress, JsonRpc::JsonData( amount.toDouble() ) );
        txId = res3.asString();
        std::cerr << "Zero Reserve: New TX on Multisig Address: " << multisigAddress << " TX ID: " << txId << std::endl;
    }
    catch( nmcrpc::JsonRpc::RpcError e ){
        std::cerr << "Zero Reserve: Exception caught: " << e.what() << std::endl;
    }
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


