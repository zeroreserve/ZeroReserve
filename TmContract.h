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
#include "OrderBook.h"
#include "zrtypes.h"

class RSZRRemoteTxItem;
class BtcContract;

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
    TmContractCoordinator( const OrderBook::Order * order, const ZR::ZR_Number & amount, const std::string & myId );
    virtual ~TmContractCoordinator(){}

    virtual ZR::RetVal init();
    virtual void rollback();

private:
    ZR::VirtualAddress m_Destination;
    std::string m_myId;
    BtcContract * payer;
};


class TmContractCohortePayee : public TmContract
{
public:
    TmContractCohortePayee( const ZR::VirtualAddress & addr, const std::string & myId );
    virtual ~TmContractCohortePayee(){}

    virtual ZR::RetVal processItem( RSZRRemoteTxItem *item );
    virtual ZR::RetVal init();
    virtual void rollback();

private:
    ZR::RetVal doQuery( RSZRRemoteTxItem * item );
    ZR::RetVal doCommit( RSZRRemoteTxItem * item );

    BtcContract * payee;
};


class TmContractCohorteHop : public TmContract
{
public:
    TmContractCohorteHop( const ZR::VirtualAddress & addr, const std::string & myId );
    virtual ~TmContractCohorteHop(){}

    virtual ZR::RetVal processItem( RSZRRemoteTxItem *item );
    virtual ZR::RetVal init();
    virtual void rollback();

private:
    ZR::RetVal doQuery( RSZRRemoteTxItem * item );
    ZR::RetVal doCommit( RSZRRemoteTxItem * item );
    ZR::RetVal doVote( RSZRRemoteTxItem * item );


    BtcContract * payer;
    BtcContract * payee;
};

#endif // TMCONTRACT_H
