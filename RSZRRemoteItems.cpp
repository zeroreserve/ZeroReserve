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


#include "RSZRRemoteItems.h"
#include "Payment.h"


#include "serialiser/rsbaseserial.h"


bool RSZRRemoteItem::serialise(void *data, uint32_t& pktsize)
{
    uint32_t tlvsize = serial_size() ;
    bool ok = RsZeroReserveItem::serialise( data, pktsize );
    ok &= setRawString( data, tlvsize, &m_Offset, m_Address );
    return ok;
}

uint32_t RSZRRemoteItem::serial_size() const
{
    return RsZeroReserveItem::serial_size() + m_Address.length() + HOLLERITH_LEN_SPEC;
}


RSZRRemoteItem::RSZRRemoteItem( void *data,uint32_t size, uint8_t zeroreserve_subtype ) :
    RsZeroReserveItem( data, size, zeroreserve_subtype)
{
    uint32_t rssize = getRsItemSize(data);
    getRawString(data, rssize, &m_Offset, m_Address );
}


//// Begin RSZRPayRequestItem Item  /////


std::ostream& RSZRPayRequestItem::print(std::ostream &out, uint16_t indent)
{
        printRsItemBase(out, "RsZeroReserveMsgItem", indent);
        uint16_t int_Indent = indent + 2;
        printIndent(out, int_Indent);
        out << "Amount : " << m_Amount << std::endl;

        printIndent(out, int_Indent);
        out << "Currency : " << m_Currency << std::endl;

        printRsItemEnd(out, "RsZeroReserveMsgItem", indent);
        return out;
}

uint32_t RSZRPayRequestItem::serial_size() const
{
        uint32_t s = RSZRRemoteItem::serial_size();
        s += m_Amount.toStdString().length() + HOLLERITH_LEN_SPEC;
        s += CURRENCY_STRLEN + HOLLERITH_LEN_SPEC;

        return s;
}

bool RSZRPayRequestItem::serialise(void *data, uint32_t& pktsize)
{
        uint32_t tlvsize = serial_size() ;

        if (pktsize < tlvsize)
                return false; /* not enough space */

        pktsize = tlvsize;

        bool ok = RSZRRemoteItem::serialise( data,  pktsize);

        std::string amount = m_Amount.toStdString();
        ok &= setRawString( data, tlvsize, &m_Offset, amount );
        ok &= setRawString( data, tlvsize, &m_Offset, m_Currency );

        if (m_Offset != tlvsize){
                ok = false;
                std::cerr << "RsZeroReserveMsgItem::serialise() Size Error! " << std::endl;
        }

        return ok;
}

RSZRPayRequestItem::RSZRPayRequestItem(void *data, uint32_t pktsize)
        : RSZRRemoteItem( data, pktsize, ZR_REMOTE_PAYREQUEST_ITEM )
{
    /* get the type and size */
    uint32_t rstype = getRsItemId(data);
    uint32_t rssize = getRsItemSize(data);

    if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)) || (ZR_REMOTE_PAYREQUEST_ITEM != getRsItemSubType(rstype)))
        throw std::runtime_error("Wrong packet type!") ;

    if (pktsize < rssize)    /* check size */
        throw std::runtime_error("Not enough size!") ;

    bool ok = true;

    std::string amount;
    ok &= getRawString(data, rssize, &m_Offset, amount );
    m_Amount = ZR::ZR_Number::fromFractionString( amount );
    ok &= getRawString(data, rssize, &m_Offset, m_Currency );


    if ( m_Offset != rssize || !ok )
        throw std::runtime_error("Deserialisation error!") ;
}

RSZRPayRequestItem::RSZRPayRequestItem( const ZR::VirtualAddress & addr, const ZR::ZR_Number & amount, const std::string & currency )
        : RSZRRemoteItem( addr, ZR_REMOTE_PAYREQUEST_ITEM ),
        m_Amount( amount ),
        m_Currency( currency )
{}


//// Begin RSZRRemoteTxItem Item  /////


std::ostream& RSZRRemoteTxItem::print(std::ostream &out, uint16_t indent)
{
        printRsItemBase(out, "RSZRRemoteTxItem", indent);
        uint16_t int_Indent = indent + 2;
        printIndent(out, int_Indent);
        out << "Done a lot of travelling " << std::endl;

        printRsItemEnd(out, "RSZRRemoteTxItem", indent);
        return out;
}

