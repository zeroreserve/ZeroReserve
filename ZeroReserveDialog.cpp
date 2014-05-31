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
#include "FriendResetDialog.h"
#include "paymentdialog.h"
#include "OrderBook.h"
#include "MyOrders.h"
#include "ZeroReservePlugin.h"
#include "p3ZeroReserverRS.h"
#include "zrdb.h"
#include "Payment.h"
#include "ZRBitcoin.h"
#include "NewWallet.h"
#include "PeerAddressDialog.h"
#include "CurrentTxList.h"

#include <QMenu>
#include <QTimer>
#include <QStandardItem>
#include <QMessageBox>
#include <QInputDialog>
#include <list>


#define IMAGE_FRIENDINFO ":/images/peerdetails_16x16.png"


ZeroReserveDialog::ZeroReserveDialog(OrderBook * bids, OrderBook * asks, QWidget *parent )
: MainPage(parent)
{
    std::cerr << "Zero Reserve: Setting up main dialog" << std::endl;
    ui.setupUi(this);
    m_update = true;

    Payment::txLogView = ui.paymentHistoryList;
    MyOrders * myOrders = MyOrders::Instance();
    if( myOrders->init() == ZR::ZR_FAILURE ){
        QMessageBox::critical( this, "Zero Reserve", "Could not load my Orders. Suggest exiting RS and fix the problem" );
    }

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
    ui.testnet_warning->setStyleSheet( "QLabel { background-color : red; qproperty-alignment: AlignCenter; }" );
#endif

    ui.ask_price->setValidator( new QDoubleValidator(0) );
    ui.ask_amount->setValidator( new QDoubleValidator(0) );
    ui.bid_price->setValidator( new QDoubleValidator(0) );
    ui.bid_amount->setValidator( new QDoubleValidator(0) );

    connect( ui.friendSelectionWidget, SIGNAL( customContextMenuRequested(QPoint) ), this, SLOT(contextMenuFriendList(QPoint) ) );
    connect( ui.friendSelectionWidget, SIGNAL( doubleClicked(int,QString) ), this, SLOT( friendDetails() ) );
    connect( ui.askButton, SIGNAL( clicked() ), this, SLOT( addAsk() ) );
    connect( ui.bidButton, SIGNAL( clicked() ), this, SLOT( addBid() ) );
    connect( ui.currencySelector2, SIGNAL( currentIndexChanged(QString) ), this, SLOT( loadGrandTotal() ) );
    connect( ui.currencySelector1, SIGNAL( currentIndexChanged(QString) ), bids, SLOT( setCurrency(QString) ) );
    connect( ui.currencySelector1, SIGNAL( currentIndexChanged(QString) ), asks, SLOT( setCurrency(QString) ) );
    connect( ui.currencySelector1, SIGNAL( currentIndexChanged(QString) ), myOrders, SLOT( setCurrency(QString) ) );
    connect( ui.paymentHistoryList, SIGNAL( currentItemChanged(QListWidgetItem*,QListWidgetItem*) ), this, SLOT( loadGrandTotal() ) );
    connect( ui.currentTx, SIGNAL( clicked() ), this, SLOT( showCurrentTx() ) );

    ui.myOrders->setContextMenuPolicy( Qt::CustomContextMenu );
    ui.myOrders->setSelectionBehavior( QAbstractItemView::SelectRows );
    ui.myOrders->setSelectionMode( QAbstractItemView::SingleSelection );
    connect(ui.myOrders, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT( contextMenuMyOrders(const QPoint &) ) );

    ui.asksTableView->setModel( asks );
    ui.bidsTableView->setModel( bids );
    ui.myOrders->setModel( myOrders );

    bids->setCurrency( ui.currencySelector2->currentText() );
    asks->setCurrency( ui.currencySelector2->currentText() );
    myOrders->setCurrency( ui.currencySelector2->currentText() );


    /* initialize friends list */
    ui.friendSelectionWidget->setHeaderText(tr("Friend List:"));
    ui.friendSelectionWidget->setModus(FriendSelectionWidget::MODUS_MULTI);
    ui.friendSelectionWidget->setShowType(FriendSelectionWidget::SHOW_GROUP | FriendSelectionWidget::SHOW_SSL);
    ui.friendSelectionWidget->start();

    loadGrandTotal();
    loadTxLog();
    ZrDB::Instance()->loadBtcContracts();

    ZR::Bitcoin::BtcInfo btcInfo;
    if( ZR::ZR_SUCCESS == ZR::Bitcoin::Instance()->getinfo( btcInfo ) ){
        QString testNet;
        if( btcInfo.testnet ){
#ifdef ZR_TESTNET
            std::cerr << "Zero Reserve: Connected to Bitcoin Client version " <<  btcInfo.version  << " TESTNET" << std::endl;
#else
            QMessageBox::critical( this, "Bitcoin Connection", "Zero Reserve is PRODNET, Bitcoin client is TESTNET. Exiting" );
            exit( -1 );
#endif
        }
        else {
#ifndef ZR_TESTNET
            std::cerr << "Zero Reserve: Connected to Bitcoin Client version " <<  btcInfo.version  << " PRODNET" << std::endl;
#else
            QMessageBox::critical( this, "Bitcoin Connection", "Zero Reserve is TESTNET, Bitcoin client is PRODNET. Exiting" );
            exit( -1 );
#endif
        }
    }
    else {
        QMessageBox::critical( this, "Bitcoin Connection", "Zero Reserve cannot connect to Bitcoin Client" );
    }


    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(janitor()));
    timer->start( 1000 );
}


