/*!
 * \file ZeroReservePlugin.cpp
 * 
 * 
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

#include <retroshare/rsplugin.h>
#include <util/rsversion.h>
#include <QTranslator>
#include <QMessageBox>


/**
 * @brief 
 */
ZeroReservePlugin * g_ZeroReservePlugin;

/**
 * @brief Widge creation mutex
 *
 * @param "widget_creation_mutex"
 *
 * @return 
 */
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
	uint32_t RETROSHARE_PLUGIN_revision = SVN_REVISION_NUMBER ;

	// This symbol contains the svn revision number grabbed from the executable. 
	// It will be tested by RS to load the plugin automatically, since it is safe to load plugins
	// with same revision numbers, assuming that the revision numbers are up-to-date.
	//
	uint32_t RETROSHARE_PLUGIN_api = RS_PLUGIN_API_VERSION ;
}

#define IMAGE_LINKS ":/images/bitcoin.png"

/**
 * @brief Get plugin version number
 *
 * @param major
 * @param minor
 * @param svn_rev
 */
void ZeroReservePlugin::getPluginVersion(int& major,int& minor,int& svn_rev) const
{
        major = 5 ;
	minor = 4 ;
	svn_rev = SVN_REVISION_NUMBER ;
}

/**
 * @brief Zero Reserve plugin
 * 
 */
ZeroReservePlugin::ZeroReservePlugin()
{
	mainpage = NULL ;
    mIcon = NULL ;
    mPlugInHandler = NULL;
    m_ZeroReserve = NULL;
    m_peers = NULL;

    m_asks = new OrderBook();
    m_bids = new OrderBook();
}

/**
 * @brief Set up the interfaces
 *
 * @param interfaces
 */
void ZeroReservePlugin::setInterfaces(RsPlugInInterfaces &interfaces)
{
    m_peers = interfaces.mPeers;
}

/**
 * @brief Creates the plugin dialog page within Rs
 *
 * @return 
 */
MainPage *ZeroReservePlugin::qt_page() const
{
    RsStackMutex widgetCreationMutex( widget_creation_mutex );
    if(mainpage == NULL)
    {
        mainpage = new ZeroReserveDialog( m_bids, m_asks );
    }

    return mainpage ;
}


/**
 * @brief Set plugin handler
 *
 * @param pgHandler
 */
void ZeroReservePlugin::setPlugInHandler(RsPluginHandler *pgHandler)
{
    mPlugInHandler = pgHandler;
}

/**
 * @brief Set the plugin icon
 *
 * @return 
 */
QIcon *ZeroReservePlugin::qt_icon() const
{
    if(mIcon == NULL)
    {
        Q_INIT_RESOURCE(ZeroReserve_images) ;

        mIcon = new QIcon(IMAGE_LINKS) ;
    }

    return mIcon ;
}

/**
 * @brief Connect ZR to RS and services
 * 
 * @todo 
 *
 * @return 
 */
RsPQIService * ZeroReservePlugin::rs_pqi_service() const
{
    if(m_ZeroReserve == NULL){
        m_ZeroReserve = new p3ZeroReserveRS(mPlugInHandler, m_bids, m_asks, m_peers );
        ZR::Bitcoin::Instance()->start();
    }

    return m_ZeroReserve ;
}

/**
 * @brief Get short plugin description
 * 
 *
 * @return  a std string with the short description
 */
std::string ZeroReservePlugin::getShortPluginDescription() const
{
        return QApplication::translate("ZeroReservePlugin", "This plugin implements a distributed Bitcoin exchange.").toUtf8().constData();
}

/**
 * @brief Get name of the plugin
 *
 * @return a translated string of the plugin name.
 */
std::string ZeroReservePlugin::getPluginName() const
{
	return QApplication::translate("ZeroReservePlugin", "Zero Reserve").toUtf8().constData();
}

/**
 * @brief Initialise the qt translator code
 *
 * @param 
 * @param languageCode
 * @param externalDir
 *
 * @return 
 */
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

/**
 * @brief Stop Zero Reserve
 * Close the instance of ZR
 * 
 * @todo Stop taking transactions, try to finish all the ongoing transactions, and send out errors to all
 * transactions controllers who aren't done within 30 seconds. And display a progress bar.
 * 
 */
void ZeroReservePlugin::stop()
{
    std::cerr << "Zero Reserve: Closing Database" << std::endl;
    // TODO: Stop taking TX, try to finish all ongoing TX and send out errors to all
    // TODO: TX controllers who aren't done within 30 seconds.
    // TODO: Display a progress bar.
    ZrDB::Instance()->close();

    ZR::Bitcoin::Instance()->stop();
}


//   EOF   
