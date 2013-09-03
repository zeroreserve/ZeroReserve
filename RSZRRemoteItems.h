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
    typedef std::string VirtualAddress;

    RSZRRemoteItem( const VirtualAddress & addr, uint8_t zeroreserve_subtype ) :
        RsZeroReserveItem( zeroreserve_subtype ),
        m_Address( addr )
    {}

    RSZRRemoteItem(void *data,uint32_t size, uint8_t zeroreserve_subtype);

    virtual std::ostream& print(std::ostream &out, uint16_t indent = 0) = 0 ;

    virtual bool serialise(void *data,uint32_t& size);
    virtual uint32_t serial_size() const = 0 ;

private:
    // depending on the item type, this can be a target address or a propagation address
    // to be added to the router
    VirtualAddress m_Address;
};


/**
 * @brief propagate a payment request through the network.
 *
 * This will be picked up by the payer. This guarantees that a possible payment route exists,
 * possibly more than one.
 */
class RSZRPayRequestItem : public RSZRRemoteItem
{
    RSZRPayRequestItem();
public:

    RSZRPayRequestItem(void *data,uint32_t size);
    RSZRPayRequestItem(const VirtualAddress & addr, const ZR::ZR_Number &amount, const std::string & currencySymbol);

    virtual bool serialise(void *data,uint32_t& size) ;
    virtual uint32_t serial_size() const ;
    virtual std::ostream & print(std::ostream &out, uint16_t indent = 0);

private:
    ZR::ZR_Number m_Amount;
    std::string m_Currency;
};


// TODO: move Order item here

#endif // RSZRREMOTEITEMS_H
