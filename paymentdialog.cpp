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

#include "paymentdialog.h"
#include "ui_paymentdialog.h"
#include "Currency.h"
#include "TmLocalCoordinator.h"
#include "Credit.h"
#include "zrtypes.h"
#include "Payment.h"

#include "retroshare/rspeers.h"

#include <QMessageBox>

/// @brief 
///
/// @param payee
/// @param parent
PaymentDialog::PaymentDialog( const std::string & payee, QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::PaymentDialog ),
    m_payee( payee )
{
    const std::string peername = rsPeers->getPeerName( payee );

    ui->setupUi(this);
    ui->label->setText( QString::fromUtf8( peername.c_str() ) );
    int index = 0;
    while(Currency::currencyNames[ index ])
    {
        ui->currencySelector->addItem( Currency::currencyNames[ index ] );
        index++;
    }
    connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT( payTo() ) );
    connect( ui->currencySelector, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadAvailableFunds( QString ) ) );
    loadAvailableFunds();
}

/// @brief Destructor
/// @details Destroys the user interface.
//
PaymentDialog::~PaymentDialog()
{
    delete ui;
}

/// @brief Pay to
void PaymentDialog::payTo()
{
    Currency::CurrencySymbols sym = Currency::getCurrencyByName( ui->currencySelector->currentText().toStdString() );
    Payment * payment = new PaymentSpender( m_payee, ZR::ZR_Number::fromDecimalString( ui->amount->text() ), Currency::currencySymbols[ sym ], Payment::PAYMENT );
    TmLocalCoordinator * tm = new TmLocalCoordinator( payment );
    if( ! tm->init() ) delete tm;
}

/// @brief Load available funds
///
/// @param arg
void PaymentDialog::loadAvailableFunds(QString arg)
{
    ui->lcdAvailableFunds->display( availableFunds().toDouble() );
}


/// @brief Determine amount of available funds
///
/// @return 
ZR::ZR_Number PaymentDialog::availableFunds()
{
    Currency::CurrencySymbols sym = Currency::getCurrencyByName( ui->currencySelector->currentText().toStdString() );
    Credit peerCredit( m_payee, Currency::currencySymbols[ sym ] );
    try {
        peerCredit.loadPeer();
    }
    catch( std::exception e ) {
        QMessageBox::critical(0, "Error reading credit", e.what() );
    }

    return  peerCredit.getMyAvailable();
}

// 
// EOF   
