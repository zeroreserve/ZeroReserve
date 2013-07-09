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
