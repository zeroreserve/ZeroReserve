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
#include "Payment.h"

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
        case RsZeroReserveItem::ZERORESERVE_TX_INIT_ITEM:
            return new RsZeroReserveInitTxItem(data, *pktsize);
        case RsZeroReserveItem::ZERORESERVE_TX_ITEM:
            return new RsZeroReserveTxItem(data, *pktsize);
        case RsZeroReserveItem::ZERORESERVE_CREDIT_ITEM:
            return new RsZeroReserveCreditItem(data, *pktsize);
        default:
            return NULL;
        }
    }
    catch(std::exception& e){
        std::cerr << "RsZeroReserveSerialiser: deserialization error: " << e.what() << std::endl;
        return NULL;
    }
}


//// Begin OrderBook Item  /////


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
        s += m_order->m_trader_id.length() + HOLLERITH_LEN_SPEC;
        s += sizeof( uint8_t );  // purpose

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
        ok &= setRawUInt8( data, tlvsize, &offset, m_order->m_purpose );

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

    uint8_t order_purpose;
    ok &= getRawUInt8(data, rssize, &offset, &order_purpose );
    m_order->m_purpose = (OrderBook::Order::Purpose) order_purpose;

    if (offset != rssize || !ok )
        throw std::runtime_error("Deserialisation error!") ;
}

RsZeroReserveOrderBookItem::RsZeroReserveOrderBookItem( OrderBook::Order * order)
        : RsZeroReserveItem( ZERORESERVE_ORDERBOOK_ITEM ),
        m_order( order )
{

}


//// Begin Credit Item  /////



std::ostream& RsZeroReserveCreditItem::print(std::ostream &out, uint16_t indent)
{
        printRsItemBase(out, "RsZeroReserveCreditItem", indent);
        uint16_t int_Indent = indent + 2;
        printIndent(out, int_Indent);
        out << "Currency: " << m_credit->m_currency << std::endl;

        printIndent(out, int_Indent);
        out << "Credit: " << m_credit->m_credit << std::endl;

        printIndent(out, int_Indent);
        out << "Our Credit : " << m_credit->m_our_credit << std::endl;

        printIndent(out, int_Indent);
        out << "Balance : " << m_credit->m_balance << std::endl;

        printRsItemEnd(out, "RsZeroReserveCreditItem", indent);
        return out;
}

uint32_t RsZeroReserveCreditItem::serial_size() const
{
        uint32_t s = 8; /* header */
        s += CURRENCY_STRLEN + HOLLERITH_LEN_SPEC;
        s += m_credit->m_credit.length() + HOLLERITH_LEN_SPEC;
        s += m_credit->m_our_credit.length() + HOLLERITH_LEN_SPEC;
        s += m_credit->m_balance.length() + HOLLERITH_LEN_SPEC;

        return s;
}

bool RsZeroReserveCreditItem::serialise(void *data, uint32_t& pktsize)
{
        uint32_t tlvsize = serial_size() ;

        if (pktsize < tlvsize)
                return false; /* not enough space */

        pktsize = tlvsize;

        bool ok = true;

        ok &= setRsItemHeader(data, tlvsize, PacketId(), tlvsize);

        uint32_t offset = 8;  // skip header

        ok &= setRawString( data, tlvsize, &offset, m_credit->m_currency );
        ok &= setRawString( data, tlvsize, &offset, m_credit->m_credit );
        ok &= setRawString( data, tlvsize, &offset, m_credit->m_our_credit );
        ok &= setRawString( data, tlvsize, &offset, m_credit->m_balance );

        if (offset != tlvsize){
                ok = false;
                std::cerr << "RsZeroReserveCreditItem::serialise() Size Error! " << std::endl;
        }

        return ok;
}

