#include "NewWallet.h"
#include "ui_NewWallet.h"

#include "ZRBitcoin.h"

NewWallet::NewWallet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewWallet)
{
    ui->setupUi(this);
    connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT( wallet() ) );
    connect( ui->newElectrum, SIGNAL( toggled( bool ) ), this, SLOT(makeSeed( bool ) ) );
    walletType = ZR::Wallet::INVALID;
}

NewWallet::~NewWallet()
{
    delete ui;
}


void NewWallet::makeSeed( bool enabled )
{
    if( !enabled ){
        ui->seed->setText( "" );
        return;
    }
    ZR::Wallet * wallet = ZR::Bitcoin::Instance()->mkWallet( ZR::Wallet::ELECTRUMSEED );
    ZR::WalletSeed seed = wallet->seed();
    ui->seed->setText( QString::fromStdString( seed ) );
}

void NewWallet::wallet()
{
    seed = ui->seed->text();
    if( ui->brainWallet->isEnabled() ){
        walletType = ZR::Wallet::BRAINWALLET;
    }
    else if( ui->importElectrum->isEnabled() || ui->newElectrum->isEnabled() ) {
        walletType = ZR::Wallet::ELECTRUMSEED;
    }
    else {
        walletType = ZR::Wallet::INVALID;
    }
}
