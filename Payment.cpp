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


ZR_Number PaymentReceiver::newBalance( const Credit * credit ) const
{
    ZR_Number amount = atof( m_amount.c_str() );
    ZR_Number balance = atof( credit->m_balance.c_str() );
    return balance + amount;
}


void PaymentReceiver::init()
{}

void PaymentReceiver::commit()
{}


/////// PaymentSpender

PaymentSpender::PaymentSpender( const std::string & counterparty, const std::string & amount, const std::string & currency, Category category) :
    Payment( counterparty, amount, currency, category)
{}

ZR_Number PaymentSpender::newBalance( const Credit * credit ) const
{
    ZR_Number amount = atof( m_amount.c_str() );
    ZR_Number balance = atof( credit->m_balance.c_str() );
    return balance - amount;
}

void PaymentSpender::init()
{}

void PaymentSpender::commit()
{}

