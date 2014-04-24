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


#ifndef TMLOCALCOHORTE_H
#define TMLOCALCOHORTE_H

#include "zrtypes.h"
#include "TransactionManager.h"

class RsZeroReserveInitTxItem;
class Payment;

class TmLocalCohorte : public TransactionManager
{
public:
    TmLocalCohorte( const ZR::TransactionId & txId );
    virtual ~TmLocalCohorte();

    virtual ZR::RetVal init();

protected:
    virtual ZR::RetVal processItem( RsZeroReserveItem * baseItem );
    virtual ZR::RetVal abortTx( RsZeroReserveTxItem * item );

    virtual void rollback();

private:
    Payment * m_payment;
};

#endif // TMLOCALCOHORTE_H
