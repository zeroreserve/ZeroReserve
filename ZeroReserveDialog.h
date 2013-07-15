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

#include <QPoint>

#include "retroshare-gui/mainpage.h"
#include "ui_ZeroReserveDialog.h"
#include "p3ZeroReserverRS.h"


class RsPeers;


class ZeroReserveDialog : public MainPage
{
    Q_OBJECT

public:
    ZeroReserveDialog( OrderBook * bids, OrderBook * asks, RsPeers* peers, p3ZeroReserveRS * p3zr, QWidget *parent = 0 );


public slots:
    void addBid();
    void addAsk();

private slots:
    void contextMenuFriendList(QPoint);
    void friendDetails();
    void payTo();

private:
    Ui::ZeroReserveDialog ui;
    RsPeers * m_Peers;
    p3ZeroReserveRS * m_ZeroReserveRS;

private:
    void publishOrder( OrderBook::Order * order );

};

#endif // ZERORESERVEDIALOG_H
