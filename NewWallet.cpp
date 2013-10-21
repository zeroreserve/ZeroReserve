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

#include "NewWallet.h"
#include "ui_NewWallet.h"

#include "ZRBitcoin.h"

/// @brief Constructor
///
/// @param parent
NewWallet::NewWallet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewWallet)
{
    ui->setupUi(this);
    connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT( wallet() ) );
    connect( ui->newElectrum, SIGNAL( toggled( bool ) ), this, SLOT(makeSeed( bool ) ) );
    m_walletType = ZR::MyWallet::INVALID;
}

/// @brief Destructor
//
/// @details Removes/destroys the user interface.
NewWallet::~NewWallet()
{
    delete ui;
}


/// @brief Make a seed for wallet
///
/// @param enabled
void NewWallet::makeSeed( bool enabled )
{
    if( !enabled )
    {
        ui->seed->setText( "" );
        return;
    }
    ZR::MyWallet * wallet = ZR::Bitcoin::Instance()->mkWallet( ZR::MyWallet::ELECTRUMSEED );
    ZR::WalletSeed seed = wallet->seed();
    ui->seed->setText( QString::fromStdString( seed ) );
}

/// @brief Wallet of given type
//
void NewWallet::wallet()
{
    m_seed = ui->seed->text();
    if( ui->brainWallet->isEnabled() )
    {
        m_walletType = ZR::MyWallet::BRAINWALLET;
    }
    else if( ui->importElectrum->isEnabled() || ui->newElectrum->isEnabled() ) 
    {
        m_walletType = ZR::MyWallet::ELECTRUMSEED;
    }
    else 
    {
        m_walletType = ZR::MyWallet::INVALID;
    }
}

// EOF   
