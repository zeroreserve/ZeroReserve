/*
    This file is part of the Zero Reserve Plugin for Retroshare.

    Foobar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FRIENDDETAILSDIALOG_H
#define FRIENDDETAILSDIALOG_H

#include <QDialog>
#include <string>

namespace Ui {
    class FriendDetailsDialog;
}

class FriendDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FriendDetailsDialog(QWidget *parent = 0, const std::string & uid = "Error. No id selected!" );
    ~FriendDetailsDialog();

private:
    Ui::FriendDetailsDialog *ui;
};

#endif // FRIENDDETAILSDIALOG_H