void ZeroReserveDialog::janitor()
{
    updateFriendList();
    g_ZeroReservePlugin->displayMsg();
    ui.paymentHistoryList->setCurrentRow( 0 ); // make the view emit currentItemChanged
}

void ZeroReserveDialog::loadTxLog()
{
    std::list< ZrDB::TxLogItem > txList;
    QStringList txStringList;
    try{
        ZrDB::Instance()->loadTxLog( txList );
    }
    catch( std::exception e ){
        std::cerr << "Zero Reserve: " << e.what() << std::endl;
    }

    for( std::list< ZrDB::TxLogItem >::const_iterator it = txList.begin(); it != txList.end(); it++ ){
        const ZrDB::TxLogItem & item = *it;
        txStringList.append( item.timestamp.toString() + " : " + item.currency + " : " + item.m_amount.toDecimalQString() );
    }
    ui.paymentHistoryList->insertItems( 0, txStringList );
}

void ZeroReserveDialog::showCurrentTx()
{
    CurrentTxList ctx;
    ctx.exec();
}


void ZeroReserveDialog::loadGrandTotal()
{
    Currency::CurrencySymbols sym = Currency::getCurrencyByName( ui.currencySelector2->currentText().toStdString() );
    std::string currencySym = Currency::currencySymbols[ sym ];
    ZrDB::GrandTotal & gt = ZrDB::Instance()->loadGrandTotal( currencySym );
    ui.lcdTotalCredit->display( gt.our_credit.toDouble() );
    ui.lcdTotalDebt->display( gt.debt.toDouble() );
    ui.lcdtotalOutstanding->display( gt.outstanding.toDouble() );
    ui.lcdBalance->display( gt.balance.toDouble() );
    ui.lcdTheirCredit->display( gt.credit.toDouble() );
}

void ZeroReserveDialog::contextMenuFriendList(QPoint)
{
    m_update = false; // updating friend list destroys selection. This protects from updating.
    QMenu contextMnu(this);

    int selectedCount = ui.friendSelectionWidget->selectedItemCount();

    FriendSelectionWidget::IdType idType;
    ui.friendSelectionWidget->selectedId(idType);

    QAction *action = contextMnu.addAction(QIcon(), tr("Pay to..."), this, SLOT(payTo()));
    action->setEnabled(selectedCount);

    contextMnu.addSeparator();

    action = contextMnu.addAction(QIcon(IMAGE_FRIENDINFO), tr("Friend Details..."), this, SLOT(friendDetails()));
    action->setEnabled(selectedCount == 1 && idType == FriendSelectionWidget::IDTYPE_SSL);

    action = contextMnu.addAction(QIcon(IMAGE_FRIENDINFO), tr("Friend Reset..."), this, SLOT(friendReset()));
    action->setEnabled(selectedCount == 1 && idType == FriendSelectionWidget::IDTYPE_SSL);

    contextMnu.exec(QCursor::pos());
    m_update = true;
}

