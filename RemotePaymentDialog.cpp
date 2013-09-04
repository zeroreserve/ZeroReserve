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

#include "RemotePaymentDialog.h"
#include "ui_RemotePaymentDialog.h"

#include "Payment.h"

RemotePaymentDialog::RemotePaymentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemotePaymentDialog)
{
    ui->setupUi(this);
    connect( ui->destination, SIGNAL(textEdited(QString)), this, SLOT( loadPayment( QString ) ) );
}

RemotePaymentDialog::~RemotePaymentDialog()
{
    delete ui;
}


void RemotePaymentDialog::payTo()
{

}

void RemotePaymentDialog::loadPayment( QString address )
{
    Payment::Request req = Payment::getRequest( address.toStdString() );
    ui->amount->display( req.m_Amount.toDouble() );
    ui->currency->setText( Currency::currencyNames[ req.m_Currency ] );
}
