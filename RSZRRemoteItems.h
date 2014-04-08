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


#ifndef RSZRREMOTEITEMS_H
#define RSZRREMOTEITEMS_H

#include "zrtypes.h"
#include "RSZeroReserveItems.h"
#include "Router.h"
#include "Payment.h"

#include <string>

/**
 * @brief base class of all items which go beyond friends.
 *
 * There are 2 types of remote items, one that propagates routing information to a target
 * address and one that is intended for the target address. Both go here.
 */
class RSZRRemoteItem : public RsZeroReserveItem
{
    RSZRRemoteItem();
public:

    RSZRRemoteItem( const ZR::VirtualAddress & addr, uint8_t zeroreserve_subtype ) :
        RsZeroReserveItem( zeroreserve_subtype ),
        m_Address( addr )
    {}

    RSZRRemoteItem(void *data,uint32_t size, uint8_t zeroreserve_subtype);

    virtual std::ostream& print(std::ostream &out, uint16_t indent = 0) = 0 ;

    virtual bool serialise(void *data,uint32_t& size);
    virtual uint32_t serial_size() const;

    const ZR::VirtualAddress & getAddress(){ return m_Address; }

protected:
    // depending on the item type, this can be a target address or a propagation address
    // to be added to the router
    ZR::VirtualAddress m_Address;
};


/**
 * @brief propagate a payment request through the network.
 * @see RsZeroReserveOrderBookItem
 *
 */

class RSZRPayRequestItem : public RSZRRemoteItem
{
    RSZRPayRequestItem();
public:

    RSZRPayRequestItem(void *data,uint32_t size);
    RSZRPayRequestItem(const ZR::VirtualAddress & addr, const ZR::ZR_Number &amount, const std::string & currencySymbol);

    virtual bool serialise(void *data,uint32_t& size) ;
    virtual uint32_t serial_size() const ;
    virtual std::ostream & print(std::ostream &out, uint16_t indent = 0);

    const std::string & getCurrency(){ return m_Currency; }
    const ZR::ZR_Number & getAmount(){ return m_Amount; }

private:
    ZR::ZR_Number m_Amount;
    std::string m_Currency;
};

/**
 * @brief propagate Bitcoin bid and ask orders through the network
 * @see RSZRPayRequestItem
 */

class RsZeroReserveOrderBookItem: public RSZRRemoteItem
{
    RsZeroReserveOrderBookItem();
public:
    RsZeroReserveOrderBookItem(void *data,uint32_t size) ;
    RsZeroReserveOrderBookItem( OrderBook::Order * order) ;

    virtual bool serialise(void *data,uint32_t& size) ;
    virtual uint32_t serial_size() const ;

    virtual ~RsZeroReserveOrderBookItem() {}
    virtual std::ostream& print(std::ostream &out, uint16_t indent = 0);
    OrderBook::Order * getOrder(){ return m_order; }

private:
    OrderBook::Order * m_order;
    uint32_t m_data_size ;
};

/**
 * @brief follows a route from the coordinator of a transaction (payer) to the payee and back.
 * @see RSZRRemoteTxInitItem
 */

class RSZRRemoteTxItem : public RSZRRemoteItem
{
    RSZRRemoteTxItem();
public:

    RSZRRemoteTxItem( void *data, uint32_t size, uint8_t itemType = ZR_REMOTE_TX_ITEM );
    RSZRRemoteTxItem(const ZR::VirtualAddress & addr, TransactionManager::TxPhase txPhase,
                     Router::TunnelDirection direction,
                     const OrderBook::Order::ID & payerId, uint8_t itemType = ZR_REMOTE_TX_ITEM );

    virtual bool serialise(void *data,uint32_t& size) ;
    virtual uint32_t serial_size() const ;
    virtual std::ostream & print(std::ostream &out, uint16_t indent = 0);

    TransactionManager::TxPhase getTxPhase() { return m_TxPhase; }
    Router::TunnelDirection getDirection() { return m_Direction; }
    const OrderBook::Order::ID & getPayerId(){ return m_PayerId; }
    const std::string & getPayload(){ return m_Payload; }
    void setPayload( const std::string & payload ){ m_Payload = payload; }

protected:
    TransactionManager::TxPhase m_TxPhase;
    Router::TunnelDirection m_Direction;
    OrderBook::Order::ID m_PayerId;
    std::string m_Payload;
};



/**
 * @brief route a buy request along an existing route to an order
 *
 */

class RSZRBuyRequestItem : public RSZRRemoteItem
{
    RSZRBuyRequestItem();
public:

    RSZRBuyRequestItem(void *data,uint32_t size);
    /**
     * @brief RSZRBuyRequestItem
     * @param sellerAddr    our order ID and also the route to be set pointing to us
     * @param buyerAddr     their order ID / buyer address
     * @param amount
     */
    RSZRBuyRequestItem( const ZR::VirtualAddress & sellerAddr, const ZR::VirtualAddress & buyerAddr, const ZR::ZR_Number &amount );

    virtual bool serialise(void *data,uint32_t& size) ;
    virtual uint32_t serial_size() const ;
    virtual std::ostream & print(std::ostream &out, uint16_t indent = 0);

    const ZR::VirtualAddress & getOurAddress(){ return m_SellerAddress; }
    const ZR::ZR_Number & getAmount(){ return m_Amount; }

private:
    ZR::ZR_Number m_Amount;
    ZR::VirtualAddress m_SellerAddress;
};

#endif // RSZRREMOTEITEMS_H
