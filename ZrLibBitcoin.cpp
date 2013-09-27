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
    m_blockChain( m_diskPool )
{
}

ZR::RetVal ZrLibBitcoin::initChain( const std::string & pathname )
{
    std::promise<std::error_code> ec_chain;
    auto blockchain_startup = [&]( const std::error_code& ec ){
            ec_chain.set_value(ec);
    };
    m_blockChain.start( pathname, blockchain_startup );
    std::error_code ec = ec_chain.get_future().get();
    if( ec ) {
        std::cerr << "Problem starting blockchain: " << ec.message() << std::endl;
        return ZR::ZR_FAILURE;
    }

    bc::block_type first_block = bc::genesis_block();
    m_blockChain.import(first_block, 0, blockchain_startup);
    ec = ec_chain.get_future().get();
    if( ec ){
        std::cerr << "Importing genesis block failed: " << ec.message() << std::cerr;
        return ZR::ZR_FAILURE;
    }
    return ZR::ZR_SUCCESS;
}

ZR::RetVal ZrLibBitcoin::start()
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    std::string pathname = RsInit::RsConfigDirectory() + "/" +
            p3zr->getOwnId() + "/zeroreserve/blockchain";
    QDir zrdata ( QString::fromStdString(pathname) );
    if( !zrdata.exists() ){
        if( !zrdata.mkpath( QString::fromStdString( pathname ) ) ){
            std::cerr << "Problem creating blockchain dir: " << pathname << std::endl;
            return ZR::ZR_FAILURE;
        }
        if( ZR::ZR_SUCCESS != initChain( pathname ) ){
            return ZR::ZR_FAILURE;
        }
    }
    return ZR::ZR_SUCCESS;
}

ZR::RetVal ZrLibBitcoin::stop()
{
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

