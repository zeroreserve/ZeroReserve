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
#include "Payment.h"
#include "RSZRRemoteItems.h"

#include "serialiser/rsbaseserial.h"

#include <iostream>
#include <stdexcept>


const uint16_t RS_SERVICE_TYPE_ZERORESERVE_PLUGIN = 0xBEEF;
const uint32_t CONFIG_TYPE_ZERORESERVE_PLUGIN     = 0xDEADBEEF;
const uint8_t RsZeroReserveItem::PROTOCOL_VERSION = 0;
const uint8_t RsZeroReserveItem::headersOffset = 8;
const int RsZeroReserveItem::CURRENCY_STRLEN = 3;
const int RsZeroReserveItem::HOLLERITH_LEN_SPEC = 4;

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
        case RsZeroReserveItem::ZERORESERVE_MSG_ITEM:
            return new RsZeroReserveMsgItem(data, *pktsize);
        case RsZeroReserveItem::ZR_REMOTE_PAYREQUEST_ITEM:
            return new RSZRPayRequestItem( data, *pktsize );
        case RsZeroReserveItem::ZR_REMOTE_TX_ITEM:
            return new RSZRRemoteTxItem( data, *pktsize );
        case RsZeroReserveItem::ZR_REMOTE_TX_INIT_ITEM:
            return new RSZRRemoteTxInitItem( data, *pktsize );
        case RsZeroReserveItem::ZR_REMOTE_BUYREQUEST_ITEM:
            return new RSZRBuyRequestItem( data, *pktsize );
        default:
            return NULL;
        }
    }
    catch(std::exception& e){
        std::cerr << "RsZeroReserveSerialiser: deserialization error: " << e.what() << std::endl;
        return NULL;
    }
}

// TODO: make all serialise() below use this

bool RsZeroReserveItem::serialise(void *data,uint32_t & /* size */)
{
    bool ok = true;
    m_Offset = headersOffset;
    uint32_t tlvsize = serial_size();
    ok &= setRsItemHeader(data, tlvsize, PacketId(), tlvsize);
    ok &= setRawUInt8( data, tlvsize, &m_Offset, PROTOCOL_VERSION );
    return ok;
}

RsZeroReserveItem::RsZeroReserveItem( void *data,uint32_t& size, uint8_t zeroreserve_subtype ) :
    RsItem(RS_PKT_VERSION_SERVICE,RS_SERVICE_TYPE_ZERORESERVE_PLUGIN, zeroreserve_subtype)
{
    setPriorityLevel(QOS_PRIORITY_RS_ZERORESERVE);
    uint32_t rssize = getRsItemSize(data);
    m_Offset = headersOffset;
    uint8_t protocolVersion;
    getRawUInt8(data, rssize, &m_Offset, &protocolVersion );
    if( protocolVersion != PROTOCOL_VERSION ){
        std::cerr << "Unknown protocol version: " << protocolVersion << std::endl;
        throw std::runtime_error( "Unknown protocol version" );
    }
}



//// Begin Msg Item  /////


std::ostream& RsZeroReserveMsgItem::print(std::ostream &out, uint16_t indent)
{
        printRsItemBase(out, "RsZeroReserveMsgItem", indent);
        uint16_t int_Indent = indent + 2;
        printIndent(out, int_Indent);
        out << "MsgType : " << m_msgType << std::endl;

        printIndent(out, int_Indent);
        out << "Message : " << m_msg << std::endl;

        printRsItemEnd(out, "RsZeroReserveMsgItem", indent);
        return out;
}

uint32_t RsZeroReserveMsgItem::serial_size() const
{
        uint32_t s = headersOffset; /* header */
        s += sizeof(uint8_t); // the type
        s += m_msg.length() + HOLLERITH_LEN_SPEC;

        return s;
}

bool RsZeroReserveMsgItem::serialise(void *data, uint32_t& pktsize)
{
        uint32_t tlvsize = serial_size() ;

        if (pktsize < tlvsize)
                return false; /* not enough space */

        pktsize = tlvsize;

        bool ok = true;

        ok &= setRsItemHeader(data, tlvsize, PacketId(), tlvsize);

        uint32_t offset = headersOffset;  // skip header

        ok &= setRawUInt8( data, tlvsize, &offset, m_msgType );
        ok &= setRawString( data, tlvsize, &offset, m_msg );

        if (offset != tlvsize){
                ok = false;
                std::cerr << "RsZeroReserveMsgItem::serialise() Size Error! " << std::endl;
        }

        return ok;
}

RsZeroReserveMsgItem::RsZeroReserveMsgItem(void *data, uint32_t pktsize)
        : RsZeroReserveItem( ZERORESERVE_MSG_ITEM )
{
    /* get the type and size */
    uint32_t rstype = getRsItemId(data);
    uint32_t rssize = getRsItemSize(data);

    uint32_t offset = headersOffset;


    if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)) || (ZERORESERVE_MSG_ITEM != getRsItemSubType(rstype)))
        throw std::runtime_error("Wrong packet type!") ;

    if (pktsize < rssize)    /* check size */
        throw std::runtime_error("Not enough size!") ;

    bool ok = true;

    uint8_t msgType;
    ok &= getRawUInt8(data, rssize, &offset, &msgType );
    m_msgType = (MsgType) msgType;
    ok &= getRawString(data, rssize, &offset, m_msg );

    if (offset != rssize || !ok )
        throw std::runtime_error("Deserialisation error!") ;
}

