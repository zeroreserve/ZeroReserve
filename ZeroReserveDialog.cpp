/*!
 * \file ZeroReserveDialog.cpp
 * 
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
#include "Payment.h"
#include "RemotePaymentDialog.h"
#include "RemotePaymentRequestDialog.h"
#include "ZRBitcoin.h"
#include "NewWallet.h"
#include "BitcoinAddressList.h"
#include "PeerAddressDialog.h"

#include <QMenu>
#include <QStandardItem>
#include <list>


#define IMAGE_FRIENDINFO ":/images/peerdetails_16x16.png"


/**
 * @brief Create the main dialog for Zero Reserve
 * 
 *
 * @param bids
 * @param asks
 * @param parent
 */
ZeroReserveDialog::ZeroReserveDialog(OrderBook * bids, OrderBook * asks, QWidget *parent )
: MainPage(parent)
{
    std::cerr << "Zero Reserve: Setting up main dialog" << std::endl;
    ui.setupUi(this);

    Payment::txLogView = ui.paymentHistoryList;

    int index = 0;
    while(Currency::currencyNames[ index ])
    {
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

    MyOrders * myOrders = new MyOrders( bids, asks );

    connect( ui.friendSelectionWidget, SIGNAL( customContextMenuRequested(QPoint) ), this, SLOT(contextMenuFriendList(QPoint) ) );
    connect( ui.friendSelectionWidget, SIGNAL( doubleClicked(int,QString) ), this, SLOT( friendDetails() ) );
    connect( ui.askButton, SIGNAL( clicked() ), this, SLOT( addAsk() ) );
    connect( ui.bidButton, SIGNAL( clicked() ), this, SLOT( addBid() ) );
    connect( ui.currencySelector2, SIGNAL( currentIndexChanged(QString) ), this, SLOT( loadGrandTotal() ) );
    connect( ui.currencySelector1, SIGNAL( currentIndexChanged(QString) ), bids, SLOT( setCurrency(QString) ) );
    connect( ui.currencySelector1, SIGNAL( currentIndexChanged(QString) ), asks, SLOT( setCurrency(QString) ) );
    connect( ui.currencySelector1, SIGNAL( currentIndexChanged(QString) ), myOrders, SLOT( setCurrency(QString) ) );
    connect( ui.remotePayment, SIGNAL( clicked() ), this, SLOT( remotePayment() ) );
    connect( ui.remoteRequest, SIGNAL( clicked() ), this, SLOT( remoteRequest() ) );
    connect( ui.paymentHistoryList, SIGNAL( currentItemChanged(QListWidgetItem*,QListWidgetItem*) ), this, SLOT( loadGrandTotal() ) );

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

    ui.MyAddresses->setContextMenuPolicy( Qt::CustomContextMenu );
    ui.MyAddresses->setSelectionBehavior( QAbstractItemView::SelectRows );
    ui.MyAddresses->setSelectionMode( QAbstractItemView::SingleSelection );
    connect(ui.MyAddresses, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT( contextMenuMyAddresses( const QPoint &) ) );

    ui.PeerAddresses->setContextMenuPolicy( Qt::CustomContextMenu );
    ui.PeerAddresses->setSelectionBehavior( QAbstractItemView::SelectRows );
    ui.PeerAddresses->setSelectionMode( QAbstractItemView::SingleSelection );
    connect(ui.PeerAddresses, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT( contextMenuPeerAddresses( const QPoint &) ) );

    BitcoinAddressList * myWallets = new BitcoinAddressList();
    BitcoinAddressList * peerAddrs = new BitcoinAddressList();

    ui.MyAddresses->setModel( myWallets );
    ui.PeerAddresses->setModel( peerAddrs );

    loadGrandTotal();
    loadTxLog();
}


/**
 * @brief Load the transaction log
 * 
 */
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

    for( std::list< ZrDB::TxLogItem >::const_iterator it = txList.begin(); it != txList.end(); it++ )
    {
        const ZrDB::TxLogItem & item = *it;
        txStringList.append( item.timestamp.toString() + " : " + item.currency + " : " + item.m_amount.toDecimalQString() );
    }
    ui.paymentHistoryList->insertItems( 0, txStringList );
}


/**
 * @brief Load grand total
 * 
 */
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

/**
 * @brief Create context menu friend list
 *
 * @param QPoint
 */
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

/**
 * @brief Friend details display?
 * 
 */
void ZeroReserveDialog::friendDetails()
{
    FriendSelectionWidget::IdType id = FriendSelectionWidget::IDTYPE_NONE;
    const std::string uid = ui.friendSelectionWidget->selectedId( id );
    const std::string peername = rsPeers->getPeerName( uid );

    FriendDetailsDialog d( uid, this, peername );
    d.exec();
}

/**
 * @brief Payment to selected friend
 */
void ZeroReserveDialog::payTo()
{
    FriendSelectionWidget::IdType id = FriendSelectionWidget::IDTYPE_NONE;
    const std::string uid = ui.friendSelectionWidget->selectedId( id );
    PaymentDialog d( uid, this );
    d.exec();
}

