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


RsItem* RsZeroReserveSerialiser::deserialise(void *data, uint32_t *pktsize)
{
#ifdef ZERORESERVE_DEBUG
    std::cerr << "RsVoipSerialiser::deserialise()" << std::endl;
#endif

    /* get the type and size */
    uint32_t rstype = getRsItemId(data);

    if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)))
        return NULL ;

    try{
        return new RsZeroReserveOrderBookItem(data, *pktsize);
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
#define CURRENCY_STRLEN 3
#define HOLLERITH_LEN_SPEC 4
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
        : RsZeroReserveItem(RS_PKT_SUBTYPE_ZERORESERVE_ORDERBOOKITEM)
{   
    /* get the type and size */
    uint32_t rstype = getRsItemId(data);
    uint32_t rssize = getRsItemSize(data);

    uint32_t offset = 8;


    if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)) || (RS_PKT_SUBTYPE_ZERORESERVE_ORDERBOOKITEM != getRsItemSubType(rstype)))
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

    if (offset != rssize)
        throw std::runtime_error("Deserialisation error!") ;

    if (!ok)
        throw std::runtime_error("Deserialisation error!") ;
}

RsZeroReserveOrderBookItem::RsZeroReserveOrderBookItem( OrderBook::Order * order)
        : RsZeroReserveItem(RS_PKT_SUBTYPE_ZERORESERVE_ORDERBOOKITEM),
        m_order( order )
{

}
