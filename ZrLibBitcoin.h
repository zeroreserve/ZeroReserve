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

#ifndef ZRLIBBITCOIN_H
#define ZRLIBBITCOIN_H

#include "ZRBitcoin.h"
#include "zrtypes.h"

#include <bitcoin/bitcoin.hpp>

#include <string>

class ZrLibBitcoin : public ZR::Bitcoin
{
public:
    ZrLibBitcoin();
    virtual ZR::RetVal start();
    virtual ZR::RetVal stop();
    virtual ZR::RetVal commit();
    virtual ZR::ZR_Number getBalance();

    virtual ZR::MyWallet *mkWallet( ZR::MyWallet::WalletType wType );

private:
    ZR::RetVal initChain( const std::string & pathname );
    void connection_started(const std::error_code& ec, bc::channel_ptr node);
    void recv_tx( const std::error_code& ec, const bc::transaction_type & tx, bc::channel_ptr node );
    void new_unconfirm_valid_tx( const std::error_code & ec, const bc::index_list & unconfirmed, const bc::transaction_type & tx );
    void handle_start(const std::error_code& ec);

    bc::threadpool m_netPool;
    bc::threadpool m_diskPool;
    bc::threadpool m_memPool;

    bc::hosts m_hosts;
    bc::handshake m_handshake;
    bc::network m_network;
    bc::protocol m_protocol;

    bc::poller m_poller;
    bc::transaction_pool m_txpool;
    bc::transaction_indexer m_txidx;

    bc::session m_session;

    bc::leveldb_blockchain m_blockChain;

    bool started;
};

class LibBitcoinWallet : public ZR::MyWallet
{
    friend ZrLibBitcoin;
    LibBitcoinWallet(  MyWallet::WalletType wType );
public:

    virtual ZR::WalletSeed seed();
    virtual void setSeed( const ZR::WalletSeed & seed );
    virtual ZR::BitcoinAddress getAddress();


    virtual ZR::RetVal getSecret( ZR::WalletSecret &secret_out  );

private:
    bc::deterministic_wallet wallet;
    bc::elliptic_curve_key privkey;
};

#endif // ZRLIBBITCOIN_H