/**
 * @brief Add bid
 * 
 */
void ZeroReserveDialog::addBid()
{
    OrderBook * bids = static_cast<OrderBook*>(ui.bidsTableView->model());
    doOrder( bids, OrderBook::Order::BID, ZR::ZR_Number::fromDecimalString( ui.bid_price->text() ), ZR::ZR_Number::fromDecimalString( ui.bid_amount->text() ) );
}

/**
 * @brief Add ask
 */
void ZeroReserveDialog::addAsk()
{
    OrderBook * asks = static_cast<OrderBook*>(ui.asksTableView->model());
    doOrder( asks, OrderBook::Order::ASK, ZR::ZR_Number::fromDecimalString( ui.ask_price->text() ), ZR::ZR_Number::fromDecimalString( ui.ask_amount->text() ) );
}

/**
 * @brief Cancel order
 * 
 */
void ZeroReserveDialog::cancelOrder()
{
    QModelIndexList indexes = ui.myOrders->selectionModel()->selection().indexes();
    if( indexes.empty() )
        return;
    MyOrders * myOrders = static_cast< MyOrders* >( ui.myOrders->model() );
    myOrders->cancelOrder( indexes.at( 0 ).row() );
}

/**
 * @brief Display context menu of my orders
 *
 * @param 
 */
void ZeroReserveDialog::contextMenuMyOrders( const QPoint & )
{
    QMenu contextMnu(this);
    QAction *action = contextMnu.addAction(QIcon(), tr("Cancel Order"), this, SLOT(cancelOrder()));
    action->setEnabled(true);
    contextMnu.exec(QCursor::pos());
}


/**
 * @brief Do order
 *
 * @param book
 * @param type
 * @param price
 * @param amount
 */
void ZeroReserveDialog::doOrder( OrderBook * book, OrderBook::Order::OrderType type, ZR::ZR_Number price, ZR::ZR_Number amount )
{
    OrderBook::Order * order = new OrderBook::Order();
    order->m_price = price;
    order->m_currency = Currency::getCurrencyByName( ui.currencySelector1->currentText().toStdString() );
    order->m_amount = amount;
    order->m_orderType = type;
    order->m_isMyOrder = true;
    order->m_purpose = OrderBook::Order::NEW;
    order->m_timeStamp = QDateTime::currentMSecsSinceEpoch();
    order->setOrderId();
    book->processMyOrder( order );
}


/**
 * @brief Remote request
 */
void ZeroReserveDialog::remoteRequest()
{
    RemotePaymentRequestDialog d;
    d.exec();
}

/**
 * @brief Remote payment
 * 
 */
void ZeroReserveDialog::remotePayment()
{
    RemotePaymentDialog d;
    d.exec();
}


/**
 * @brief Update friend list
 * 
 * @todo FIXME
 * 
 */
void ZeroReserveDialog::updateFriendList()
{
// FIXME:    ui.friendSelectionWidget->setModus(FriendSelectionWidget::MODUS_MULTI);
}

///////////////////////// My Addresses //////////////////////////


/**
 * @brief Context menu for my addresses
 *
 * @param 
 */
void ZeroReserveDialog::contextMenuMyAddresses( const QPoint & )
{
    QMenu contextMnu(this);
    QAction *action = contextMnu.addAction(QIcon(), tr("New Wallet"), this, SLOT( newWallet() ) );
    action->setEnabled(true);
    contextMnu.exec(QCursor::pos());
}


/**
 * @brief New wallet creator
 * 
 */
void ZeroReserveDialog::newWallet()
{
    NewWallet d( this );
    if( d.exec() == 0 )
        return;
    ZR::MyWallet * wallet = ZR::Bitcoin::Instance()->mkWallet( d.m_walletType );
    wallet->setSeed( d.m_seed.toStdString() );
    BitcoinAddressList * addrs = static_cast< BitcoinAddressList* >( ui.MyAddresses->model() );
    wallet->persist();
    addrs->addWallet( wallet );
}

///////////////////////// Peer Addresses //////////////////////////

/**
 * @brief Context menu of peer addresses
 *
 * @param 
 */
void ZeroReserveDialog::contextMenuPeerAddresses( const QPoint & )
{
    QMenu contextMnu(this);
    QAction *action = contextMnu.addAction(QIcon(), tr("New Peer Address"), this, SLOT( newPeerAddress() ) );
    action->setEnabled(true);
    contextMnu.exec(QCursor::pos());
}

/**
 * @brief Add a new peer address
 * 
 */
void ZeroReserveDialog::newPeerAddress()
{
    PeerAddressDialog d( this );
    if( d.exec() == 0 )
        return;
    ZR::BitcoinAddressEntry * wallet = new ZR::BitcoinAddressEntry( d.m_address );
    wallet->setNick( d.m_nick );
    BitcoinAddressList * addrs = static_cast< BitcoinAddressList* >( ui.PeerAddresses->model() );
    wallet->persist();
    addrs->addWallet( wallet );
}

//   EOF   
