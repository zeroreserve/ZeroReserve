/*!
 * \file RemotePaymentRequestDialog.cpp
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


#include "RemotePaymentRequestDialog.h"
#include "ui_RemotePaymentRequestDialog.h"

#include "Currency.h"
#include "p3ZeroReserverRS.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"
#include "ZeroReservePlugin.h"
#include "Payment.h"

#include "util/radix64.h"

#include <openssl/sha.h>


/// @brief Constructor
///
/// @param parent
RemotePaymentRequestDialog::RemotePaymentRequestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemotePaymentRequestDialog)
{
    ui->setupUi(this);
    int index = 0;
    while(Currency::currencyNames[ index ])
    {
        ui->currencySelector->addItem( Currency::currencyNames[ index ] );
        index++;
    }

    ui->amount->setValidator( new QDoubleValidator( 1E-8, 1E12, 0) );

    connect( ui->currencySelector, SIGNAL( currentIndexChanged( QString ) ), this, SLOT( currencySelected( QString ) ) );
    connect( ui->amount, SIGNAL( textEdited( QString ) ), this, SLOT( amountEntered( QString ) ) );
    connect( ui->buttonBox, SIGNAL( accepted() ), this, SLOT( sendRequest() ) );
}


/// @brief Destructor
/// @details Destroys/deletes the user interface
//
RemotePaymentRequestDialog::~RemotePaymentRequestDialog()
{
    delete ui;
}


/// @brief Currency selected
//
/// @details 
///
/// @param currency
void RemotePaymentRequestDialog::currencySelected( QString currency )
{
    QString amount = ui->amount->text();
    if( !amount.isEmpty() )
    {
        ui->payAddress->setText( getPayAddress( currency, amount ) );
    }
}

/// @brief Amount entered 
//
/// @details 
/// @param amount
void RemotePaymentRequestDialog::amountEntered( QString amount )
{
    QString currency =  QString::fromStdString( Currency::currencySymbols[ Currency::getCurrencyByName( ui->currencySelector->currentText().toStdString() ) ] );
    ui->payAddress->setText( getPayAddress( currency, amount ) );
}


/// @brief Get address to pay
///
/// @todo FIXME: the getOwnId() must be replaced by a secret - else the originator of the order can be calculated
/// FIXME: by friends and friends of friends
// 
/// @param amount
/// @param currency
///
/// @return 
QString RemotePaymentRequestDialog::getPayAddress( QString amount, QString currency )
{
    unsigned char md[ SHA256_DIGEST_LENGTH ];
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    // FIXME: the getOwnId() must be replaced by a secret - else the originator of the order can be calculated
    // FIXME: by friends and friends of friends
    QString data = QString::fromStdString( p3zr->getOwnId() ) + amount + currency + QString::number( rand() );
    int length = data.length();
    unsigned char * buff = new unsigned char[ length ];
    memcpy( buff, data.toAscii(), length );
    SHA256( buff, length, md);
    std::string payAddress;
    Radix64::encode( (const char*)md, SHA256_DIGEST_LENGTH, payAddress );
    delete [] buff;
    return QString::fromStdString( payAddress );
}

/// @brief Send request
//
void RemotePaymentRequestDialog::sendRequest()
{
    p3ZeroReserveRS * p3zr = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    ZR::VirtualAddress address = ui->payAddress->text().toStdString();
    ZR::ZR_Number amount = ZR::ZR_Number::fromDecimalString( ui->amount->text() );
    Currency::CurrencySymbols currencySym = Currency::getCurrencyByName(ui->currencySelector->currentText().toStdString() );
    std::string currency = Currency::currencySymbols[ currencySym ];
    p3zr->sendRemote( address, amount, currency );
    Payment::addMyRequest( address, Payment::Request( amount, currencySym ));
}

// EOF   
