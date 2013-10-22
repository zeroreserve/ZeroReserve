/*!
 * \file Currency.cpp
 * 
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

#include "Currency.h"

#include <iostream>

/**
 * @brief const char * const Currency::currencyNames array
 * 
 */
const char * const Currency::currencyNames[] =
    {
#ifdef ZR_TESTNET
        "Fools Gold (ug)",
        "German Papiermark (1923)",
        "Testnet Bitcoin",
        "Zimbabwe Dollar",
        0
#else

        "UAE Dirham",
        "Afghanistan Afghani",
        "Albania Lek",
        "Armenia Dram",
        "Netherlands Antilles Guilder",
        "Angola Kwanza",
        "Argentina Peso",
        "Australia Dollar",
        "Gold (troy ounce)",
        "Aruba Guilder",
        "Azerbaijan New Manat",
        "Bosnia and Herzegovina Convertible Marka",
        "Barbados Dollar",
        "Bangladesh Taka",
        "Bulgaria Lev",
        "Bahrain Dinar",
        "Burundi Franc",
        "Bermuda Dollar",
        "Brunei Darussalam Dollar",
        "Bolivia Boliviano",
        "Brazil Real",
        "Bahamas Dollar",
        "Bitcoin",
        "Bhutan Ngultrum",
        "Botswana Pula",
        "Belarus Ruble",
        "Belize Dollar",
        "Canada Dollar",
        "Congo/Kinshasa Franc",
        "Switzerland Franc",
        "Chile Peso",
        "China Yuan Renminbi",
        "Colombia Peso",
        "Costa Rica Colon",
        "Cuba Convertible Peso",
        "Cuba Peso",
        "Cape Verde Escudo",
        "Czech Republic Koruna",
        "Djibouti Franc",
        "Denmark Krone",
        "Dominican Republic Peso",
        "Algeria Dinar",
        "Egypt Pound",
        "Eritrea Nakfa",
        "Ethiopia Birr",
        "Euro",
        "Fiji Dollar",
        "Falkland/Malvinas Pound",
        "United Kingdom Pound",
        "Georgia Lari",
        "Guernsey Pound",
        "Ghana Cedi",
        "Gibraltar Pound",
        "Gambia Dalasi",
        "Guinea Franc",
        "Guatemala Quetzal",
        "Guyana Dollar",
        "Hong Kong Dollar",
        "Honduras Lempira",
        "Croatia Kuna",
        "Haiti Gourde",
        "Hungary Forint",
        "Indonesia Rupiah",
        "Israel Shekel",
        "Isle of Man Pound",
        "India Rupee",
        "Iraq Dinar",
        "Iran Rial",
        "Iceland Krona",
        "Jersey Pound",
        "Jamaica Dollar",
        "Jordan Dinar",
        "Japan Yen",
        "Kenya Shilling",
        "Kyrgyzstan Som",
        "Cambodia Riel",
        "Comoros Franc",
        "Korea (North) Won",
        "Korea (South) Won",
        "Kuwait Dinar",
        "Cayman Islands Dollar",
        "Kazakhstan Tenge",
        "Laos Kip",
        "Lebanon Pound",
        "Sri Lanka Rupee",
        "Liberia Dollar",
        "Lesotho Loti",
        "Litecoin",
        "Lithuania Litas",
        "Latvia Lat",
        "Libya Dinar",
        "Morocco Dirham",
        "Moldova Leu",
        "Madagascar Ariary",
        "Macedonia Denar",
        "Myanmar (Burma) Kyat",
        "Mongolia Tughrik",
        "Macau Pataca",
        "Mauritania Ouguiya",
        "Mauritius Rupee",
        "Maldives Rufiyaa",
        "Malawi Kwacha",
        "Mexico Peso",
        "Malaysia Ringgit",
        "Mozambique Metical",
        "Namibia Dollar",
        "Nigeria Naira",
        "Nicaragua Cordoba",
        "Namecoin",
        "Norway Krone",
        "Nepal Rupee",
        "New Zealand Dollar",
        "Oman Rial",
        "Panama Balboa",
        "Peru Nuevo Sol",
        "Papua New Guinea Kina",
        "Philippines Peso",
        "Pakistan Rupee",
        "Poland Zloty",
        "Paraguay Guarani",
        "Qatar Riyal",
        "Romania New Leu",
        "Serbia Dinar",
        "Russia Ruble",
        "Rwanda Franc",
        "Saudi Arabia Riyal",
        "Solomon Islands Dollar",
        "Seychelles Rupee",
        "Sudan Pound",
        "Sweden Krona",
        "Singapore Dollar",
        "Saint Helena Pound",
        "Sierra Leone Leone",
        "Somalia Shilling",
        "Seborga Luigino",
        "Suriname Dollar",
        "São Tomé and Príncipe Dobra",
        "El Salvador Colon",
        "Syria Pound",
        "Swaziland Lilangeni",
        "Thailand Baht",
        "Tajikistan Somoni",
        "Turkmenistan Manat",
        "Tunisia Dinar",
        "Tonga Pa'anga",
        "Turkey Lira",
        "Trinidad and Tobago Dollar",
        "Tuvalu Dollar",
        "Taiwan New Dollar",
        "Tanzania Shilling",
        "Ukraine Hryvna",
        "Uganda Shilling",
        "United States Dollar",
        "Uruguay Peso",
        "Uzbekistan Som",
        "Venezuela Bolivar",
        "Viet Nam Dong",
        "Vanuatu Vatu",
        "Samoa Tala",
        "CFA Franc BEAC",
        "East Caribbean Dollar",
        "IMF Special Drawing Rights",
        "BCEAO Franc",
        "CFP Franc",
        "Yemen Rial",
        "South Africa Rand",
        "Zambia Kwacha",
        0
#endif // ZR_TESTNET
    };
    
