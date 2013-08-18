#ifndef DBCONFIG_H
#define DBCONFIG_H


#include "retroshare-gui/configpage.h"


class DBConfig : public ConfigPage
{
public:
    DBConfig();

    virtual bool save(QString &errmsg);
    virtual void load();

    virtual QPixmap iconPixmap() const { return QPixmap(":/images/bitcoin.png") ; }
    virtual QString pageName() const { return "Zero Reserve"; }
};

#endif // DBCONFIG_H
