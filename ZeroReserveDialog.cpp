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

#include "ZeroReserveDialog.h"
#include "frienddetailsdialog.h"
#include "paymentdialog.h"
#include "OrderBook.h"
#include "MyOrders.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"
#include "zrdb.h"

#include <QMenu>
#include <QStandardItem>
#include <list>


#define IMAGE_FRIENDINFO ":/images/peerdetails_16x16.png"


ZeroReserveDialog::ZeroReserveDialog(OrderBook * bids, OrderBook * asks, QWidget *parent )
: MainPage(parent)
{
    ui.setupUi(this);
    int index = 0;
    while(Currency::currencyNames[ index ]){
        ui.currencySelector2->addItem( Currency::currencyNames[ index ] );
#ifdef ZR_TESTNET
        if(QString(Currency::currencyNames[ index ]) != "Testnet Bitcoin" ){
#else
        if(QString(Currency::currencyNames[ index ]) != "Bitcoin" ){
#endif
            ui.currencySelector1->addItem( Currency::currencyNames[ index ] );
        }
        index++;
    }
#ifdef ZR_TESTNET
    ui.testnet_warning->setText( "TestNet" );
    ui.testnet_warning->setStyleSheet( "QLabel { background-color : red; }" );
#endif

    ui.ask_price->setValidator( new QDoubleValidator(0) );
    ui.ask_amount->setValidator( new QDoubleValidator(0) );
    ui.bid_price->setValidator( new QDoubleValidator(0) );
    ui.bid_amount->setValidator( new QDoubleValidator(0) );

    connect(ui.friendSelectionWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuFriendList(QPoint)));
    connect(ui.friendSelectionWidget, SIGNAL(doubleClicked(int,QString)), this, SLOT(friendDetails()));
    connect(ui.askButton, SIGNAL(clicked()), this, SLOT(addAsk()));
    connect(ui.bidButton, SIGNAL(clicked()), this, SLOT(addBid()));
    connect(ui.currencySelector2, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadGrandTotal(QString) ) );

    ui.asksTableView->setModel( asks );
    ui.bidsTableView->setModel( bids );
    ui.myOrders->setModel( new MyOrders() );

    /* initialize friends list */
    ui.friendSelectionWidget->setHeaderText(tr("Friend List:"));
    ui.friendSelectionWidget->setModus(FriendSelectionWidget::MODUS_MULTI);
    ui.friendSelectionWidget->setShowType(FriendSelectionWidget::SHOW_GROUP | FriendSelectionWidget::SHOW_SSL);
    ui.friendSelectionWidget->start();

    loadGrandTotal();
}

void ZeroReserveDialog::loadGrandTotal(QString)
{
    Currency::CurrencySymbols sym = Currency::getCurrencyByName( ui.currencySelector2->currentText().toStdString() );
    std::string currencySym = Currency::currencySymbols[ sym ];
    const ZrDB::GrandTotal & gt = ZrDB::Instance()->loadGrandTotal( currencySym );
    ui.lcdTotalCredit->display( gt.our_credit );
    ui.lcdTotalDebt->display( gt.debt );
    ui.lcdtotalOutstanding->display( gt.outstanding );
    ui.lcdBalance->display( gt.balance );
    ui.lcdTheirCredit->display( gt.credit );
}

void ZeroReserveDialog::contextMenuFriendList(QPoint)
{
    QMenu contextMnu(this);

    int selectedCount = ui.friendSelectionWidget->selectedItemCount();

    FriendSelectionWidget::IdType idType;
    ui.friendSelectionWidget->selectedId(idType);

    QAction *action = contextMnu.addAction(QIcon(), tr("Pay to..."), this, SLOT(payTo()));
    action->setEnabled(selectedCount);

    contextMnu.addSeparator();

    action = contextMnu.addAction(QIcon(IMAGE_FRIENDINFO), tr("Friend Details..."), this, SLOT(friendDetails()));
    action->setEnabled(selectedCount == 1 && idType == FriendSelectionWidget::IDTYPE_SSL);

    contextMnu.exec(QCursor::pos());
}

void ZeroReserveDialog::friendDetails()
{
    FriendSelectionWidget::IdType id = FriendSelectionWidget::IDTYPE_NONE;
    const std::string uid = ui.friendSelectionWidget->selectedId( id );
    const std::string peername = rsPeers->getPeerName( uid );

    FriendDetailsDialog d( uid, this, peername );
    d.exec();
}

void ZeroReserveDialog::payTo()
{
    FriendSelectionWidget::IdType id = FriendSelectionWidget::IDTYPE_NONE;
    const std::string uid = ui.friendSelectionWidget->selectedId( id );
    const std::string peername = rsPeers->getPeerName( uid );

    PaymentDialog d( uid, this, peername );
    d.exec();
}

void ZeroReserveDialog::addBid()
{
    OrderBook * bids = static_cast<OrderBook*>(ui.bidsTableView->model());
    MyOrders * myOrders = static_cast<MyOrders*>(ui.myOrders->model());

    OrderBook::Order * bid = new OrderBook::Order();
    p3ZeroReserveRS * p3zs = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    bid->setPrice( ui.bid_price->text() );
    bid->m_currency = Currency::getCurrencyByName( ui.currencySelector1->currentText().toStdString() );
    bid->m_amount = ui.bid_amount->text();
    bid->m_orderType = OrderBook::Order::BID;
    bid->sent = false;
    bid->m_timeStamp = time(0);
    bid->m_trader_id = p3zs->getOwnId();
    bids->addOrder( bid );
    myOrders->addOrder( bid );
    p3zs->publishOrder( bid );
}

void ZeroReserveDialog::addAsk()
{
    OrderBook * asks = static_cast<OrderBook*>(ui.asksTableView->model());
    MyOrders * myOrders = static_cast<MyOrders*>(ui.myOrders->model());

    OrderBook::Order * ask = new OrderBook::Order();
    p3ZeroReserveRS * p3zs = static_cast< p3ZeroReserveRS* >( g_ZeroReservePlugin->rs_pqi_service() );
    ask->setPrice( ui.ask_price->text() );
    ask->m_currency = Currency::getCurrencyByName( ui.currencySelector1->currentText().toStdString() );
    ask->m_amount = ui.ask_amount->text();
    ask->m_orderType = OrderBook::Order::ASK;
    ask->sent = false;
    ask->m_timeStamp = time(0);
    ask->m_trader_id = p3zs->getOwnId();
    asks->addOrder( ask );
    myOrders->addOrder( ask );
    p3zs->publishOrder( ask );
}

