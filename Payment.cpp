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
#include "zrdb.h"

#include <QListWidget>

#include <sstream>

#include <stdlib.h>


QListWidget * Payment::txLogView = NULL;


Payment::Payment( const std::string & counterparty, const std::string & amount, const std::string & currency, Category category) :
    m_credit( counterparty, currency ),
    m_amount( amount ),
    m_category( category )
{
    m_credit.loadPeer();
}

void Payment::setCounterparty( const std::string & counterparty )
{
    m_credit.m_id = counterparty;
    m_credit.loadPeer();
}


/////// PaymentReceiver

PaymentReceiver::PaymentReceiver( const std::string & counterparty, const std::string & amount, const std::string & currency, Category category) :
    Payment( counterparty, amount, currency, category)
{}


ZR::ZR_Number PaymentReceiver::newBalance() const
{
    ZR::ZR_Number amount = atof( m_amount.c_str() );
    ZR::ZR_Number balance = atof( m_credit.m_balance.c_str() );
    return balance + amount;
}


int PaymentReceiver::init()
{
    if( ( atof( m_credit.m_credit.c_str()) - newBalance( ) ) < 0 ){
        return ZR::ZR_FAILURE;
    }

    switch( m_category )
    {
    case BITCOIN:
        return MyOrders::Instance()->startExecute( this );
    case PAYMENT:
        return ZR::ZR_SUCCESS;
    default:
        return ZR::ZR_FAILURE;
    }
}

int PaymentReceiver::commit()
{
    m_credit.loadPeer();
    std::ostringstream balance;
    balance << newBalance();
    m_credit.m_balance = balance.str();
    // TODO: make atomic !!!!
    ZrDB::Instance()->updatePeerCredit( m_credit, "balance", m_credit.m_balance );
    ZrDB::Instance()->appendTx( m_credit.m_id, m_amount );

    if( txLogView ){
        txLogView->insertItem( 0, QString::fromStdString( m_credit.m_id + " : " + m_credit.m_currency + " : +" + m_amount ) );
    }

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

ZR::ZR_Number PaymentSpender::newBalance() const
{
    ZR::ZR_Number amount = atof( m_amount.c_str() );
    ZR::ZR_Number balance = atof( m_credit.m_balance.c_str() );
    return balance - amount;
}

int PaymentSpender::init()
{
    if( atof( m_credit.m_our_credit.c_str()) + newBalance() < 0 ){
        return ZR::ZR_FAILURE;
    }
    return ZR::ZR_SUCCESS;
}

int PaymentSpender::commit()
{
    m_credit.loadPeer();
    std::ostringstream balance;
    balance << newBalance();
    m_credit.m_balance = balance.str();
    // TODO: make atomic !!!!
    ZrDB::Instance()->updatePeerCredit( m_credit, "balance", m_credit.m_balance );
    ZrDB::Instance()->appendTx( m_credit.m_id, std::string("-") + m_amount );

    if( txLogView ){
        txLogView->insertItem( 0, QString::fromStdString( m_credit.m_id + " : " + m_credit.m_currency + " : -" + m_amount ) );
    }

    return ZR::ZR_SUCCESS;
}

