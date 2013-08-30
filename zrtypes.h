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

    static ZR_Number fromString( const std::string & s_num )
    {
      // TODO locale doesn't quite work. We need to accomodate any of dot or comma
        char delim = '.';
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
    static ZR_Number fromString( QString s_num )
    {
        return fromString( s_num.toStdString() );
    }

    std::string toStdString()
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


}

#endif // ZRTYPES_H
