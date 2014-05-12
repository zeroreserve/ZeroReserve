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


#include "ZeroReservePlugin.h"
#include "ZeroReserveDialog.h"
#include "OrderBook.h"
#include "p3ZeroReserverRS.h"
#include "zrdb.h"
#include "dbconfig.h"
#include "ZRBitcoin.h"
#include "util/rsversion.h"

#include <retroshare/rsplugin.h>
#include <util/rsversion.h>
#include <QTranslator>
#include <QMessageBox>


ZeroReservePlugin * g_ZeroReservePlugin;
RsMutex ZeroReservePlugin::widget_creation_mutex("widget_creation_mutex");


extern "C" {
	void *RETROSHARE_PLUGIN_provide()
	{
        g_ZeroReservePlugin = new ZeroReservePlugin() ;
        return (void*) g_ZeroReservePlugin;
	}
	// This symbol contains the svn revision number grabbed from the executable. 
	// It will be tested by RS to load the plugin automatically, since it is safe to load plugins
	// with same revision numbers, assuming that the revision numbers are up-to-date.
	//
    uint32_t RETROSHARE_PLUGIN_revision = SVN_REVISION_NUMBER;

	// This symbol contains the svn revision number grabbed from the executable. 
	// It will be tested by RS to load the plugin automatically, since it is safe to load plugins
	// with same revision numbers, assuming that the revision numbers are up-to-date.
	//
	uint32_t RETROSHARE_PLUGIN_api = RS_PLUGIN_API_VERSION ;
}

#define IMAGE_LINKS ":/images/bitcoin.png"

void ZeroReservePlugin::getPluginVersion(int& major,int& minor,int& svn_rev) const
{
    major = 5 ;
    minor = 5 ;
    svn_rev = SVN_REVISION_NUMBER;
}

ZeroReservePlugin::ZeroReservePlugin() :
    mainpage( NULL ),
    m_messages_mutex ( "messages_mutex" ),
    m_stopped( false )
{
	mainpage = NULL ;
        mIcon = NULL ;
        mPlugInHandler = NULL;
        m_ZeroReserve = NULL;
        m_peers = NULL;

        m_asks = new OrderBook();
        m_bids = new OrderBook();
}

void ZeroReservePlugin::setInterfaces(RsPlugInInterfaces &interfaces)
{
    m_peers = interfaces.mPeers;
}

MainPage *ZeroReservePlugin::qt_page() const
{
    RsStackMutex widgetCreationMutex( widget_creation_mutex );
    if(mainpage == NULL){
        mainpage = new ZeroReserveDialog( m_bids, m_asks );
    }

    return mainpage ;
}


void ZeroReservePlugin::setPlugInHandler(RsPluginHandler *pgHandler)
{
    mPlugInHandler = pgHandler;
}

QIcon *ZeroReservePlugin::qt_icon() const
{
    if(mIcon == NULL)
    {
        Q_INIT_RESOURCE(ZeroReserve_images) ;

        mIcon = new QIcon(IMAGE_LINKS) ;
    }

    return mIcon ;
}

RsPQIService * ZeroReservePlugin::rs_pqi_service() const
{
    if(m_ZeroReserve == NULL){
        m_ZeroReserve = new p3ZeroReserveRS(mPlugInHandler, m_bids, m_asks, m_peers );
        ZR::Bitcoin::Instance()->start();
    }

    return m_ZeroReserve ;
}

std::string ZeroReservePlugin::getShortPluginDescription() const
{
        return QApplication::translate("ZeroReservePlugin", "This plugin implements a distributed Bitcoin exchange.").toUtf8().constData();
}

std::string ZeroReservePlugin::getPluginName() const
{
	return QApplication::translate("ZeroReservePlugin", "Zero Reserve").toUtf8().constData();
}

QTranslator* ZeroReservePlugin::qt_translator(QApplication */*app*/, const QString& languageCode, const QString& externalDir) const
{
	if (languageCode == "en") {
		return NULL;
	}

	QTranslator* translator = new QTranslator();

	if (translator->load(externalDir + "/ZeroReserve_" + languageCode + ".qm")) {
		return translator;
	} else if (translator->load(":/lang/ZeroReserve_" + languageCode + ".qm")) {
		return translator;
	}

	delete(translator);
	return NULL;
}

ConfigPage * ZeroReservePlugin::qt_config_page() const
{
    return new DBConfig() ;
}

QDialog * ZeroReservePlugin::qt_about_page() const
{
        static QMessageBox *about_dialog = NULL ;

        if(about_dialog == NULL)
        {
                about_dialog = new QMessageBox() ;

                QString text ;
                text += QObject::tr("<h3>RetroShare Zero Reserve plugin</h3><br/>   * Author: R&uuml;diger Koch<br/>" ) ;
                text += QObject::tr("<br/>Zero Reserve implements a distributed Bitcoin Exchange based on the Ripple idea.") ;
                text += QObject::tr("Your friend needs to run the plugin to trade with you, of course.") ;
                text += QObject::tr("<br/><br/>This is experimental software. Use at your own risk. Don't hesitate to send comments and suggestion to anu at zerostate dot net.") ;

                about_dialog->setText(text) ;
                about_dialog->setStandardButtons(QMessageBox::Ok) ;
        }

        return about_dialog ;
}

void ZeroReservePlugin::stop()
{
    if( m_stopped )return;

    std::cerr << "Zero Reserve: Closing Database" << std::endl;
    m_stopped = true;
    ZrDB::Instance()->close();
    ZR::Bitcoin::Instance()->stop();
}


void ZeroReservePlugin::placeMsg( const std::string & _msg )
{
    QString msg = QString::fromStdString( _msg );
    RsStackMutex messagesMutex( m_messages_mutex );
    m_messages.append( msg );
}


void ZeroReservePlugin::displayMsg()
{
    RsStackMutex messagesMutex( m_messages_mutex );
    for( QList< QString >::Iterator it = m_messages.begin(); it != m_messages.end(); ){
        QString msg = *it;
        QMessageBox::warning( mainpage, "Zero Reserve", msg);
        it = m_messages.erase( it );
    }
}