uint32_t RSZRRemoteTxItem::serial_size() const
{
        return  RSZRRemoteItem::serial_size()
                + sizeof(uint8_t)   // tx phase
                + sizeof(uint8_t);  // direction
}

bool RSZRRemoteTxItem::serialise(void *data, uint32_t& pktsize)
{
        uint32_t tlvsize = serial_size() ;

        if (pktsize < tlvsize)
                return false; /* not enough space */

        pktsize = tlvsize;

        bool ok = RSZRRemoteItem::serialise( data,  pktsize);
        ok &= setRawUInt8( data, tlvsize, &m_Offset, m_TxPhase );
        ok &= setRawUInt8( data, tlvsize, &m_Offset, m_Direction );

        if (m_Offset != tlvsize){
                ok = false;
                std::cerr << "RSZRRemoteTxItem::serialise() Size Error! " << std::endl;
        }

        return ok;
}

RSZRRemoteTxItem::RSZRRemoteTxItem(void *data, uint32_t pktsize, uint8_t itemType )
        : RSZRRemoteItem( data, pktsize, itemType )
{
    /* get the type and size */
    uint32_t rstype = getRsItemId(data);
    uint32_t rssize = getRsItemSize(data);

    if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)))
        throw std::runtime_error("Wrong packet type!") ;

    if (pktsize < rssize)    /* check size */
        throw std::runtime_error("Not enough size!") ;

    uint8_t txPhase;
    bool ok = getRawUInt8(data, rssize, &m_Offset, &txPhase );
    m_TxPhase = (TransactionManager::TxPhase) txPhase;
    uint8_t direction;
    ok &= getRawUInt8(data, rssize, &m_Offset, &direction );
    m_Direction = ( Router::TunnelDirection ) direction;

    if ( m_Offset != rssize )
        throw std::runtime_error("Deserialisation error!") ;
}

RSZRRemoteTxItem::RSZRRemoteTxItem( const ZR::VirtualAddress & addr, TransactionManager::TxPhase txPhase , Router::TunnelDirection direction, uint8_t itemType )
        : RSZRRemoteItem( addr, itemType ),
          m_TxPhase( txPhase ),
          m_Direction( direction )
{}


//// Begin RSZRRemoteTxInitItem Item  /////


std::ostream& RSZRRemoteTxInitItem::print(std::ostream &out, uint16_t indent)
{
        printRsItemBase(out, "RSZRRemoteTxInitItem", indent);
        uint16_t int_Indent = indent + 2;
        printIndent(out, int_Indent);
        out << "Done a lot of travelling " << std::endl;

        printRsItemEnd(out, "RSZRRemoteTxInitItem", indent);
        return out;
}

uint32_t RSZRRemoteTxInitItem::serial_size() const
{
        return  RSZRRemoteTxItem::serial_size();
}

bool RSZRRemoteTxInitItem::serialise(void *data, uint32_t& pktsize)
{
        uint32_t tlvsize = serial_size() ;

        if (pktsize < tlvsize)
                return false; /* not enough space */

        pktsize = tlvsize;

        bool ok = RSZRRemoteTxItem::serialise( data,  pktsize);


        if (m_Offset != tlvsize){
                ok = false;
                std::cerr << "RSZRRemoteTxInitItem::serialise() Size Error! " << std::endl;
        }

        return ok;
}

RSZRRemoteTxInitItem::RSZRRemoteTxInitItem(void *data, uint32_t pktsize )
        : RSZRRemoteTxItem( data, pktsize, ZR_REMOTE_TX_INIT_ITEM )
{
    /* get the type and size */
    uint32_t rstype = getRsItemId(data);
    uint32_t rssize = getRsItemSize(data);

    if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) || (RS_SERVICE_TYPE_ZERORESERVE_PLUGIN != getRsItemService(rstype)) || (ZR_REMOTE_TX_INIT_ITEM != getRsItemSubType(rstype)))
        throw std::runtime_error("Wrong packet type!") ;

    if (pktsize < rssize)    /* check size */
        throw std::runtime_error("Not enough size!") ;



    if ( m_Offset != rssize )
        throw std::runtime_error("Deserialisation error!") ;
}

RSZRRemoteTxInitItem::RSZRRemoteTxInitItem( const ZR::VirtualAddress & addr, TransactionManager::TxPhase txPhase , Router::TunnelDirection direction )
        : RSZRRemoteTxItem( addr, txPhase, direction, ZR_REMOTE_TX_INIT_ITEM )
{}