void ZeroReserveDialog::friendDetails()
{
    FriendSelectionWidget::IdType id = FriendSelectionWidget::IDTYPE_NONE;
    const std::string uid = ui.friendSelectionWidget->selectedId( id );
    const std::string peername = rsPeers->getPeerName( uid );

    FriendDetailsDialog d( uid, this, peername );
    d.exec();
}


void ZeroReserveDialog::friendReset()
{
    FriendSelectionWidget::IdType id = FriendSelectionWidget::IDTYPE_NONE;
    const std::string uid = ui.friendSelectionWidget->selectedId( id );
    const std::string peername = rsPeers->getPeerName( uid );

    FriendResetDialog d( uid, this, peername );
    d.exec();
}

void ZeroReserveDialog::payTo()
{
    FriendSelectionWidget::IdType id = FriendSelectionWidget::IDTYPE_NONE;
    const std::string uid = ui.friendSelectionWidget->selectedId( id );
    PaymentDialog d( uid, this );
    d.exec();
}

void ZeroReserveDialog::addBid()
{
    OrderBook * bids = static_cast<OrderBook*>(ui.bidsTableView->model());
    doOrder( bids, OrderBook::Order::BID, ZR::ZR_Number::fromDecimalString( ui.bid_price->text() ), ZR::ZR_Number::fromDecimalString( ui.bid_amount->text() ) );
}

void ZeroReserveDialog::addAsk()
{
    OrderBook * asks = static_cast<OrderBook*>(ui.asksTableView->model());
    doOrder( asks, OrderBook::Order::ASK, ZR::ZR_Number::fromDecimalString( ui.ask_price->text() ), ZR::ZR_Number::fromDecimalString( ui.ask_amount->text() ) );
}

void ZeroReserveDialog::cancelOrder()
{
    QModelIndexList indexes = ui.myOrders->selectionModel()->selection().indexes();
    if( indexes.empty() )
        return;
    MyOrders * myOrders = static_cast< MyOrders* >( ui.myOrders->model() );
    myOrders->cancelOrder( indexes.at( 0 ).row() );
}

void ZeroReserveDialog::contextMenuMyOrders( const QPoint & )
{
    QMenu contextMnu(this);
    QAction *action = contextMnu.addAction(QIcon(), tr("Cancel Order"), this, SLOT(cancelOrder()));
    action->setEnabled(true);
    contextMnu.exec(QCursor::pos());
}


void ZeroReserveDialog::doOrder( OrderBook * book, OrderBook::Order::OrderType type, ZR::ZR_Number price, ZR::ZR_Number amount )
{
    if( amount <= 0 || price <= 0 ){
        QMessageBox::warning( this, "New Order", "Amount and Price must be greater than zero" );
        return;
    }

    OrderBook::Order * order = new OrderBook::Order();
    order->m_price = price;
    order->m_currency = Currency::getCurrencyByName( ui.currencySelector1->currentText().toStdString() );
    order->m_amount = amount;
    order->m_orderType = type;
    order->m_isMyOrder = true;
    order->m_purpose = OrderBook::Order::NEW;
    order->m_timeStamp = QDateTime::currentMSecsSinceEpoch();
    order->setOrderId();

    if( type == OrderBook::Order::ASK ){
        order->m_btcAddr = ZR::Bitcoin::Instance()->mkOrderAddress( amount );

        if( order->m_btcAddr.empty() ){
            g_ZeroReservePlugin->placeMsg( "Cannot place order" );
            delete order;
            return;
        }
    }

    book->processMyOrder( order );
}



void ZeroReserveDialog::updateFriendList()
{
    if( !m_update )return;
    ui.friendSelectionWidget->setModus(FriendSelectionWidget::MODUS_SINGLE );
}

