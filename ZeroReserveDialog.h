#ifndef ZERORESERVEDIALOG_H
#define ZERORESERVEDIALOG_H

#include <QPoint>

#include "retroshare-gui/mainpage.h"
#include "ui_ZeroReserveDialog.h"


class ZeroReserveDialog : public MainPage
{
    Q_OBJECT

public:
    ZeroReserveDialog( QWidget *parent = 0 );

private slots:
    void contextMenuFriendList(QPoint);
    void friendDetails();
    void payTo();

private:
    Ui::ZeroReserveDialog ui;
};

#endif // ZERORESERVEDIALOG_H
