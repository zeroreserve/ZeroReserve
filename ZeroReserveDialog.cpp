#include "ZeroReserveDialog.h"




ZeroReserveDialog::ZeroReserveDialog(QWidget *parent)
: MainPage(parent)
{
    ui.setupUi(this);

    /* initialize friends list */
    ui.friendSelectionWidget->setHeaderText(tr("Friend List:"));
    ui.friendSelectionWidget->setModus(FriendSelectionWidget::MODUS_MULTI);
    ui.friendSelectionWidget->setShowType(FriendSelectionWidget::SHOW_GROUP | FriendSelectionWidget::SHOW_SSL);
    ui.friendSelectionWidget->start();

}
