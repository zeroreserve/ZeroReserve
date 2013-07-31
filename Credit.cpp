#include "Credit.h"

#include "zrdb.h"
#include "Currency.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"

#include <stdexcept>

Credit::Credit( const std::string & id, const std::string & currencySym ) :
    m_id( id ),
    m_currency( currencySym )
{
    if( Currency::getCurrencyBySymbol( currencySym) == Currency::INVALID ){
        throw std::runtime_error( "Credit::Credit(): Invalid currency symbol" );
    }
}


// TODO: Check for SQL injection

void Credit::updateCredit()
{
    ZrDB * db = ZrDB::Instance();
    if( !db->peerExists( *this )){
        db->createPeerRecord( *this );
    }
    db->updatePeerCredit( *this, "credit", m_credit );
}

void Credit::updateOurCredit()
{
    ZrDB * db = ZrDB::Instance();
    if( !db->peerExists( *this )){
        db->createPeerRecord( *this );
    }
    db->updatePeerCredit( *this, "our_credit", m_our_credit );
}

void Credit::updateBalance()
{
    ZrDB * db = ZrDB::Instance();
    if( !db->peerExists( *this )){
        db->createPeerRecord( *this );
    }
    db->updatePeerCredit( *this, "balance", m_balance );
}

void Credit::loadPeer()
{
    ZrDB::Instance()->loadPeer( *this );
}

void Credit::publish()
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    p3zr->sendCredit( this );
}
