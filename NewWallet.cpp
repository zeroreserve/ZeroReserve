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
    walletType = INVALID;
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
    ZR::WalletSeed seed = ZR::Bitcoin::Instance()->getSeed();
    ui->seed->setText( QString::fromStdString( seed ) );
}

void NewWallet::wallet()
{
    seed = ui->seed->text();
    if( ui->brainWallet->isEnabled() ){
        walletType = BRAINWALLET;
    }
    else if( ui->importElectrum->isEnabled() || ui->newElectrum->isEnabled() ) {
        walletType = ELECTRUMSEED;
    }
    else {
        walletType = INVALID;
    }
}
