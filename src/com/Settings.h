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
#include <sambag/com/ArithmeticWrapper.hpp>

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
	sambag::com::ArithmeticWrapper<bool> _isDemo;
	//--------------------------------------------------------------------------------------------------------
public:
	//--------------------------------------------------------------------------------------------------------
	bool isDemo() const { return _isDemo; }
	//--------------------------------------------------------------------------------------------------------
	void setIsDemo(bool val);
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
	string getPlugCollectionDumpFilename();
	//--------------------------------------------------------------------------------------------------------
	string getLogFilename() const;
	//--------------------------------------------------------------------------------------------------------
	string getConfFilename() const;
	//--------------------------------------------------------------------------------------------------------
	string getPlugInitLogFilename() const;
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
	string versionToString( const unsigned int version = PPI_VERSION ) const;
	//--------------------------------------------------------------------------------------------------------
	virtual ~Settings(){}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @throw IllegalStateException if key not found
	 */ 
	bool getBooleanValue(const std::string &key) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @throw IllegalStateException if key not found
	 */ 
	void setBooleanValue(const std::string &key, bool val);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @throw IllegalStateException if not found
	 */ 
	std::string getStringValue(const std::string &key) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @throw IllegalStateException if key not found
	 */ 
	void setStringValue(const std::string &key, const std::string &val);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @throw IllegalStateException if not found
	 */ 
	int getIntegerValue(const std::string &key) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @throw IllegalStateException if key not found
	 */ 
	void setIntegerValue(const std::string &key, int val);
};
} // namespace com

#endif


