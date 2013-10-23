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

#include <boost/lexical_cast.hpp>

#include <string>
#include <future>
#include <unordered_set>




//////////////////////////////////////////////////////////////////////////////////

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using namespace bc;

// warning: this is not good code!
std::mutex broadcast_mutex;
std::vector<bc::transaction_type> tx_broadcast_queue;

class wallet_control;

typedef boost::circular_buffer<std::string> string_buffer;

typedef std::unordered_set<bc::payment_address> unique_address_set;

struct wallet_history_entry
{
    bool is_credit;
    bc::output_point point;
    size_t block_height;
    std::string address;
    uint64_t amount;
};



struct address_cycler
{
    const bc::payment_address address(size_t sequence, bool change)
    {
        bc::payment_address addr;
        bc::set_public_key(addr,
            detwallet.generate_public_key(sequence, change));
        return addr;
    }
    const bc::payment_address address()
    {
        return address(n, false);
    }

    size_t n = 0;
    bc::deterministic_wallet detwallet;
};

namespace std
{
    template <>
    struct hash<bc::output_point>
    {
        size_t operator()(const bc::output_point& outpoint) const
        {
            std::string raw;
            raw.resize(hash_digest_size + 4);
            auto serial = bc::make_serializer(raw.begin());
            serial.write_hash(outpoint.hash);
            serial.write_4_bytes(outpoint.index);
            std::hash<std::string> hash_fn;
            return hash_fn(raw);
        }
    };
}

class wallet_control
{
public:
    wallet_control( address_cycler& addr_cycler)
      : addr_cycler_(addr_cycler)
    {}

    void next_address()
    {
        ++addr_cycler_.n;
    }
    void previous_address()
    {
        if (addr_cycler_.n == 0)
            return;
        --addr_cycler_.n;
    }

    void add_address(const bc::payment_address& addr)
    {
        our_addrs_.insert(addr);
    }
    bool is_ours(const bc::payment_address& addr)
    {
        return our_addrs_.find(addr) != our_addrs_.end();
    }

    void add_unspent(const output_point& outpoint,
        const bc::payment_address& addr, uint64_t value)
    {
        unspent_[outpoint] = output_data{addr, value};
    }

    select_outputs_result find_unspent(uint64_t value)
    {
        // Generate unspent out list.
        output_info_list outs;
        for (const auto& pair: unspent_)
        {
            outs.push_back({pair.first, pair.second.value});
        }
        return bc::select_outputs(outs, value);
    }

    // For finding the right signing key.
    const bc::payment_address& lookup(const output_point& outpoint)
    {
        return unspent_[outpoint].addr;
    }

    const bc::payment_address change_address()
    {
        return addr_cycler_.address(addr_cycler_.n, true);
    }

    void add_key(const bc::payment_address& addr,
        const bc::secret_parameter& secret)
    {
        privkeys_[addr] = secret;
    }
    const bc::secret_parameter lookup(const bc::payment_address& addr)
    {
        return privkeys_[addr];
    }

private:
    struct output_data
    {
        bc::payment_address addr;
        uint64_t value;
    };

    typedef std::unordered_map<output_point, output_data> output_info_map;

    typedef std::unordered_map<bc::payment_address, bc::secret_parameter>
        keys_map;

    address_cycler& addr_cycler_;
    unique_address_set our_addrs_;
    output_info_map unspent_;
    keys_map privkeys_;
};



void history_fetched(const std::error_code& ec,
    const bc::blockchain::history_list& history,
    wallet_control& control, const std::string& btc_address)
{
    if (ec)
    {
        //std::cerr << "history: Failed to fetch history: "
        //    << ec.message() << std::endl;
        return;
    }
    uint64_t balance = 0;
    for (const auto& row: history)
    {
        if (row.spend.hash == bc::null_hash)
        {
            bc::payment_address addr;
            addr.set_encoded(btc_address);
            control.add_unspent(row.output, addr, row.value);
            balance += row.value;
            continue;
        }
    }
}


void new_update(const std::error_code& ec, size_t height,
    const bc::hash_digest& block_hash, const bc::transaction_type& tx)
{
    //std::cout << "Update " << bc::encode_hex(bc::hash_transaction(tx))
    //    << " [ #" << height << " "
    //    << bc::encode_hex(block_hash) << " ]" << std::endl;
}

