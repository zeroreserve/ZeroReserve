#include <retroshare/rsplugin.h>
#include <util/rsversion.h>
#include <QTranslator>

#include "ZeroReservePlugin.h"
#include "ZeroReserveDialog.h"

static void *inited = new ZeroReservePlugin() ;

extern "C" {
	void *RETROSHARE_PLUGIN_provide()
	{
		static ZeroReservePlugin *p = new ZeroReservePlugin() ;

		return (void*)p ;
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

void ZeroReservePlugin::getPluginVersion(int& major,int& minor,int& svn_rev) const
{
	major = 5 ;
	minor = 4 ;
	svn_rev = SVN_REVISION_NUMBER ;
}

ZeroReservePlugin::ZeroReservePlugin()
{
	mainpage = NULL ;
        mIcon = NULL ;
        mPlugInHandler = NULL;
        mPeers = NULL;
        mFiles = NULL;
}

void ZeroReservePlugin::setInterfaces(RsPlugInInterfaces &interfaces){

    mPeers = interfaces.mPeers;
    mFiles = interfaces.mFiles;
}

MainPage *ZeroReservePlugin::qt_page() const
{
	if(mainpage == NULL)
                mainpage = new ZeroReserveDialog() ;

	return mainpage ;
}


void ZeroReservePlugin::setPlugInHandler(RsPluginHandler *pgHandler){
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
