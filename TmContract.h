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
 * @see   TmContractCoordinator
 * @see   TmContractCohortePayee
 * @see   TmContractCohorteHop
 * Subclasses of this interface implement a 1 1/2 PC protocol. The COMMIT phase results in a series of contracts
 * between all hops in a payment chain. This contract will get fulfilled when a particular
 * condition on the Bitcoin blockchain is met. The contract times out if the condition is not met
 * within a timeout period.
 */

class TmContract : public TransactionManager
{
public:
    TmContract( const ZR::VirtualAddress & addr, const std::string & myId );
    virtual ~TmContract(){}

    virtual ZR::RetVal init() = 0;
    virtual void rollback() = 0;

};

/**
 * @brief The TmContractCoordinator class
 * @see TmContract
 * @see   TmContractCohortePayee
 * @see   TmContractCohorteHop
 *
 * Implement the TmContract interface.
 * Phase QUERY:    Submit relevant TX data for both hops and payee
 * Phase VOTE_YES: Nodes indicate that they are OK with the TX, maybe with some changes, like lowering the amount
 * Phase COMMIT:   Commit to the contract as implemented in @see BtcContract
 */

class TmContractCoordinator : public TmContract
{
public:
    /**
     * @brief TmContractCoordinator
     * @param order the seller's order
     * @param amount Bitcoin amount to buy as part of the seller's order
     * @param myId my order id
     */
    TmContractCoordinator( const OrderBook::Order * order, const ZR::ZR_Number & amount, const std::string & myId );
    virtual ~TmContractCoordinator(){}

    virtual ZR::RetVal init();
    virtual void rollback();
    virtual ZR::RetVal processItem( RSZRRemoteTxItem * item );


private:
    ZR::RetVal doTx( RSZRRemoteTxItem * item );

    // unlike the other TM, which request an abort on calling this function, a coordinator aborts.
    ZR::RetVal abortTx( RSZRRemoteTxItem *item );

    ZR::VirtualAddress m_Destination;
    std::string m_myId;
    BtcContract * m_payer;
    ZR::BitcoinAddress m_btcAddr;
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

    // request an abort
    ZR::RetVal abortTx( RSZRRemoteTxItem *item );

    BtcContract * m_payee;
    ZR::BitcoinTxHex m_txHex;
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
    ZR::RetVal forwardItem( RSZRRemoteTxItem * item );
    void mkTunnel( RSZRRemoteTxItem * item );

    // request an abort
    ZR::RetVal abortTx( RSZRRemoteTxItem *item );

    BtcContract * m_payer;
    BtcContract * m_payee;
};

#endif // TMCONTRACT_H
