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


#include "RSZeroReserveItems.h"
#include "OrderBook.h"
#include "serialiser/rsbaseserial.h"

#include <iostream>
#include <stdexcept>


const uint16_t RS_SERVICE_TYPE_ZERORESERVE_PLUGIN = 0xBEEF;
const uint32_t CONFIG_TYPE_ZERORESERVE_PLUGIN     = 0xDEADBEEF;

#define CURRENCY_STRLEN 3
#define HOLLERITH_LEN_SPEC 4

RsItem* RsZeroReserveSerialiser::deserialise(void *data, uint32_t *pktsize)
{
    std::cerr << "Zero Reserve: deserialise()" << std::endl;

    /* get the type and size */
    uint32_t rstype = getRsItemId(data);

    if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)))
        return NULL ;

    try{
        switch(getRsItemSubType(rstype))
        {
        case RsZeroReserveItem::ZERORESERVE_ORDERBOOK_ITEM:
            return new RsZeroReserveOrderBookItem(data, *pktsize);
            break;
        case RsZeroReserveItem::ZERORESERVE_TX_INIT_ITEM:
            return new RsZeroReserveInitTxItem(data, *pktsize);
            break;
        case RsZeroReserveItem::ZERORESERVE_TX_ITEM:
            return new RsZeroReserveTxItem(data, *pktsize);
            break;
        default:
            return NULL;
        }
    }
    catch(std::exception& e){
        std::cerr << "RsZeroReserveSerialiser: deserialization error: " << e.what() << std::endl;
        return NULL;
    }
}



std::ostream& RsZeroReserveOrderBookItem::print(std::ostream &out, uint16_t indent)
{
        printRsItemBase(out, "RsZeroReserveOrderBookItem", indent);
        uint16_t int_Indent = indent + 2;
        printIndent(out, int_Indent);
        out << "Amount: " << m_order->m_amount.toStdString() << std::endl;

        printIndent(out, int_Indent);
        out << "Price : " << m_order->m_price_d << std::endl;

        printIndent(out, int_Indent);
        out << "Type  : " << m_order->m_orderType << std::endl;

        printRsItemEnd(out, "RsZeroReserveOrderBookItem", indent);
        return out;
}

uint32_t RsZeroReserveOrderBookItem::serial_size() const
{
        uint32_t s = 8; /* header */
        s += m_order->m_amount.length() + HOLLERITH_LEN_SPEC;
        s += m_order->m_price.length() + HOLLERITH_LEN_SPEC;
        s += sizeof(uint8_t); // the type (BID / ASK)
        s += CURRENCY_STRLEN + HOLLERITH_LEN_SPEC;
        s += sizeof(uint64_t);
        s+= m_order->m_trader_id.length() + HOLLERITH_LEN_SPEC;

        return s;
}

bool RsZeroReserveOrderBookItem::serialise(void *data, uint32_t& pktsize)
{
        uint32_t tlvsize = serial_size() ;

        if (pktsize < tlvsize)
                return false; /* not enough space */

        pktsize = tlvsize;

        bool ok = true;

        ok &= setRsItemHeader(data, tlvsize, PacketId(), tlvsize);

        uint32_t offset = 8;  // skip header

        std::string buf = m_order->m_amount.toStdString();
        ok &= setRawString( data, tlvsize, &offset, buf );

        buf = Currency::currencySymbols[m_order->m_currency];
        ok &= setRawString( data, tlvsize, &offset, buf );

        ok &= setRawUInt8( data, tlvsize, &offset, m_order->m_orderType );

        buf = m_order->m_price.toStdString();
        ok &= setRawString( data, tlvsize, &offset, buf );

        ok &= setRawUInt64( data, tlvsize, &offset, m_order->m_timeStamp );
        ok &= setRawString( data, tlvsize, &offset, m_order->m_trader_id );

        if (offset != tlvsize){
                ok = false;
                std::cerr << "RsZeroReserveOrderBookItem::serialise() Size Error! " << std::endl;
        }

        return ok;
}

RsZeroReserveOrderBookItem::RsZeroReserveOrderBookItem(void *data, uint32_t pktsize)
        : RsZeroReserveItem( ZERORESERVE_ORDERBOOK_ITEM )
{   
    /* get the type and size */
    uint32_t rstype = getRsItemId(data);
    uint32_t rssize = getRsItemSize(data);

    uint32_t offset = 8;


    if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)) || (ZERORESERVE_ORDERBOOK_ITEM != getRsItemSubType(rstype)))
        throw std::runtime_error("Wrong packet type!") ;

    if (pktsize < rssize)    /* check size */
        throw std::runtime_error("Not enough size!") ;

    bool ok = true;

    m_order = new OrderBook::Order;

    std::string amount;
    ok &= getRawString(data, rssize, &offset, amount);
    m_order->m_amount = QString::fromStdString( amount );

    std::string currency;
    ok &= getRawString(data, rssize, &offset, currency);
    m_order->m_currency = Currency::getCurrencyBySymbol( currency );  // TODO: Check error "no such symbol"

    uint8_t order_type;
    ok &= getRawUInt8(data, rssize, &offset, &order_type );
    m_order->m_orderType = (OrderBook::Order::OrderType) order_type;

    std::string price;
    ok &= getRawString(data, rssize, &offset, price);
    m_order->setPrice( QString::fromStdString( price ) );

    uint64_t timestamp;
    ok &= getRawUInt64(data, rssize, &offset, &timestamp );
    m_order->m_timeStamp = timestamp;

    std::string trader_id;
    ok &= getRawString(data, rssize, &offset, trader_id);
    m_order->m_trader_id = trader_id;

    if (offset != rssize || !ok )
        throw std::runtime_error("Deserialisation error!") ;
}

