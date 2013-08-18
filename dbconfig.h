#ifndef DBCONFIG_H
#define DBCONFIG_H

#include "ui_ZeroReserveConfig.h"

#include "retroshare-gui/configpage.h"


class DBConfig : public ConfigPage
{
    Q_OBJECT
public:
    DBConfig( QWidget * parent = 0, Qt::WFlags flags = 0 );

    virtual bool save(QString &errmsg);
    virtual void load();

    virtual QPixmap iconPixmap() const { return QPixmap(":/images/bitcoin.png") ; }
    virtual QString pageName() const { return "Zero Reserve"; }

private:
    Ui::DBConfig ui;
};

#endif // DBCONFIG_H
