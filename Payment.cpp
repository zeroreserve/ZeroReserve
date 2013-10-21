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
#include <QDateTime>

#include <sstream>

#include <stdlib.h>


QListWidget * Payment::txLogView = NULL;

Payment::Requests Payment::requestList;
Payment::Requests Payment::myRequests;


/// @brief Payment constructor
///
/// @param counterparty
/// @param amount
/// @param currency
/// @param category
Payment::Payment( const std::string & counterparty, const ZR::ZR_Number & amount, const std::string & currency, Category category) :
    m_credit( counterparty, currency ),
    m_amount( amount ),
    m_category( category )
{
    m_credit.loadPeer();
}

/// @brief Set counter party for payment
///
/// @param counterparty
void Payment::setCounterparty( const std::string & counterparty )
{
    m_credit.m_id = counterparty;
    m_credit.loadPeer();
}


/// @brief Get request for payment
///
/// @param addr
///
/// @return 
const Payment::Request Payment::getRequest( const ZR::VirtualAddress & addr )
{
    Requests::iterator it = requestList.find( addr );
    if( it == requestList.end() )
        return Request( 0, Currency::INVALID );
    return (*it).second;
}

/// @brief Get my request for payment
///
/// @param addr
///
/// @return 
const Payment::Request Payment::getMyRequest( const ZR::VirtualAddress & addr )
{
    Requests::iterator it = myRequests.find( addr );
    if( it == myRequests.end() )
        return Request( 0, Currency::INVALID );
    return (*it).second;
}

/////// PaymentReceiver

/// @brief Payment receiver constructor
///
/// @param counterparty
/// @param amount
/// @param currency
/// @param category
PaymentReceiver::PaymentReceiver( const std::string & counterparty, const ZR::ZR_Number & amount, const std::string & currency, Category category) :
    Payment( counterparty, amount, currency, category)
{}


/// @brief New balance
///
/// @return 
ZR::ZR_Number PaymentReceiver::newBalance() const
{
    return m_credit.m_balance + m_amount;
}


/// @brief Initialise
///
/// @return 
int PaymentReceiver::init()
{
    switch( m_category )
    {
    case BITCOIN:
        if( m_credit.getPeerAvailable() <= 0 )
            return ZR::ZR_FAILURE;
        if( m_amount > m_credit.getPeerAvailable() ){
            m_amount = m_credit.getPeerAvailable();
        }
        return MyOrders::Instance()->startExecute( this );
    case PAYMENT:
        if( m_credit.getPeerAvailable() < m_amount ) return ZR::ZR_FAILURE;
        return ZR::ZR_SUCCESS;
    default:
        return ZR::ZR_FAILURE;
    }
}

/// @brief Commit
//
/// @todo TODO: make atomic! - refer to code
///
/// @param txId
///
/// @return 
int PaymentReceiver::commit( const ZR::TransactionId &txId )
{
    m_credit.loadPeer();
    m_credit.m_balance = newBalance();
    // TODO: make atomic !!!!
    ZrDB::Instance()->updatePeerCredit( m_credit, "balance", m_credit.m_balance );
    ZrDB::Instance()->appendTx( m_credit.m_id, m_credit.m_currency, m_amount );

    if( txLogView )
    {
        txLogView->insertItem( 0, QDateTime::currentDateTime().toString() + " : " + m_credit.m_currency.c_str() + " : +" + m_amount.toDecimalQString() );
        txLogView->setCurrentRow( 0 ); // make the view emit currentItemChanged()
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

/// @brief Constructor
///
/// @param counterparty
/// @param amount
/// @param currency
/// @param category
PaymentSpender::PaymentSpender(const std::string & counterparty, ZR::ZR_Number amount, const std::string & currency, Category category) :
    Payment( counterparty, amount, currency, category)
{}

/// @brief New balance of spending some coin
///
/// @return 
ZR::ZR_Number PaymentSpender::newBalance() const
{
    return m_credit.m_balance - m_amount;
}

/// @brief Initialise payment spender
///
/// @return 
int PaymentSpender::init()
{
    if( m_credit.getMyAvailable() < m_amount )
    {
        return ZR::ZR_FAILURE;
    }
    return ZR::ZR_SUCCESS;
}

/// @brief Commit the transaction id
///
/// @todo Make atomic - refer to the code
//
/// @param txId
///
/// @return 
int PaymentSpender::commit( const ZR::TransactionId & txId )
{
    m_credit.loadPeer();
    m_credit.m_balance = newBalance();
    // TODO: make atomic !!!!
    ZrDB::Instance()->updatePeerCredit( m_credit, "balance", m_credit.m_balance );
    ZrDB::Instance()->appendTx( m_credit.m_id, m_credit.m_currency,  -m_amount );

    if( txLogView )
    {
        txLogView->insertItem( 0, QDateTime::currentDateTime().toString() + " : " + m_credit.m_currency.c_str() + " : -" + m_amount.toDecimalQString() );
        txLogView->setCurrentRow( 0 ); // make the view emit currentItemChanged()
    }

    switch( m_category )
    {
    case BITCOIN:
        return MyOrders::Instance()->updateOrders( this, txId );
    case PAYMENT:
        return ZR::ZR_SUCCESS;
    default:
        return ZR::ZR_FAILURE;
    }
    return ZR::ZR_SUCCESS;
}

//   EOF   
