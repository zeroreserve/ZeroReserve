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


#include "frienddetailsdialog.h"
#include "ui_frienddetailsdialog.h"
#include "Credit.h"
#include "Currency.h"

#include <QString>
#include <QMessageBox>

FriendDetailsDialog::FriendDetailsDialog( const std::string & uid, QWidget *parent, const std::string & name ) :
    QDialog(parent),
    ui(new Ui::FriendDetailsDialog),
    m_id(uid)
{
    ui->setupUi(this);

    int index = 0;
    while(Currency::currencyNames[ index ]){
        ui->currencySelector->addItem( Currency::currencyNames[ index ] );
        index++;
    }

    ui->label->setText( QString::fromUtf8( name.c_str() ) );

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(editFriend()));
    connect(ui->currencySelector, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadPeer(QString) ) );

    loadPeer();
}

void FriendDetailsDialog::loadPeer( QString )
{
    Currency::CurrencySymbols sym = Currency::getCurrencyByName( ui->currencySelector->currentText().toStdString() );
    Credit peerCredit( m_id, Currency::currencySymbols[ sym ] );
    try {
        peerCredit.loadPeer();
    }
    catch( std::exception e ) {
        QMessageBox::critical(0, "Error reading credit", e.what() );
    }
    ui->creditSpinBox->setValue( peerCredit.m_credit.toDouble() );
    ui->yourCredit->display( peerCredit.m_our_credit.toDouble() );
    ui->balance->display( peerCredit.m_balance.toDouble() );
    ui->balance->display( peerCredit.m_allocated.toDouble() );
}

FriendDetailsDialog::~FriendDetailsDialog()
{
    delete ui;
}


void FriendDetailsDialog::editFriend()
{
    Currency::CurrencySymbols sym = Currency::getCurrencyByName( ui->currencySelector->currentText().toStdString() );
    Credit peerCredit( m_id, Currency::currencySymbols[ sym ] );
    peerCredit.loadPeer();
    peerCredit.m_credit = ZR::ZR_Number::fromDecimalString( ui->creditSpinBox->text() );
    try {
        peerCredit.updateCredit();
        peerCredit.publish();
    }
    catch( std::exception e ) {
        QMessageBox::critical( 0, "Error inserting credit", e.what() );
    }
}
