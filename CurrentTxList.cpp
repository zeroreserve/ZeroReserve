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

#include "CurrentTxList.h"
#include "ui_CurrentTxList.h"

#include "BtcContract.h"

#include "retroshare/rspeers.h"


#include <QTableWidgetItem>


CurrentTxList::CurrentTxList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CurrentTxList)
{
    ui->setupUi(this);

    RsStackMutex contractMutex( BtcContract::m_contractMutex );

    for( BtcContract::ContractIterator it = BtcContract::contracts.begin(); it != BtcContract::contracts.end(); it++ ){
        BtcContract * contract = *it;
        ui->txList->insertRow( 0 );

        QString btcTxId = QString::fromStdString( contract->getBtcTxId() );
        ui->txList->setItem( 0, 0, new QTableWidgetItem( btcTxId ) );

        QString counterparty = QString::fromStdString( rsPeers->getPeerName( contract->getCounterParty() ) );
        ui->txList->setItem( 0, 1, new QTableWidgetItem( counterparty ) );

        ui->txList->setItem( 0, 2, new QTableWidgetItem( contract->getFiatAmount().toDecimalQString() ) );

        QString curr = QString::fromStdString( contract->getCurrencySym() );
        ui->txList->setItem( 0, 3, new QTableWidgetItem( curr ) );

        ui->txList->setItem( 0, 4, new QTableWidgetItem( contract->getPrice().toDecimalQString() ) );

        QString party = ( BtcContract::SENDER == contract->getParty() )? "Payer" : "Payee";
        ui->txList->setItem( 0, 5, new QTableWidgetItem( party ) );

        QDateTime creationTime = QDateTime::fromMSecsSinceEpoch( contract->getCreationTime() );
        ui->txList->setItem( 0, 6, new QTableWidgetItem( creationTime.toString() ) );
    }
}

CurrentTxList::~CurrentTxList()
{
    delete ui;
}
