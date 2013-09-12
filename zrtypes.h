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


#ifndef ZRTYPES_H
#define ZRTYPES_H

#include <boost/rational.hpp>
#include <string>
#include <QString>
#include <sstream>
#include <math.h>
#include <stdint.h>

namespace ZR {

class ZR_Number : public boost::rational< int64_t >
{
public:
    ZR_Number() : boost::rational< int64_t >::rational( 0 ){}

    ZR_Number( int64_t numerator, int64_t denumerator = 1):
        boost::rational< int64_t >::rational( numerator, denumerator){}

    ZR_Number( const  boost::rational< int64_t > & a ) :
        boost::rational< int64_t >::rational( a.numerator(), a.denominator() ){}

    double toDouble() const
    {
        return boost::rational_cast<double>( *this );
    }

    static ZR_Number fromFractionString( const std::string & s_num )
    {
        boost::rational< int64_t > num;
        std::istringstream sNum( s_num );
        sNum >> num;
        return num;
    }

    static ZR_Number fromDecimalString( const std::string & s_num )
    {
        const char delim = ( s_num.find( ',' ) != std::string::npos ) ? ',' : '.';
        std::istringstream iss( s_num );
        std::string sIntPart;
        std::string sFracPart;
        std::getline(iss, sIntPart, delim);
        std::getline(iss, sFracPart, delim);
        int factor = (int)pow(10, sFracPart.length() );
        int intPart = strtol( sIntPart.c_str(), NULL, 10 );
        int fracPart = strtol( sFracPart.c_str(), NULL, 10 );
        if( intPart < 0 ) fracPart = -fracPart;
        ZR_Number zrnum( intPart * factor + fracPart, factor );
        return zrnum;
    }
    static ZR_Number fromDecimalString( QString s_num )
    {
        return fromDecimalString( s_num.toStdString() );
    }

    std::string toStdString() const
    {
        std::ostringstream o;
        o << *this;
        return o.str();
    }
    int length()
    {
        return toStdString().length();
    }
    QString toQString()
    {
        return QString::fromStdString( toStdString() );
    }

    std::string toDecimalStdString() const
    {
        std::ostringstream o;
        o << toDouble();
        return o.str();
    }
    QString toDecimalQString() const
    {
        return QString::fromStdString( toDecimalStdString() );
    }
};

enum RetVal {
    ZR_FAILURE = 0,
    ZR_SUCCESS,
    ZR_FINISH
};

/**
 * @brief serves as the virtual address of a Turtle style tunnel and also as a TX id for remote payments
 */
typedef std::string VirtualAddress;
/**
 * @brief Transaction ID for local payments
 */
typedef std::string TransactionId;

typedef std::string PeerAddress;

}

#endif // ZRTYPES_H
