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

#ifndef TMLOCALCOORDINATOR_H
#define TMLOCALCOORDINATOR_H

#include "TransactionManager.h"

class Payment;

class TmLocalCoordinator : public TransactionManager
{
public:
    TmLocalCoordinator( Payment * payment );
    virtual ~TmLocalCoordinator();

    virtual ZR::RetVal init();

protected:
    virtual ZR::RetVal processItem( RsZeroReserveTxItem * item );
    virtual ZR::RetVal abortTx( RsZeroReserveTxItem * item );

    virtual void rollback();
    virtual bool isTimedOut();

    static const ZR::TransactionId mkId();

private:
    Payment * m_payment;

    static unsigned int sequence;
};

#endif // TMLOCALCOORDINATOR_H
