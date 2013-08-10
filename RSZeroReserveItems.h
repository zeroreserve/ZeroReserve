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


#ifndef RSZERORESERVEITEMS_H
#define RSZERORESERVEITEMS_H

#include "serialiser/rsserial.h"
#include "OrderBook.h"
#include "Credit.h"
#include "TransactionManager.h"


const uint8_t QOS_PRIORITY_RS_ZERORESERVE = 3;
extern const uint16_t RS_SERVICE_TYPE_ZERORESERVE_PLUGIN;
extern const uint32_t CONFIG_TYPE_ZERORESERVE_PLUGIN;


class RsZeroReserveItem: public RsItem
{
public:
    RsZeroReserveItem(uint8_t zeroreserve_subtype)
        : RsItem(RS_PKT_VERSION_SERVICE,RS_SERVICE_TYPE_ZERORESERVE_PLUGIN, zeroreserve_subtype)
    {
        setPriorityLevel(QOS_PRIORITY_RS_ZERORESERVE) ;
    }
    enum RS_PKT_SUBTYPE {
        ZERORESERVE_ORDERBOOK_ITEM = 0x01,
        ZERORESERVE_TX_ITEM,
        ZERORESERVE_TX_INIT_ITEM,
        ZERORESERVE_CREDIT_ITEM
    };

    virtual ~RsZeroReserveItem() {};
    virtual void clear() {};
    virtual std::ostream& print(std::ostream &out, uint16_t indent = 0) = 0 ;

    virtual bool serialise(void *data,uint32_t& size) = 0 ;
    virtual uint32_t serial_size() const = 0 ;
};

class RsZeroReserveTxItem: public RsZeroReserveItem
{
    RsZeroReserveTxItem();
public:
    RsZeroReserveTxItem( RS_PKT_SUBTYPE zeroreserve_subtype = ZERORESERVE_TX_ITEM) :
        RsZeroReserveItem( (uint8_t)zeroreserve_subtype ) {}

    RsZeroReserveTxItem(void *data,uint32_t size, RS_PKT_SUBTYPE zeroreserve_subtype = ZERORESERVE_TX_ITEM );
    RsZeroReserveTxItem( TransactionManager::TxPhase phase, RS_PKT_SUBTYPE subtype = ZERORESERVE_TX_ITEM );
    void setTxId( const std::string & id ){ m_txId = id; }
    const std::string & getTxId(){ return m_txId; }

    virtual bool serialise(void *data,uint32_t& size) ;
    virtual uint32_t serial_size() const ;

    virtual ~RsZeroReserveTxItem() {}
    virtual std::ostream& print(std::ostream &out, uint16_t indent = 0);

    TransactionManager::TxPhase getTxPhase() { return m_TxPhase; }

protected:
    TransactionManager::TxPhase m_TxPhase;
    uint32_t m_offset;
    std::string m_txId;

};


class RsZeroReserveInitTxItem: public RsZeroReserveTxItem
{
    RsZeroReserveInitTxItem();
public:
    RsZeroReserveInitTxItem(void *data,uint32_t size) ;
    RsZeroReserveInitTxItem( Payment * payment );

    virtual bool serialise(void *data,uint32_t& size) ;
    virtual uint32_t serial_size() const ;

    virtual ~RsZeroReserveInitTxItem();
    virtual std::ostream& print(std::ostream &out, uint16_t indent = 0);

    Payment * getPayment();
    TransactionManager::Role getRole() { return m_Role; }

private:
    TransactionManager::Role m_Role;
    Payment * m_payment;
};


class RsZeroReserveOrderBookItem: public RsZeroReserveItem
{
public:
    RsZeroReserveOrderBookItem() :RsZeroReserveItem( ZERORESERVE_ORDERBOOK_ITEM ) {}
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


class RsZeroReserveCreditItem: public RsZeroReserveItem
{
public:
    RsZeroReserveCreditItem() :RsZeroReserveItem( ZERORESERVE_CREDIT_ITEM ) {}
    RsZeroReserveCreditItem(void *data,uint32_t size) ;
    RsZeroReserveCreditItem( Credit * credit) ;

    virtual bool serialise(void *data,uint32_t& size) ;
    virtual uint32_t serial_size() const ;

    virtual ~RsZeroReserveCreditItem();
    virtual std::ostream& print(std::ostream &out, uint16_t indent = 0);
    Credit * getCredit(){ return m_credit; }

private:
    Credit * m_credit;
};


class RsZeroReserveSerialiser: public RsSerialType
{
public:
    RsZeroReserveSerialiser()
        :RsSerialType(RS_PKT_VERSION_SERVICE, RS_SERVICE_TYPE_ZERORESERVE_PLUGIN)
    {
    }
    virtual ~RsZeroReserveSerialiser() {}

    virtual uint32_t 	size (RsItem *item)
    {
        return dynamic_cast<RsZeroReserveItem *>(item)->serial_size() ;
    }

    virtual	bool serialise  (RsItem *item, void *data, uint32_t *size)
    {
        return dynamic_cast<RsZeroReserveItem *>(item)->serialise(data,*size) ;
    }
    virtual	RsItem *deserialise(void *data, uint32_t *size);
};

#endif // RSZERORESERVEITEMS_H
