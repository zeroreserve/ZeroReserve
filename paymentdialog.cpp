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
#include "TransactionManager.h"

PaymentDialog::PaymentDialog( const std::string & payee, QWidget *parent, const std::string & peername ) :
    QDialog( parent ),
    ui( new Ui::PaymentDialog ),
    m_payee( payee )
{
    ui->setupUi(this);
    ui->label->setText( QString::fromUtf8( peername.c_str() ) );
    int index = 0;
    while(Currency::currencyNames[ index ]){
        ui->currencySelector->addItem( Currency::currencyNames[ index ] );
        index++;
    }
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT( payTo()));
}

PaymentDialog::~PaymentDialog()
{
    delete ui;
}

void PaymentDialog::payTo()
{
    TransactionManager * tm = new TransactionManager();
    tm->initCoordinator( m_payee, ui->amount->text().toStdString() );
}
