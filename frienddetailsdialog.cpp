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
#include "zrdb.h"
#include "Currency.h"

#include <QString>
#include <QMessageBox>

FriendDetailsDialog::FriendDetailsDialog( const std::string & uid, QWidget *parent, const std::string & name ) :
    QDialog(parent),
    ui(new Ui::FriendDetailsDialog),
    m_id(uid)
{
    ui->setupUi(this);
    ui->label->setText( QString::fromUtf8( name.c_str() ) );

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(editFriend()));

    ZrDB::Credit peerCredit;
    Currency::CurrencySymbols sym = Currency::getCurrencyByName( ui->currencySelector->currentText().toStdString() );
    peerCredit.currency = Currency::currencySymbols[ sym ];
    try {
        ZrDB::Instance()->loadPeer( m_id, peerCredit );
    }
    catch( const char * e) {
        QMessageBox::critical(0, "Error reading credit", e);
    }
    ui->creditSpinBox->setValue( QString::fromStdString(peerCredit.credit).toDouble() );
}

FriendDetailsDialog::~FriendDetailsDialog()
{
    delete ui;
}

void FriendDetailsDialog::editFriend()
{
    ZrDB::Credit peerCredit;
    peerCredit.id = m_id;
    peerCredit.credit = ui->creditSpinBox->text().toStdString();
    Currency::CurrencySymbols sym = Currency::getCurrencyByName( ui->currencySelector->currentText().toStdString() );
    peerCredit.currency = Currency::currencySymbols[ sym ];
    try {
        ZrDB::Instance()->storePeer( peerCredit );
    }
    catch( const char * e) {
        QMessageBox::critical(0, "Error inserting credit", e);
    }
}
