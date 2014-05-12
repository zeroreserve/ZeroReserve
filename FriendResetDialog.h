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

#ifndef FRIENDRESETDIALOG_H
#define FRIENDRESETDIALOG_H

#include <QDialog>

#include <string>

namespace Ui {
class FriendResetDialog;
}

class FriendResetDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FriendResetDialog( const std::string &uid, QWidget *parent = 0, const std::string &name = "Error. No id selected!" );
    ~FriendResetDialog();
    
private slots:
    void resetFriend();

private:
    Ui::FriendResetDialog *ui;
    std::string m_id;
};

#endif // FRIENDRESETDIALOG_H
