#ifndef SETTINGS_H
#define SETTINGS_H

#include "PPIError.h"
#include "boost/shared_ptr.hpp"
#include <string>
#include <list>
#include <set>
#define SETTINGS com::Settings::getSettings()
#define HOSTINFO SETTINGS->getHostInformation()

std::string getHomeDirectory();

class CConfigToolDlg;

// TODO: forward decl. ist scheiss lösung 

namespace ppiGui{ 
	class SetupDialog; 
	class MainCtrl;
}

namespace tests {
	class PluginCollectionTest;
}

namespace com{
//============================================================================================================
//Enthaelt allgeimeine Einstellungs Variablen 
//( Singleton )
//============================================================================================================
class Settings {
friend class CConfigToolDlg;
friend class ppiGui::SetupDialog;
friend class ppiGui::MainCtrl;
friend class tests::PluginCollectionTest;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Settings> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef string Pathname;
	//--------------------------------------------------------------------------------------------------------
	typedef set<Pathname> PathnameSet;
private:
	//--------------------------------------------------------------------------------------------------------
	// Singleton Settings Instanz
	static Ptr settings; 
	//--------------------------------------------------------------------------------------------------------
	// privater Konstruktor
	Settings();
	//--------------------------------------------------------------------------------------------------------
	PathnameSet pluginDirectories;
	//--------------------------------------------------------------------------------------------------------
	string plugCollectionDumpFile;
	//--------------------------------------------------------------------------------------------------------
	string plugDir;
	//--------------------------------------------------------------------------------------------------------
	string outDir;
	//--------------------------------------------------------------------------------------------------------
	bool fastScan; 
	//--------------------------------------------------------------------------------------------------------
	unsigned int windowWidth; 
	//--------------------------------------------------------------------------------------------------------
	unsigned int windowHeight;
	//--------------------------------------------------------------------------------------------------------
	unsigned int maxLogSize;
	//--------------------------------------------------------------------------------------------------------
	void loadConfigFile();
	//--------------------------------------------------------------------------------------------------------
	void setWindowWidth ( int w ) { 
		if ( w < MIN_WINDOW_WIDTH ) return;
		windowWidth = w; 
	}
	//--------------------------------------------------------------------------------------------------------
	void setWindowHeight ( int h ) { 
		if ( h < MIN_WINDOW_HEIGHT ) return;
		windowHeight = h; 
	}
	//--------------------------------------------------------------------------------------------------------
public:
	//--------------------------------------------------------------------------------------------------------
	void reloadConfigFile() { loadConfigFile(); }
	//--------------------------------------------------------------------------------------------------------
	// PPI KONSTANTEN
	static const int UNIQUE_ID = 'forx';
	static const int UNIQUE_ID_INSTR = 'frxi';
	static const int GRIDSIZE = 15;
	static const int COMMAND_WORKER_INTERVAL_MS = 1000;
	static const int STD_KNOB_SUBPIXMAPS = 100;
	static const int PROGRAM_PARAMETER = 512;
	static const int PPI_VERSION = /*000*/ 4300; // xx.xx.xxx
	static const char PPI_VERSION_EXTRA_CHAR = 'd';
	static const unsigned int MIN_WINDOW_WIDTH = 800;
	static const unsigned int MIN_WINDOW_HEIGHT = 600;
	static const unsigned int MAX_STR_MENU_LABEL = 40;
	static const string NAME; 
	static const string VENDOR;
	static const string SCAN_REPORT_FILENAME;
	static const string FX_BUNDLE_ID; // osx bundleID effect
	static const string I_BUNDLE_ID;  // osx bundleID Instrument
	// dateinamen
	static const string PLUG_LOAD_LOGFILE; 
	static const string CONFIG_FILE;
	//--------------------------------------------------------------------------------------------------------
	static bool isInit() { return settings.get() == NULL; }
	//--------------------------------------------------------------------------------------------------------
	string getPlugCollectionDumpFilename ()  { return getHomeDirectory() + plugCollectionDumpFile; }
	//--------------------------------------------------------------------------------------------------------
	static string getLogFilename()  { return getHomeDirectory() + NAME + ".log"; }
	//--------------------------------------------------------------------------------------------------------
	static string getConfFilename()  { return getHomeDirectory() + CONFIG_FILE; }
	//--------------------------------------------------------------------------------------------------------
	static string getPlugInitLogFilename()  { return getHomeDirectory() + PLUG_LOAD_LOGFILE; }
	//--------------------------------------------------------------------------------------------------------
	void addVSTFolder ( const string &path );
	//--------------------------------------------------------------------------------------------------------
	void removeVSTFolder ( const string &path ) {
		pluginDirectories.erase ( path );
	}
	//--------------------------------------------------------------------------------------------------------
	void clearVSTFolders () {
		pluginDirectories.clear();
	}
	//--------------------------------------------------------------------------------------------------------
	bool isFastScan() { return fastScan; }
	//--------------------------------------------------------------------------------------------------------
	unsigned int getMaxLogSize() { return maxLogSize; }
	//--------------------------------------------------------------------------------------------------------
	void saveConfigFile ();
	//--------------------------------------------------------------------------------------------------------
	static Ptr getSettings();
	//--------------------------------------------------------------------------------------------------------
	unsigned int getWindowWidth() const { return windowWidth; }
	//--------------------------------------------------------------------------------------------------------
	unsigned int getWindowHeight() const { return windowHeight; }
	//--------------------------------------------------------------------------------------------------------
	const PathnameSet & getPluginDirectoryList() const { return pluginDirectories; }
	//--------------------------------------------------------------------------------------------------------
	static string versionToString( const unsigned int version = PPI_VERSION );
	//--------------------------------------------------------------------------------------------------------
	virtual ~Settings(){}
private:
};

} // namespace com

#endif


