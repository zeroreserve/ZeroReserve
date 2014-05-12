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


#include "FriendResetDialog.h"
#include "ui_FriendResetDialog.h"

#include "Currency.h"
#include "zrdb.h"

#include <QMessageBox>


FriendResetDialog::FriendResetDialog( const std::string & uid, QWidget *parent, const std::string & name ) :
    QDialog(parent),
    ui(new Ui::FriendResetDialog),
    m_id(uid)
{
    ui->setupUi(this);

    int index = 0;
    while(Currency::currencyNames[ index ]){
        ui->currencySelector->addItem( Currency::currencyNames[ index ] );
        index++;
    }
    ui->label->setText( QString::fromUtf8( name.c_str() ) );
    connect( ui->buttonBox, SIGNAL( accepted() ), this, SLOT( resetFriend() ) );
}

FriendResetDialog::~FriendResetDialog()
{
    delete ui;
}


void FriendResetDialog::resetFriend()
{
    Currency::CurrencySymbols sym = ( ui->allCurrencyCheckBox->isChecked() )?
                Currency::INVALID :
                Currency::getCurrencyByName( ui->currencySelector->currentText().toStdString() );

    try{
        ZrDB::Instance()->deletePeerRecord( m_id, sym );
    }
    catch( std::exception e ) {
        QMessageBox::critical( 0, "Error resetting friend credit: ", e.what() );
    }
}