// omg hacks
bool stopped = false;

// Maybe should also be in libbitcoin too?
script_type build_pubkey_hash_script(const short_hash& pubkey_hash)
{
    script_type result;
    result.push_operation({opcode::dup, data_chunk()});
    result.push_operation({opcode::hash160, data_chunk()});
    result.push_operation({opcode::special,
        data_chunk(pubkey_hash.begin(), pubkey_hash.end())});
    result.push_operation({opcode::equalverify, data_chunk()});
    result.push_operation({opcode::checksig, data_chunk()});
    return result;
}

script_type build_script_hash_script(const short_hash& script_hash)
{
    script_type result;
    result.push_operation({opcode::hash160, data_chunk()});
    result.push_operation({opcode::special,
        data_chunk(script_hash.begin(), script_hash.end())});
    result.push_operation({opcode::equal, data_chunk()});
    return result;
}

bool build_output_script(
    script_type& out_script, const payment_address& payaddr)
{
    switch (payaddr.version())
    {
        case payment_address::pubkey_version:
            out_script = build_pubkey_hash_script(payaddr.hash());
            return true;

        case payment_address::script_version:
            out_script = build_script_hash_script(payaddr.hash());
            return true;
    }
    return false;
}

bool make_signature(transaction_type& tx, size_t input_index,
    const elliptic_curve_key& key, const script_type& script_code)
{
    transaction_input_type& input = tx.inputs[input_index];

    const data_chunk public_key = key.public_key();
    if (public_key.empty())
        return false;
    hash_digest tx_hash =
        script_type::generate_signature_hash(tx, input_index, script_code, 1);
    if (tx_hash == null_hash)
        return false;
    data_chunk signature = key.sign(tx_hash);
    signature.push_back(0x01);
    //std::cout << signature << std::endl;
    script_type& script = tx.inputs[input_index].script;
    // signature
    script.push_operation({opcode::special, signature});
    // public key
    script.push_operation({opcode::special, public_key});
    return true;
}

bc::hash_digest send(wallet_control& control, std::vector<std::string>& strs,
    string_buffer& console_output)
{
    if (strs.size() != 3 && strs.size() != 4)
    {
        console_output.push_back("send: Wrong number of arguments.");
        return null_hash;
    }
    bc::payment_address dest_addr;
    if (!dest_addr.set_encoded(strs[1]))
    {
        console_output.push_back("send: Invalid address.");
        return null_hash;
    }
    uint64_t amount;
    try
    {
        amount = boost::lexical_cast<uint64_t>(strs[2]);
    }
    catch (const boost::bad_lexical_cast&)
    {
        console_output.push_back("send: Invalid amount.");
        return null_hash;
    }
    // Fee of 0.0001 BTC = 10000 Sat
    uint64_t fee = 10000;
    if (strs.size() == 4)
    {
        try
        {
            fee = boost::lexical_cast<uint64_t>(strs[3]);
        }
        catch (const boost::bad_lexical_cast&)
        {
            console_output.push_back("send: Invalid fee.");
            return null_hash;
        }
    }
    console_output.push_back("Sending...");
    select_outputs_result unspent = control.find_unspent(amount + fee);
    if (unspent.points.empty())
    {
        console_output.push_back("send: Not enough funds.");
        return null_hash;
    }
    // construct transaction now.
    transaction_type tx;
    tx.version = 1;
    tx.locktime = 0;
    // start with outputs.
    // dest addr output first.
    transaction_output_type dest_output;
    dest_output.value = amount;
    if (!build_output_script(dest_output.script, dest_addr))
    {
        console_output.push_back("send: Unsupported address type.");
        return null_hash;
    }
    tx.outputs.push_back(dest_output);
    // add change output also.
    transaction_output_type change_output;
    change_output.value = unspent.change;
    bc::payment_address change_addr = control.change_address();
    bool change_script_success =
        build_output_script(change_output.script, change_addr);
    BITCOIN_ASSERT(change_script_success);
    tx.outputs.push_back(change_output);
    // notice we have left the fee out.
    // now do inputs.
    for (const bc::output_point& prevout: unspent.points)
    {
        transaction_input_type input;
        input.previous_output = prevout;
        input.sequence = 4294967295;
        tx.inputs.push_back(input);
    }
    // now sign inputs
    for (size_t i = 0; i < tx.inputs.size(); ++i)
    {
        bc::transaction_input_type& input = tx.inputs[i];
        const bc::payment_address addr = control.lookup(input.previous_output);
        const bc::secret_parameter secret = control.lookup(addr);
        elliptic_curve_key key;
        bool set_secret_success = key.set_secret(secret);
        BITCOIN_ASSERT(set_secret_success);
        payment_address address;
        set_public_key(address, key.public_key());
        script_type prevout_script_code;
        bool prevout_script_code_success =
            build_output_script(prevout_script_code, address);
        BITCOIN_ASSERT(prevout_script_code_success);
        bool sign_success = make_signature(tx, i, key, prevout_script_code);
    }
    // holy shit! now lets broadcast the tx!
    broadcast_mutex.lock();
    tx_broadcast_queue.push_back(tx);
    broadcast_mutex.unlock();
    return bc::hash_transaction(tx);
}



