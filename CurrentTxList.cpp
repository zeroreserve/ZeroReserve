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

#include <QTableWidgetItem>


CurrentTxList::CurrentTxList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CurrentTxList)
{
    ui->setupUi(this);

    int row = 0;
    for( BtcContract::ContractIterator it = BtcContract::contracts.begin(); it != BtcContract::contracts.end(); it++ ){
        BtcContract * contract = *it;
        QString counterparty = QString::fromStdString( contract->getCounterParty() );
        ui->txList->setItem( row, 0, new QTableWidgetItem( counterparty ) );
    }
}

CurrentTxList::~CurrentTxList()
{
    delete ui;
}
