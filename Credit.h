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

#ifndef CREDIT_H
#define CREDIT_H

#include "zrtypes.h"

#include <string>
#include <list>

class Credit
{
    Credit();
public:
    typedef std::list< Credit * > CreditList;


    Credit( const std::string & id, const std::string & currencySym );

    void updateCredit();
    void loadPeer();
    void publish();
    void updateOurCredit();
    void updateBalance();
    void allocate( const ZR::ZR_Number & amount);
    void deallocate( const ZR::ZR_Number & amount);

    static void getCreditList( CreditList & outList, const std::string & id );

    ZR::ZR_Number getPeerAvailable(){ return m_credit - m_balance - m_allocated; }
    ZR::ZR_Number getMyAvailable(){ return m_our_credit + m_balance; }

public:
    std::string m_id;
    std::string m_currency;
    ZR::ZR_Number m_our_credit; // our credit with peer
    ZR::ZR_Number m_credit;     // their credit with us
    ZR::ZR_Number m_balance;    // negative means we owe them money
    ZR::ZR_Number m_allocated;  // funds allocated to ongoing contracts / TX. Always positive
};

#endif // CREDIT_H
