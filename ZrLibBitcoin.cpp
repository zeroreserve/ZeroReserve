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

#include "ZrLibBitcoin.h"

#include "retroshare/rsinit.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"
#include "zrdb.h"

#include <QString>
#include <QDir>

#include <string>
#include <future>


ZR::Bitcoin * ZR::Bitcoin::instance = NULL;


ZR::Bitcoin * ZR::Bitcoin::Instance()
{
    if( instance == NULL ){
        instance = new ZrLibBitcoin();
    }
    return instance;
}


ZrLibBitcoin::ZrLibBitcoin() :
    m_netPool( 1 ), m_diskPool( 4 ), m_memPool( 1 ),
    m_hosts(m_netPool), m_handshake(m_netPool), m_network(m_netPool),
    m_protocol(m_netPool, m_hosts, m_handshake, m_network),
    m_blockChain( m_diskPool ),
    m_poller( m_memPool, m_blockChain), m_txpool( m_memPool, m_blockChain ), m_txidx(m_memPool),
    m_session( m_netPool, { m_handshake, m_protocol, m_blockChain, m_poller, m_txpool} ),
    started( false )
{}

ZR::RetVal ZrLibBitcoin::initChain( const std::string & pathname )
{
    std::promise<std::error_code> pr_chain;
    auto blockchain_startup = [&]( const std::error_code& ec ){
            pr_chain.set_value(ec);
    };
    m_blockChain.start( pathname, blockchain_startup );
    std::error_code ec_start = pr_chain.get_future().get();
    if( ec_start ) {
        std::cerr << "Problem starting blockchain: " << ec_start.message() << std::endl;
        return ZR::ZR_FAILURE;
    }

    std::promise<std::error_code> pr_genesis;
    auto blockchain_genesis = [&]( const std::error_code& ec ){
            pr_genesis.set_value(ec);
    };
    bc::block_type first_block = bc::genesis_block();
    m_blockChain.import( first_block, 0, blockchain_genesis );
    std::error_code ec_genesis = pr_genesis.get_future().get();
    if( ec_genesis ){
        std::cerr << "Importing genesis block failed: " << ec_genesis.message() << std::cerr;
        return ZR::ZR_FAILURE;
    }
    return ZR::ZR_SUCCESS;
}


void ZrLibBitcoin::connection_started(const std::error_code& ec, bc::channel_ptr node )
{
    if ( ec ){
        std::cerr << "Couldn't start connection: " << ec.message() << std::endl;
        return;
    }
    node->subscribe_transaction( std::bind( &ZrLibBitcoin::recv_tx, this, std::placeholders::_1, std::placeholders::_2, node ) );
    m_protocol.subscribe_channel( std::bind(&ZrLibBitcoin::connection_started, this, std::placeholders::_1, std::placeholders::_2 ) );
}



void ZrLibBitcoin::recv_tx( const std::error_code& ec, const bc::transaction_type& tx, bc::channel_ptr node )
{
    std::cerr << "Zero Reserve: Receiving Transaction" << std::endl;
    if ( ec ){
        std::cerr << "Receive transaction: " << ec.message();
        return;
    }
    auto handle_deindex = [](const std::error_code& ec){
            if (ec)
                std::cerr << "Deindex error: " << ec.message();
        };

    auto handle_confirm = [this, tx, handle_deindex]( const std::error_code& ec ){
        if (ec)
            std::cerr << "Confirm error (" << /* bc::hash_transaction(tx) << */ "): " << ec.message();
        m_txidx.deindex(tx, handle_deindex);
    };

    m_txpool.store(tx, handle_confirm,
        std::bind(&ZrLibBitcoin::new_unconfirm_valid_tx, this, std::placeholders::_1, std::placeholders::_2, tx));

    node->subscribe_transaction( std::bind( &ZrLibBitcoin::recv_tx, this, std::placeholders::_1, std::placeholders::_2, node ) );
}



void ZrLibBitcoin::new_unconfirm_valid_tx( const std::error_code & ec, const bc::index_list & unconfirmed, const bc::transaction_type & tx )
{
    auto handle_index = [](const std::error_code& ec){
        if (ec)
            std::cerr << "Index error: " << ec.message() << std::endl;
    };

    const bc::hash_digest & tx_hash = hash_transaction(tx);
    if (ec){
        std::cerr  << "Error storing memory pool transaction " << /* tx_hash << */ ": " << ec.message() << std::endl;
        return;
    }

    std::cerr << "Zero Reserve: Accepted transaction ";
    if (!unconfirmed.empty())
    {
        std::cerr << "(Unconfirmed inputs";
        for (auto idx: unconfirmed)
            std::cerr << " " << idx;
        std::cerr << ") ";
    }
//    std::cerr << tx_hash;
    m_txidx.index(tx, handle_index);
}


