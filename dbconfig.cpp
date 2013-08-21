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

#include "dbconfig.h"
#include "zrdb.h"

#include "QFileDialog"
#include "QFileInfo"


DBConfig::DBConfig( QWidget *parent, Qt::WFlags flags ) :
    ConfigPage(parent, flags)
{
    ui.setupUi( this );
    connect( ui.TxLogPathButton, SIGNAL(clicked()), this, SLOT(editTxLog()) );
}


bool DBConfig::save(QString &errmsg)
{
    return true;
}

void DBConfig::load()
{

}

void DBConfig::editTxLog()
{
    QString txLogPath = QString::fromStdString( ZrDB::Instance()->getConfig( ZrDB::TXLOGPATH ) );
    QFileInfo fileInfo( txLogPath );
    QString newPath   = QFileDialog::getSaveFileName( 0, "Set the Transaction Log", fileInfo.absoluteDir().absolutePath(), "Transaction Log (*.tx)" );
    if( !newPath.isEmpty() )
        ZrDB::Instance()->updateConfig( ZrDB::TXLOGPATH, newPath.toStdString() );
}
