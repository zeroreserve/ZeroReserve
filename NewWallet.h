#ifndef NEWWALLET_H
#define NEWWALLET_H

#include <QDialog>

namespace Ui {
class NewWallet;
}

class NewWallet : public QDialog
{
    Q_OBJECT
    
public:
    enum SeedType {
        BRAINWALLET,
        ELECTRUMSEED,
        INVALID
    };

    explicit NewWallet(QWidget *parent = 0);
    ~NewWallet();

    QString seed;
    SeedType walletType;

private slots:
    void wallet();
    void makeSeed( bool enabled );

    
private:
    Ui::NewWallet *ui;
};

#endif // NEWWALLET_H