void ZrLibBitcoin::handle_start(const std::error_code& ec)
{
    if (ec)
        std::cerr << "fullnode: " << ec.message() << std::endl;
}



ZR::RetVal ZrLibBitcoin::start()
{
    if( started) return ZR::ZR_FINISH;
    started = true;

    std::cerr << "Zero Reserve: Starting Blockchain" << std::endl;
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    std::string pathname = RsInit::RsConfigDirectory() + "/" +
            p3zr->getOwnId() + "/zeroreserve/blockchain";
    QDir zrdata ( QString::fromStdString(pathname) );
    if( !zrdata.exists() ){
        if( !zrdata.mkpath( QString::fromStdString( pathname ) ) ){
            std::cerr << "Zero Reserve: Problem creating blockchain dir: " << pathname << std::endl;
            return ZR::ZR_FAILURE;
        }
        if( ZR::ZR_SUCCESS != initChain( pathname ) ){
            return ZR::ZR_FAILURE;
        }
    }
    else{
        std::promise<std::error_code> pr_chain;
        auto blockchain_startup = [&]( const std::error_code& ec ){
                pr_chain.set_value(ec);
        };
        m_blockChain.start( pathname, blockchain_startup );
        std::error_code ec_start = pr_chain.get_future().get();
        if( ec_start ) {
            std::cerr << "Zero Reserve: Problem starting blockchain: " << ec_start.message() << std::endl;
            return ZR::ZR_FAILURE;
        }
    }

    m_protocol.subscribe_channel( std::bind( &ZrLibBitcoin::connection_started, this, std::placeholders::_1, std::placeholders::_2 ) );

    m_txpool.start();
    auto handle_start = std::bind( &ZrLibBitcoin::handle_start, this, std::placeholders::_1 );
    m_session.start(handle_start);

    return ZR::ZR_SUCCESS;
}

ZR::RetVal ZrLibBitcoin::stop()
{
    std::cerr << "Zero Reserve: Stoping Blockchain" << std::endl;

    m_netPool.stop();
    m_diskPool.stop();
    m_memPool.stop();

    m_netPool.join();
    m_diskPool.join();
    m_memPool.join();

    m_blockChain.stop();
}


ZR::RetVal ZrLibBitcoin::commit()
{
}


ZR::ZR_Number ZrLibBitcoin::getBalance()
{

}


ZR::MyWallet * ZrLibBitcoin::mkWallet(  ZR::MyWallet::WalletType wType )
{
    return new LibBitcoinWallet( wType );
}



// Wallet


LibBitcoinWallet::LibBitcoinWallet(  MyWallet::WalletType wType ) :
    ZR::MyWallet( wType )
{}


ZR::WalletSeed LibBitcoinWallet::seed()
{
    if( wallet.seed() == "" )
        wallet.new_seed();
    return wallet.seed();
}

void LibBitcoinWallet::setSeed( const ZR::WalletSeed & seed )
{
    m_walletType = ELECTRUMSEED;
    wallet.set_seed( seed );
}


ZR::BitcoinAddress LibBitcoinWallet::getAddress()
{
    bc::data_chunk pubkey = wallet.generate_public_key(2);
    bc::payment_address addr;
    bc::set_public_key(addr, pubkey);
    return addr.encoded();
}


ZR::RetVal LibBitcoinWallet::getSecret( ZR::WalletSecret & secret_out )
{
    switch( m_walletType ){
    case ELECTRUMSEED:
    {
        if( wallet.seed() == "" )
            return ZR::ZR_FAILURE;
        bc::secret_parameter secret = wallet.generate_secret( 2 );
        secret_out = bc::encode_hex(secret);
        return ZR::ZR_SUCCESS;
    }
    default:
        return ZR::ZR_FAILURE;
    }


    return ZR::ZR_FAILURE;
}


void LibBitcoinWallet::persist()
{
    ZR::WalletSecret secret;
    getSecret( secret );
    ZrDB::Instance()->addMyWallet( secret, m_walletType, m_nick );
}