void handle_start(const std::error_code& ec)
{

}

void output_to_file(std::ofstream& file, bc::log_level level,
    const std::string& domain, const std::string& body)
{
    if (body.empty())
        return;
    file << bc::level_repr(level);
    if (!domain.empty())
        file << " [" << domain << "]";
    file << ": " << body << std::endl;
}

// warning: this is not good code!
void broadcast_subsystem()
{
    std::ofstream outfile("wallet.log");
    log_debug().set_output_function(
        std::bind(output_to_file, std::ref(outfile), _1, _2, _3));
    log_info().set_output_function(
        std::bind(output_to_file, std::ref(outfile), _1, _2, _3));
    log_warning().set_output_function(
        std::bind(output_to_file, std::ref(outfile), _1, _2, _3));
    log_error().set_output_function(
        std::bind(output_to_file, std::ref(outfile), _1, _2, _3));
    log_fatal().set_output_function(
        std::bind(output_to_file, std::ref(outfile), _1, _2, _3));
    threadpool pool(4);
    // Create dependencies for our protocol object.
    hosts hst(pool);
    handshake hs(pool);
    network net(pool);
    // protocol service.
    protocol prot(pool, hst, hs, net);
    prot.set_max_outbound(4);
    // Perform node discovery if needed and then creating connections.
    prot.start(handle_start);
    // wait
    while (!stopped)
    {
        sleep(0.2);
        // if any new shit then broadcast it.
        broadcast_mutex.lock();
        if (tx_broadcast_queue.empty())
        {
            broadcast_mutex.unlock();
            continue;
        }
        transaction_type tx = tx_broadcast_queue.back();
        tx_broadcast_queue.pop_back();
        broadcast_mutex.unlock();
        auto ignore_send = [](const std::error_code&, size_t) {};
        prot.broadcast(tx, ignore_send);
    }
    auto ignore_stop = [](const std::error_code&) {};
    prot.stop(ignore_stop);
    // Safely close down.
    pool.stop();
    pool.join();
}



//////////////////////////////////////////////////////////////////////////////////




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

void ZrLibBitcoin::loadWallets( std::vector< ZR::MyWallet *> & wallets )
{
    std::vector< ZrDB::MyWallet > dbWallets;
    ZrDB::Instance()->loadMyWallets( dbWallets );
    for( std::vector< ZrDB::MyWallet >::const_iterator it = dbWallets.begin(); it != dbWallets.end(); it++ ){
        LibBitcoinWallet * wallet;
        ZR::MyWallet::WalletType type = (ZR::MyWallet::WalletType) (*it).type;
        switch( type ){
        case ZR::MyWallet::ELECTRUMSEED:
            wallet = new LibBitcoinWallet( (ZR::MyWallet::WalletType) (*it).type );
            wallet->setSeed( (*it).secret );
            wallet->setNick( (*it).nick );
            break;
        default:
            delete wallet;
            continue;
        }
        wallets.push_back( wallet );
    }
}



////////////////// Wallet ////////////////////////////////////


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


ZR::RetVal LibBitcoinWallet::persist()
{
    switch( m_walletType )
    {
    case ELECTRUMSEED:
        return ZrDB::Instance()->storeMyWallet( wallet.seed(), m_walletType, m_nick );
    case WIFIMPORT:
        ZR::WalletSecret secret;
        getSecret( secret );
        return ZrDB::Instance()->storeMyWallet( secret, m_walletType, m_nick );
    }
}
