/*
 * ===========================================================================================================
 * Settings.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include "PPIError.h"
#include <string>
#include <list>
#include <set>
#include <loki/Singleton.h>

#define SETTINGS com::getSettings()
#define HOSTINFO SETTINGS.getHostInformation()

namespace tests {
	class PluginCollectionTest;
}

namespace com{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Settings;
//------------------------------------------------------------------------------------------------------------
extern Settings & getSettings();
//------------------------------------------------------------------------------------------------------------
extern Settings & initSettings(const std::string &homeDirectory);
//============================================================================================================
//Enthaelt allgeimeine Einstellungs Variablen 
//( Singleton )
//============================================================================================================
class Settings {
friend class tests::PluginCollectionTest;
friend struct Loki::CreateUsingNew<Settings>;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef string Pathname;
	//--------------------------------------------------------------------------------------------------------
	typedef set<Pathname> PathnameSet;
private:
	//--------------------------------------------------------------------------------------------------------
	std::string homeDir;
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
	const std::string & getHomeDirectory() const {
		return homeDir;
	}
	//--------------------------------------------------------------------------------------------------------
	void setHomeDirectory(const std::string &path);
	//--------------------------------------------------------------------------------------------------------
	void init(const std::string &homeDirectory);
	//--------------------------------------------------------------------------------------------------------
	void reloadConfigFile() { loadConfigFile(); }
	//--------------------------------------------------------------------------------------------------------
	// PPI KONSTANTEN
	enum { UNIQUE_ID = 'forx' };
	enum { UNIQUE_ID_INSTR = 'frxi' };
	enum { COMMAND_WORKER_INTERVAL_MS = 1000 };
	enum { PROGRAM_PARAMETER = 512 };
	static const int PPI_VERSION = /*000*/ 4444; // xx.xx.xxx
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
	string getPlugCollectionDumpFilename ()  { return getHomeDirectory() + plugCollectionDumpFile; }
	//--------------------------------------------------------------------------------------------------------
	static string getLogFilename()  { return SETTINGS.getHomeDirectory() + NAME + ".log"; }
	//--------------------------------------------------------------------------------------------------------
	static string getConfFilename()  { return SETTINGS.getHomeDirectory() + CONFIG_FILE; }
	//--------------------------------------------------------------------------------------------------------
	static string getPlugInitLogFilename()  { return SETTINGS.getHomeDirectory() + PLUG_LOAD_LOGFILE; }
	//--------------------------------------------------------------------------------------------------------
	bool addVSTFolder ( const string &path );
	//--------------------------------------------------------------------------------------------------------
	bool removeVSTFolder ( const string &path );
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
	unsigned int getWindowWidth() const { return windowWidth; }
	//--------------------------------------------------------------------------------------------------------
	unsigned int getWindowHeight() const { return windowHeight; }
	//--------------------------------------------------------------------------------------------------------
	const PathnameSet & getPluginDirectoryList() const { return pluginDirectories; }
	//--------------------------------------------------------------------------------------------------------
	static string versionToString( const unsigned int version = PPI_VERSION );
	//--------------------------------------------------------------------------------------------------------
	virtual ~Settings(){}
};
} // namespace com

#endif


