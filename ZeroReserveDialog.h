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


#ifndef ZERORESERVEDIALOG_H
#define ZERORESERVEDIALOG_H


#include "retroshare-gui/mainpage.h"
#include "ui_ZeroReserveDialog.h"
#include "OrderBook.h"

#include <QPoint>
#include <QString>


class ZeroReserveDialog : public MainPage
{
    Q_OBJECT

public:
    ZeroReserveDialog( OrderBook * bids, OrderBook * asks, QWidget *parent = 0 );
    void updateFriendList();


public slots:
    void addBid();
    void addAsk();

private slots:
    void contextMenuFriendList(QPoint);
    void friendDetails();
    void payTo();
    void loadGrandTotal();
    void cancelOrder();
    void contextMenuMyOrders(const QPoint & );
    void remoteRequest();
    void remotePayment();

// Bitcoin Wallet
    void contextMenuMyAddresses( const QPoint & );
    void newWallet();

    void contextMenuPeerAddresses( const QPoint & );
    void newPeerAddress();
    void refreshWallet();


private:
    void doOrder(OrderBook * book, OrderBook::Order::OrderType type, ZR::ZR_Number price, ZR::ZR_Number amount );
    void loadTxLog();

    Ui::ZeroReserveDialog ui;

};

#endif // ZERORESERVEDIALOG_H
