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
#include <boost/shared_ptr.hpp>

#define SETTINGS com::getSettings()
#define HOSTINFO SETTINGS.getHostInformation()

namespace tests {
	class PluginCollectionTest;
}

extern bool globFrxIsDemo();
/**
 * restrictions check:
 */
extern void globAddProcessor( boost::shared_ptr<void> obj );
extern void globAddPlugin( boost::shared_ptr<void> obj );
extern string globVersionStr();

namespace com {
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
    struct PathComparator {
        bool operator() (const Pathname& lhs, const Pathname& rhs) const;
    };
	//--------------------------------------------------------------------------------------------------------
	typedef set<Pathname, PathComparator> PathnameSet;
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
	int windowWidth; 
	//--------------------------------------------------------------------------------------------------------
	int windowHeight;
	//--------------------------------------------------------------------------------------------------------
	int maxLogSize;
	//--------------------------------------------------------------------------------------------------------
	void loadConfigFile();
	//--------------------------------------------------------------------------------------------------------
	void setWindowWidth ( int w ) { 
		if ( w < (int)MIN_WINDOW_WIDTH ) return;
		windowWidth = w; 
	}
	//--------------------------------------------------------------------------------------------------------
	void setWindowHeight ( int h ) { 
		if ( h < (int)MIN_WINDOW_HEIGHT ) return;
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
	enum { PROGRAM_PARAMETER = 512 };
	static const unsigned int MIN_WINDOW_WIDTH = 800;
	static const unsigned int MIN_WINDOW_HEIGHT = 600;
	static const unsigned int MAX_STR_MENU_LABEL = 40;
	static const string NAME; 
	static const string VENDOR;
	static const string SCAN_REPORT_FILENAME;
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
	/**
     * @deprecated use addPluginFolder() instead.
     */
    bool addVSTFolder ( const string &path );
	//--------------------------------------------------------------------------------------------------------
	bool addPluginFolder ( const string &path );
	//--------------------------------------------------------------------------------------------------------
	/**
     * @deprecated use removePluginFolder() instead.
     */
    bool removeVSTFolder ( const string &path );
	//--------------------------------------------------------------------------------------------------------
	bool removePluginFolder ( const string &path );
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
	const string & versionToString() const;
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


