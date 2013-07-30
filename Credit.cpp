#include "Credit.h"


#include <zrdb.h>

Credit::Credit( const std::string & id, const std::string & currencySym ) :
    m_id( id ),
    m_currency( currencySym )
{}

void Credit::updateCredit()
{
}

void Credit::loadPeer()
{
    ZrDB::Instance()->loadPeer( *this );
}