RsZeroReserveCreditItem::RsZeroReserveCreditItem(void *data, uint32_t pktsize)
        : RsZeroReserveItem( ZERORESERVE_CREDIT_ITEM )
{
    /* get the type and size */
    uint32_t rstype = getRsItemId(data);
    uint32_t rssize = getRsItemSize(data);

    bool ok = true;
    uint32_t offset = 8;

    std::string currency;

    ok &= getRawString( data, rssize, &offset, currency );
    m_credit = new Credit( "", currency );  // FIXME: We can't give an ID here - add it later. Bad

    if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)) || (ZERORESERVE_CREDIT_ITEM != getRsItemSubType(rstype)))
        throw std::runtime_error("Wrong packet type!") ;

    if (pktsize < rssize)    /* check size */
        throw std::runtime_error("Not enough size!") ;

    ok &= getRawString( data, rssize, &offset, m_credit->m_our_credit ); // these 2 need to interchange
    ok &= getRawString( data, rssize, &offset, m_credit->m_credit );     // because credit at peer is our_credit here
    ok &= getRawString( data, rssize, &offset, m_credit->m_balance );

    if (offset != rssize || !ok )
        throw std::runtime_error("Deserialisation error!") ;
}

RsZeroReserveCreditItem::RsZeroReserveCreditItem( Credit * credit )
        : RsZeroReserveItem( ZERORESERVE_CREDIT_ITEM )
{
    m_credit = new Credit( *credit );
    PeerId( credit->m_id );
}

RsZeroReserveCreditItem::~RsZeroReserveCreditItem()
{
    delete m_credit;
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
    std::string amount;
    ok &= getRawString(data, rssize, &m_offset, amount );
    std::string currency;
    ok &= getRawString(data, rssize, &m_offset, currency );
    uint8_t category;
    ok &= getRawUInt8(data, rssize, &m_offset, &category );
    std::string freeformText;
    ok &= getRawString(data, rssize, &m_offset, freeformText );

    if ( !ok )
        throw std::runtime_error("Deserialisation error!") ;

    m_payment = new PaymentReceiver( "", amount, currency, (Payment::Category)category );
    m_payment->setText( freeformText );
}


Payment * RsZeroReserveInitTxItem::getPayment()
{
    m_payment->setCounterparty( PeerId() );
    return m_payment;
}


uint32_t RsZeroReserveInitTxItem::serial_size() const
{
    return RsZeroReserveTxItem::serial_size()
            + sizeof(uint8_t)                          // Role
            + CURRENCY_STRLEN + HOLLERITH_LEN_SPEC     // currency
            + m_payment->getAmount().length() + HOLLERITH_LEN_SPEC  // amount
            + sizeof(uint8_t)                          // Category
            + m_payment->getText().length() + HOLLERITH_LEN_SPEC; // freeform data
}



bool RsZeroReserveInitTxItem::serialise(void *data, uint32_t& pktsize)
{
    bool ok = true;
    RsZeroReserveTxItem::serialise( data, pktsize);

    uint32_t tlvsize = serial_size() ;

    ok &= setRawUInt8( data, tlvsize, &m_offset, m_Role );

    std::string amount = m_payment->getAmount();
    ok &= setRawString( data, tlvsize, &m_offset, amount );

    std::string currency = m_payment->getCurrency();
    ok &= setRawString( data, tlvsize, &m_offset, currency );

    ok &= setRawUInt8( data, tlvsize, &m_offset, m_payment->getCategory() );

    std::string freeformText = m_payment->getText();
    ok &= setRawString( data, tlvsize, &m_offset, freeformText );

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
    out << "Amount   : " << m_payment->getAmount() << std::endl;

    printIndent(out, int_Indent);
    out << "Currency : " << m_payment->getCurrency() << std::endl;

    printRsItemEnd(out, "RsZeroReserveInitTxItem", indent);
    return out;
}

RsZeroReserveInitTxItem::RsZeroReserveInitTxItem(Payment *payment) :
    RsZeroReserveTxItem( TransactionManager::QUERY, ZERORESERVE_TX_INIT_ITEM ),
    m_payment( payment )
{
    this->PeerId( m_payment->getCounterparty() );
}


RsZeroReserveInitTxItem::~RsZeroReserveInitTxItem()
{
}

