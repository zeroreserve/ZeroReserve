/*!
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

#ifndef DBCONFIG_H
#define DBCONFIG_H

#include "ui_ZeroReserveConfig.h"

#include "retroshare-gui/configpage.h"


/**
 * @class DBConfig
 */
class DBConfig : public ConfigPage
{
    Q_OBJECT
public:
    DBConfig( QWidget * parent = 0, Qt::WFlags flags = 0 );

    virtual bool save(QString &errmsg);
    virtual void load();

    virtual QPixmap iconPixmap() const { return QPixmap(":/images/bitcoin.png") ; }
    virtual QString pageName() const { return "Zero Reserve"; }

private slots:
    void editTxLog();

private:
    Ui::DBConfig ui;
};

#endif // DBCONFIG_H