RsZeroReserveOrderBookItem::RsZeroReserveOrderBookItem( OrderBook::Order * order)
        : RsZeroReserveItem( ZERORESERVE_ORDERBOOK_ITEM ),
        m_order( order )
{

}


//// Begin TX Items  /////



RsZeroReserveTxItem::RsZeroReserveTxItem( TransactionManager::TxPhase phase, RS_PKT_SUBTYPE subtype ) :
    RsZeroReserveItem( (uint8_t)subtype ),
    m_TxPhase(phase)
{}


RsZeroReserveTxItem::RsZeroReserveTxItem(void *data, uint32_t pktsize, RS_PKT_SUBTYPE zeroreserve_subtype )
        : RsZeroReserveItem( zeroreserve_subtype )
{
    uint32_t rssize = getRsItemSize(data);
    if( zeroreserve_subtype == ZERORESERVE_TX_ITEM ){
        /* get the type and size */
        uint32_t rstype = getRsItemId(data);

        if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)) || (ZERORESERVE_TX_ITEM != getRsItemSubType(rstype)))
            throw std::runtime_error("Wrong packet type!") ;

        if (pktsize < rssize)    /* check size */
            throw std::runtime_error("Not enough size!") ;
    }

    m_offset = 8;
    bool ok = true;

    uint8_t txPhase;
    ok &= getRawUInt8(data, rssize, &m_offset, &txPhase );
    m_TxPhase = (TransactionManager::TxPhase) txPhase;

    if ( !ok )
        throw std::runtime_error("Deserialisation error!") ;
}

uint32_t RsZeroReserveTxItem::serial_size() const
{
    return 8                    //  header
            + sizeof(uint8_t);  // TX Phase
}



bool RsZeroReserveTxItem::serialise(void *data, uint32_t& pktsize)
{
    uint32_t tlvsize = serial_size() ;

    if (pktsize < tlvsize)
        return false; /* not enough space */

    pktsize = tlvsize;

    bool ok = true;

    ok &= setRsItemHeader(data, tlvsize, PacketId(), tlvsize);

    m_offset = 8;  // skip header

    ok &= setRawUInt8( data, tlvsize, &m_offset, m_TxPhase );

    return ok;
}

std::ostream& RsZeroReserveTxItem::print(std::ostream &out, uint16_t indent)
{
    printRsItemBase(out, "RsZeroReserveTxItem", indent);
    uint16_t int_Indent = indent + 2;
    printIndent(out, int_Indent);
    out << "TX Phase : " << m_TxPhase << std::endl;

    printRsItemEnd(out, "RsZeroReserveTxItem", indent);
    return out;
}



//// Begin TX INIT Item  /////


RsZeroReserveInitTxItem::RsZeroReserveInitTxItem(void *data, uint32_t pktsize )
        : RsZeroReserveTxItem( data, pktsize, ZERORESERVE_TX_INIT_ITEM )
{
    uint32_t rstype = getRsItemId(data);
    uint32_t rssize = getRsItemSize(data);

    if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)) || (ZERORESERVE_TX_INIT_ITEM != getRsItemSubType(rstype)))
        throw std::runtime_error("Wrong packet type!") ;

    if (pktsize < rssize)    /* check size */
        throw std::runtime_error("Not enough size!") ;

    bool ok = true;

    uint8_t role;
    ok &= getRawUInt8(data, rssize, &m_offset, &role );
    m_Role = (TransactionManager::Role) role;
    ok &= getRawString(data, rssize, &m_offset, m_amount );
    ok &= getRawString(data, rssize, &m_offset, m_currency );

    if ( !ok )
        throw std::runtime_error("Deserialisation error!") ;
}


uint32_t RsZeroReserveInitTxItem::serial_size() const
{
    return RsZeroReserveTxItem::serial_size()
            + sizeof(uint8_t)                          // Role
            + CURRENCY_STRLEN + HOLLERITH_LEN_SPEC     // currency
            + m_amount.length() + HOLLERITH_LEN_SPEC;  // amount
}



bool RsZeroReserveInitTxItem::serialise(void *data, uint32_t& pktsize)
{
    bool ok = true;
    RsZeroReserveTxItem::serialise( data, pktsize);

    uint32_t tlvsize = serial_size() ;

    ok &= setRawUInt8( data, tlvsize, &m_offset, m_Role );
    ok &= setRawString( data, tlvsize, &m_offset, m_amount );
    ok &= setRawString( data, tlvsize, &m_offset, m_currency );

    if (m_offset != tlvsize){
        ok = false;
        std::cerr << "RsZeroReserveInitTxItem::serialise() Size Error! " << std::endl;
    }
    return ok;
}

std::ostream& RsZeroReserveInitTxItem::print(std::ostream &out, uint16_t indent)
{
    printRsItemBase(out, "RsZeroReserveInitTxItem", indent);
    uint16_t int_Indent = indent + 2;
    printIndent(out, int_Indent);
    out << "Role     : " << m_Role << std::endl;

    printIndent(out, int_Indent);
    out << "Amount   : " << m_amount << std::endl;

    printIndent(out, int_Indent);
    out << "Currency : " << m_currency << std::endl;

    printRsItemEnd(out, "RsZeroReserveInitTxItem", indent);
    return out;
}

RsZeroReserveInitTxItem::RsZeroReserveInitTxItem( TransactionManager::TxPhase phase, const std::string & amount, const std::string & currency ) :
    RsZeroReserveTxItem( phase, ZERORESERVE_TX_INIT_ITEM ),
    m_amount( amount ),
    m_currency( currency )
{}