const char * const Currency::currencySymbols[] =
    {
#ifdef ZR_TESTNET
        "FAU",
        "GPM",
        "TBC",
        "ZWD",
        0
#else
        "AED",
        "AFN",
        "ALL",
        "AMD",
        "ANG",
        "AOA",
        "ARS",
        "AUD",
        "XAU",
        "AWG",
        "AZN",
        "BAM",
        "BBD",
        "BDT",
        "BGN",
        "BHD",
        "BIF",
        "BMD",
        "BND",
        "BOB",
        "BRL",
        "BSD",
        "BTC",
        "BTN",
        "BWP",
        "BYR",
        "BZD",
        "CAD",
        "CDF",
        "CHF",
        "CLP",
        "CNY",
        "COP",
        "CRC",
        "CUC",
        "CUP",
        "CVE",
        "CZK",
        "DJF",
        "DKK",
        "DOP",
        "DZD",
        "EGP",
        "ERN",
        "ETB",
        "EUR",
        "FJD",
        "FKP",
        "GBP",
        "GEL",
        "GGP",
        "GHS",
        "GIP",
        "GMD",
        "GNF",
        "GTQ",
        "GYD",
        "HKD",
        "HNL",
        "HRK",
        "HTG",
        "HUF",
        "IDR",
        "ILS",
        "IMP",
        "INR",
        "IQD",
        "IRR",
        "ISK",
        "JEP",
        "JMD",
        "JOD",
        "JPY",
        "KES",
        "KGS",
        "KHR",
        "KMF",
        "KPW",
        "KRW",
        "KWD",
        "KYD",
        "KZT",
        "LAK",
        "LBP",
        "LKR",
        "LRD",
        "LSL",
        "LTC",
        "LTL",
        "LVL",
        "LYD",
        "MAD",
        "MDL",
        "MGA",
        "MKD",
        "MMK",
        "MNT",
        "MOP",
        "MRO",
        "MUR",
        "MVR",
        "MWK",
        "MXN",
        "MYR",
        "MZN",
        "NAD",
        "NGN",
        "NIO",
        "NMC",
        "NOK",
        "NPR",
        "NZD",
        "OMR",
        "PAB",
        "PEN",
        "PGK",
        "PHP",
        "PKR",
        "PLN",
        "PYG",
        "QAR",
        "RON",
        "RSD",
        "RUB",
        "RWF",
        "SAR",
        "SBD",
        "SCR",
        "SDG",
        "SEK",
        "SGD",
        "SHP",
        "SLL",
        "SOS",
        "SPL",
        "SRD",
        "STD",
        "SVC",
        "SYP",
        "SZL",
        "THB",
        "TJS",
        "TMT",
        "TND",
        "TOP",
        "TRY",
        "TTD",
        "TVD",
        "TWD",
        "TZS",
        "UAH",
        "UGX",
        "USD",
        "UYU",
        "UZS",
        "VEF",
        "VND",
        "VUV",
        "WST",
        "XAF",
        "XCD",
        "XDR",
        "XOF",
        "XPF",
        "YER",
        "ZAR",
        "ZMW",
        0
#endif // ZR_TESTNET
    };



/// @brief Constructor
//
Currency::Currency()
{}

/// @brief Destructor
//
Currency::~Currency()
{}

/// @brief 
///
/// @details Gets currency by symbol.
// If the currency symbol can't be found, displays an error
//
/// @param currency
///
/// @return currency symbol
Currency::CurrencySymbols Currency::getCurrencyBySymbol( const std::string & currency )
{
    int index = 0;
    while(Currency::currencySymbols[ index ])
    {
        if( currency == Currency::currencySymbols[ index ])
        {
            return (Currency::CurrencySymbols)index;
        }
        index++;
    }
    std::cerr << "Zero Reserve: Currency Symbol " << currency << " not found" << std::endl;
    return INVALID;
}

/// @brief Get currency by name
///
/// @param currency
///
/// @return currency symbol
Currency::CurrencySymbols Currency::getCurrencyByName( const std::string & currency )
{
    int index = 0;
    while(Currency::currencyNames[ index ])
    {
        if( currency == Currency::currencyNames[ index ])
        {
            return (Currency::CurrencySymbols)index;
        }
        index++;
    }
    std::cerr << "Zero Reserve: Currency Name " << currency << " not found" << std::endl;
    return INVALID;
}

// EOF  
