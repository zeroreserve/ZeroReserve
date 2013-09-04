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
#include "Currency.h"

#include <string>
#include <map>

class QListWidget;

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

    class Request {
    public:
        Request( const ZR::ZR_Number & amount, const Currency::CurrencySymbols & currency) :
            m_Amount( amount ), m_Currency( currency ){}

        ZR::ZR_Number m_Amount;
        Currency::CurrencySymbols m_Currency;
    };

    typedef std::map< ZR::VirtualAddress, Request > Requests;

    Payment(const std::string & counterparty, const ZR::ZR_Number &amount, const std::string & currency, Category category);
    virtual ~Payment(){}

    virtual ZR::ZR_Number newBalance() const = 0;
    virtual int init() = 0;
    virtual int commit() = 0;

    const std::string & getCounterparty(){ return m_credit.m_id; }
    void setCounterparty( const std::string & counterparty );
    const std::string & getCurrency(){ return m_credit.m_currency; }
    ZR::ZR_Number getAmount(){ return m_amount; }
    Category getCategory(){ return m_category; }
    void referrerId( const std::string & referrer ){ m_referrer = referrer; }
    const std::string & referrerId(){ return m_referrer; }

    static void addRequest( const ZR::VirtualAddress & addr, const Request & req )
    {
        requestList.insert( std::pair< ZR::VirtualAddress, Request >( addr, req ) );
    }

    static const Request getRequest( const ZR::VirtualAddress & addr );

protected:
    Credit m_credit;
    ZR::ZR_Number m_amount;
    Category m_category;
    ZR::VirtualAddress m_referrer;

public:
    static QListWidget * txLogView;

private:
    static Requests requestList;

};


class PaymentReceiver : public Payment
{
public:
    PaymentReceiver(const std::string & counterparty, const ZR::ZR_Number &amount, const std::string & currency, Category category);
    virtual ~PaymentReceiver(){}

    virtual ZR::ZR_Number newBalance() const;
    virtual int init();
    virtual int commit();
};


class PaymentSpender : public Payment
{
public:
    PaymentSpender(const std::string & counterparty, ZR::ZR_Number amount, const std::string & currency, Category category);
    virtual ~PaymentSpender(){}

    virtual ZR::ZR_Number newBalance() const;
    virtual int init();
    virtual int commit();
};



#endif // PAYMENT_H
