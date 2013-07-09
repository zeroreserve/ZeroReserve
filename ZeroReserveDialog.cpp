/*
    This file is part of the Zero Reserve Plugin for Retroshare.

    Zero Reserve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zero Reserve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Zero Reserve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ZeroReserveDialog.h"
#include "frienddetailsdialog.h"
#include "paymentdialog.h"
#include "OrderBook.h"

#include "retroshare/rspeers.h"

#include <QMenu>
#include <QStandardItem>
#include <list>


#define IMAGE_FRIENDINFO ":/images/peerdetails_16x16.png"


ZeroReserveDialog::ZeroReserveDialog(QWidget *parent)
: MainPage(parent)
{
    ui.setupUi(this);

    connect(ui.friendSelectionWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuFriendList(QPoint)));

    OrderBook * asks = new OrderBook();
    OrderBook * bids = new OrderBook();
    ui.asksTableView->setModel( asks );
    ui.bidsTableView->setModel( bids );

    /* initialize friends list */
    ui.friendSelectionWidget->setHeaderText(tr("Friend List:"));
    ui.friendSelectionWidget->setModus(FriendSelectionWidget::MODUS_MULTI);
    ui.friendSelectionWidget->setShowType(FriendSelectionWidget::SHOW_GROUP | FriendSelectionWidget::SHOW_SSL);
    ui.friendSelectionWidget->start();

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

    FriendDetailsDialog d( this, peername );
    d.exec();
}

void ZeroReserveDialog::payTo()
{
    FriendSelectionWidget::IdType id = FriendSelectionWidget::IDTYPE_NONE;
    const std::string uid = ui.friendSelectionWidget->selectedId( id );
    const std::string peername = rsPeers->getPeerName( uid );

    PaymentDialog d( this, peername );
    d.exec();
}
