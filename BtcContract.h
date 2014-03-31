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

#ifndef BTCCONTRACT_H
#define BTCCONTRACT_H

#include "zrtypes.h"
#include "Currency.h"

/**
 * @brief Bitcoin sale contract
 *
 * This class implements a local contract of money transfer with a friend. The money
 * is released when an agreed upon Bitcoin transaction has at least 6 confirmations.
 */

class BtcContract
{
public:    
    BtcContract( const std::string & txId, const ZR::ZR_Number & fiatAmount, const std::string & currencySym );

    /** make sure we survive a crash or a shutdown once we're committed */
    void persist();
    /** last steps of this deal - remove from DB */
    void finalize();

private:
    void poll();
    void execute();

private:
    std::string m_txId;
    ZR::ZR_Number m_fiatAmount;
    std::string m_currencySym;

public:
    typedef std::vector< BtcContract* >::const_iterator ContractIterator;
    /** container for all active btcContracts */
    static std::vector< BtcContract* > contracts;
    static void loadContracts();
    static void pollContracts();
    static const unsigned int reqConfirmations;
};

#endif // BTCCONTRACT_H
