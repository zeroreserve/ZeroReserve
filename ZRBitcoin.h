#ifndef ZRBITCOIN_H
#define ZRBITCOIN_H

#include "zrtypes.h"

namespace ZR
{

class Bitcoin {
public:
    ZR::RetVal commit() = 0;
    ZR::ZR_Number getBalance() = 0;

};

}

#endif // ZRBITCOIN_H
