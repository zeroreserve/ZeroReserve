#include "dbconfig.h"

DBConfig::DBConfig( QWidget *parent, Qt::WFlags flags ) :
    ConfigPage(parent, flags)
{
    ui.setupUi( this );
}


bool DBConfig::save(QString &errmsg)
{
    return true;
}

void DBConfig::load()
{

}
