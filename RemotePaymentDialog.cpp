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
#include "TmRemoteCoordinator.h"
#include "Router.h"

#include "QMessageBox"

RemotePaymentDialog::RemotePaymentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemotePaymentDialog)
{
    ui->setupUi(this);
    connect( ui->destination, SIGNAL(textEdited(QString)), this, SLOT( loadPayment( QString ) ) );
    connect( ui->buttonBox, SIGNAL( accepted() ), this, SLOT( payTo() ) );
}

RemotePaymentDialog::~RemotePaymentDialog()
{
    delete ui;
}


void RemotePaymentDialog::payTo()
{
    TmRemoteCoordinator * tm = new TmRemoteCoordinator( ui->destination->text().toStdString() );
    Currency::CurrencySymbols sym = Currency::getCurrencyByName( ui->currency->text().toStdString() );
    const std::string & nextHop = Router::Instance()->nextHop( ui->destination->text().toStdString() );
    if( nextHop.empty() ){
        QMessageBox::critical(0, "Router Error", "No route to destination" );
        return;
    }
    Payment * payment = new PaymentSpender( nextHop, ZR::ZR_Number::fromDecimalString( ui->amount->text() ), Currency::currencySymbols[ sym ], Payment::PAYMENT );
    if( ! tm->initCoordinator( payment ) ) delete tm;
}

void RemotePaymentDialog::loadPayment( QString address )
{
    Payment::Request req = Payment::getRequest( address.toStdString() );
    ui->amount->setText( req.m_Amount.toDecimalQString() );
    ui->currency->setText( Currency::currencyNames[ req.m_Currency ] );
}