RsZeroReserveMsgItem::RsZeroReserveMsgItem( MsgType msgType, const std::string & msg )
        : RsZeroReserveItem( ZERORESERVE_MSG_ITEM ),
        m_msgType( msgType ),
        m_msg( msg )
{}


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
        uint32_t s = headersOffset; /* header */
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

        uint32_t offset = headersOffset;  // skip header

        ok &= setRawString( data, tlvsize, &offset, m_credit->m_currency );
        ok &= setRawString( data, tlvsize, &offset, m_credit->m_credit.toStdString() );
        ok &= setRawString( data, tlvsize, &offset, m_credit->m_our_credit.toStdString() );
        ok &= setRawString( data, tlvsize, &offset, m_credit->m_balance.toStdString() );

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
    uint32_t offset = headersOffset;

    std::string currency;

    ok &= getRawString( data, rssize, &offset, currency );
    m_credit = new Credit( "", currency );  // FIXME: We can't give an ID here - add it later. Bad

    if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)) || (ZERORESERVE_CREDIT_ITEM != getRsItemSubType(rstype)))
        throw std::runtime_error("Wrong packet type!") ;

    if (pktsize < rssize)    /* check size */
        throw std::runtime_error("Not enough size!") ;

    std::string buf;
    ok &= getRawString( data, rssize, &offset, buf ); // these 2 need to interchange
    m_credit->m_our_credit = ZR::ZR_Number::fromFractionString( buf );
    ok &= getRawString( data, rssize, &offset, buf );     // because credit at peer is our_credit here
    m_credit->m_credit = ZR::ZR_Number::fromFractionString( buf );
    ok &= getRawString( data, rssize, &offset, buf );
    m_credit->m_balance = ZR::ZR_Number::fromFractionString( buf );

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
        : RsZeroReserveItem( data, pktsize, zeroreserve_subtype )
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

    bool ok = true;

    uint8_t txPhase;
    ok &= getRawUInt8(data, rssize, &m_Offset, &txPhase );
    m_TxPhase = (TransactionManager::TxPhase) txPhase;

    ok &= getRawString(data, rssize, &m_Offset, m_txId );

    if ( !ok )
        throw std::runtime_error("Deserialisation error!") ;
}

uint32_t RsZeroReserveTxItem::serial_size() const
{
    return RsZeroReserveItem::serial_size()
            + sizeof(uint8_t)   // TX Phase
            + m_txId.length() + HOLLERITH_LEN_SPEC;
}



bool RsZeroReserveTxItem::serialise(void *data, uint32_t& pktsize)
{
    uint32_t tlvsize = serial_size() ;

    if (pktsize < tlvsize)
        return false; /* not enough space */

    pktsize = tlvsize;

    bool ok = RsZeroReserveItem::serialise( data, pktsize );

    ok &= setRawUInt8( data, tlvsize, &m_Offset, m_TxPhase );
    ok &= setRawString(data, tlvsize, &m_Offset, m_txId );

    return ok;
}

std::ostream& RsZeroReserveTxItem::print(std::ostream &out, uint16_t indent)
{
    printRsItemBase(out, "RsZeroReserveTxItem", indent);
    uint16_t int_Indent = indent + 2;
    printIndent(out, int_Indent);
    out << "TX Phase : " << m_TxPhase << std::endl;
    out << "TX ID    : " << m_txId    << std::endl;

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
    ok &= getRawUInt8(data, rssize, &m_Offset, &role );
    m_Role = (TransactionManager::Role) role;
    std::string s_amount;
    ok &= getRawString(data, rssize, &m_Offset, s_amount );
    ZR::ZR_Number amount = ZR::ZR_Number::fromFractionString( s_amount );
    std::string currency;
    ok &= getRawString(data, rssize, &m_Offset, currency );
    uint8_t category;
    ok &= getRawUInt8(data, rssize, &m_Offset, &category );
    std::string referrer;
    ok &= getRawString(data, rssize, &m_Offset, referrer );

    if ( !ok )
        throw std::runtime_error("Deserialisation error!") ;

    m_payment = new PaymentReceiver( "", amount, currency, (Payment::Category)category );
    m_payment->referrerId( referrer );
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
            + m_payment->referrerId().length() + HOLLERITH_LEN_SPEC; // freeform data
}



bool RsZeroReserveInitTxItem::serialise(void *data, uint32_t& pktsize)
{
    bool ok = true;
    RsZeroReserveTxItem::serialise( data, pktsize);

    uint32_t tlvsize = serial_size() ;

    ok &= setRawUInt8( data, tlvsize, &m_Offset, m_Role );
    ok &= setRawString( data, tlvsize, &m_Offset, m_payment->getAmount().toStdString() );
    ok &= setRawString( data, tlvsize, &m_Offset, m_payment->getCurrency() );
    ok &= setRawUInt8( data, tlvsize, &m_Offset, m_payment->getCategory() );
    ok &= setRawString( data, tlvsize, &m_Offset, m_payment->referrerId() );

    if (m_Offset != tlvsize){
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

