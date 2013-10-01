#ifndef NEWWALLET_H
#define NEWWALLET_H

#include "ZRBitcoin.h"

#include <QDialog>

namespace Ui {
class NewWallet;
}

class NewWallet : public QDialog
{
    Q_OBJECT
    
public:

    explicit NewWallet(QWidget *parent = 0);
    ~NewWallet();

    QString seed;
    ZR::Wallet::WalletType walletType;

private slots:
    void wallet();
    void makeSeed( bool enabled );

    
private:
    Ui::NewWallet *ui;
};

#endif // NEWWALLET_H
