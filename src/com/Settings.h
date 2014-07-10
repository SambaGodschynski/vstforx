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
extern std::string globVersionStr();

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
	typedef std::string Pathname;
    //--------------------------------------------------------------------------------------------------------
    struct PathComparator {
        bool operator() (const Pathname& lhs, const Pathname& rhs) const;
    };
	//--------------------------------------------------------------------------------------------------------
	typedef std::set<Pathname, PathComparator> PathnameSet;
private:
	//--------------------------------------------------------------------------------------------------------
	std::string homeDir;
	//--------------------------------------------------------------------------------------------------------
	// privater Konstruktor
	Settings();
	//--------------------------------------------------------------------------------------------------------
	PathnameSet pluginDirectories;
	//--------------------------------------------------------------------------------------------------------
	std::string plugCollectionDumpFile;
	//--------------------------------------------------------------------------------------------------------
	std::string plugDir;
	//--------------------------------------------------------------------------------------------------------
	std::string outDir;
    //--------------------------------------------------------------------------------------------------------
    std::string style;
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
	static const std::string NAME; 
	static const std::string VENDOR;
	static const std::string SCAN_REPORT_FILENAME;
	// dateinamen
	static const std::string PLUG_LOAD_LOGFILE; 
	static const std::string CONFIG_FILE;
	//--------------------------------------------------------------------------------------------------------
	std::string getPlugCollectionDumpFilename();
	//--------------------------------------------------------------------------------------------------------
	std::string getLogFilename() const;
	//--------------------------------------------------------------------------------------------------------
	std::string getConfFilename() const;
	//--------------------------------------------------------------------------------------------------------
	std::string getInitScriptFilename() const;
	//--------------------------------------------------------------------------------------------------------
	std::string getPlugInitLogFilename() const;
	//--------------------------------------------------------------------------------------------------------
	/**
     * @deprecated use addPluginFolder() instead.
     */
    bool addVSTFolder ( const std::string &path );
	//--------------------------------------------------------------------------------------------------------
	bool addPluginFolder ( const std::string &path );
	//--------------------------------------------------------------------------------------------------------
	/**
     * @deprecated use removePluginFolder() instead.
     */
    bool removeVSTFolder ( const std::string &path );
	//--------------------------------------------------------------------------------------------------------
	bool removePluginFolder ( const std::string &path );
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
	const std::string & versionToString() const;
    //--------------------------------------------------------------------------------------------------------
    std::string getStyle() const;
    //--------------------------------------------------------------------------------------------------------
    std::string getStylePath() const;
    //--------------------------------------------------------------------------------------------------------
    std::string getStyleRootPath() const;
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


