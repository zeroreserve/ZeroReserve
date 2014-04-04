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


#ifndef TMCONTRACT_H
#define TMCONTRACT_H

#include "TransactionManager.h"
#include "zrtypes.h"

/**
 * @brief Remote contract transaction.
 * @see   BtcContract
 * This class implements a 3PC transaction. The result will be a series of contracts
 * between all friends in a payment.
 */

class TmContract : public TransactionManager
{
public:
    TmContract( const ZR::VirtualAddress & addr, const std::string & myId );
    virtual ~TmContract(){}

    virtual ZR::RetVal init() = 0;
    virtual void rollback() = 0;

};


class TmContractCoordinator : public TmContract
{
public:
    TmContractCoordinator( const ZR::VirtualAddress & addr, const std::string & myId );
    virtual ~TmContractCoordinator(){}

    virtual ZR::RetVal init();
    virtual void rollback();

private:
    ZR::VirtualAddress m_Destination;
    std::string m_myId;
};


class TmContractCohorte : public TmContract
{
public:
    TmContractCohorte( const ZR::VirtualAddress & addr, const std::string & myId );
    virtual ~TmContractCohorte(){}

    virtual ZR::RetVal init();
    virtual void rollback();

};

#endif // TMCONTRACT_H
