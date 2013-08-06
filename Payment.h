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

#ifndef PAYMENT_H
#define PAYMENT_H

#include "zrtypes.h"
#include "Credit.h"

#include <string>


/**
  * Take care of the actual payment on behalf of the @TransactionManager
  */

class Payment
{
    Payment();
public:
    enum Category {
        PAYMENT,      // generic payment where the other leg of the deal is outside the system
        DEBT_CANCEL,  // triangle payments with the aim to cancel out debt
        BITCOIN
    };

    Payment( const std::string & counterparty, const std::string & amount, const std::string & currency, Category category);
    virtual ~Payment(){}

    virtual ZR_Number newBalance( const Credit * credit ) const = 0;
    virtual void init() = 0;
    virtual void commit() = 0;

    const std::string & getCounterparty(){ return m_counterparty; }
    void setCounterparty( const std::string & counterparty ){ m_counterparty = counterparty; }
    const std::string & getCurrency(){ return m_currency; }
    const std::string & getAmount(){ return m_amount; }
    Category getCategory(){ return m_category; }

protected:
    std::string m_counterparty;
    std::string m_amount;
    std::string m_currency;
    Category m_category;
    std::string text;  // this is freeform data which is the category


};


class PaymentReceiver : public Payment
{
public:
    PaymentReceiver( const std::string & counterparty, const std::string & amount, const std::string & currency, Category category);
    virtual ~PaymentReceiver(){}

    virtual ZR_Number newBalance(const Credit * credit ) const;
    virtual void init();
    virtual void commit();
};


class PaymentSpender : public Payment
{
public:
    PaymentSpender( const std::string & counterparty, const std::string & amount, const std::string & currency, Category category);
    virtual ~PaymentSpender(){}

    virtual ZR_Number newBalance( const Credit * credit ) const;
    virtual void init();
    virtual void commit();
};



#endif // PAYMENT_H
