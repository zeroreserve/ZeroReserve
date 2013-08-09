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

#include "Payment.h"
#include "MyOrders.h"
#include "zrtypes.h"

#include <stdlib.h>


Payment::Payment( const std::string & counterparty, const std::string & amount, const std::string & currency, Category category) :
    m_counterparty( counterparty ),
    m_amount( amount ),
    m_currency( currency ),
    m_category( category )
{
}


/////// PaymentReceiver

PaymentReceiver::PaymentReceiver( const std::string & counterparty, const std::string & amount, const std::string & currency, Category category) :
    Payment( counterparty, amount, currency, category)
{}


ZR::ZR_Number PaymentReceiver::newBalance( const Credit * credit ) const
{
    ZR::ZR_Number amount = atof( m_amount.c_str() );
    ZR::ZR_Number balance = atof( credit->m_balance.c_str() );
    return balance + amount;
}


int PaymentReceiver::init()
{
    switch( m_category )
    {
    case BITCOIN:
        return MyOrders::Instance()->startExecute();
    case PAYMENT:
        return ZR::ZR_SUCCESS;
    default:
        return ZR::ZR_FAILURE;
    }
}

int PaymentReceiver::commit()
{
    switch( m_category )
    {
    case BITCOIN:
        return MyOrders::Instance()->finishExecute( this );
    case PAYMENT:
        return ZR::ZR_SUCCESS;
    default:
        return ZR::ZR_FAILURE;
    }
}


/////// PaymentSpender

PaymentSpender::PaymentSpender( const std::string & counterparty, const std::string & amount, const std::string & currency, Category category) :
    Payment( counterparty, amount, currency, category)
{}

ZR::ZR_Number PaymentSpender::newBalance( const Credit * credit ) const
{
    ZR::ZR_Number amount = atof( m_amount.c_str() );
    ZR::ZR_Number balance = atof( credit->m_balance.c_str() );
    return balance - amount;
}

int PaymentSpender::init()
{
    return ZR::ZR_SUCCESS;
}

int PaymentSpender::commit()
{
    return ZR::ZR_SUCCESS;
}

