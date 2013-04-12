/*
 * ===========================================================================================================
 * Settings.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "Settings.h"
#include "MyString.h"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/algorithm/string.hpp> 
#include "boost/filesystem.hpp"
#include "com/One4All.h"
#include "OS_Specific/OS_com.h"
#include "com/PPIError.h"
#include <sambag/disco/FileResourceManager.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>

static const string SEPARATOR = "=";
static const string IN_DIR = "in_dir";
static const string WINDOW_WIDTH = "window_width";
static const string WINDOW_HEIGHT = "window_height";
static const string MAX_LOGSIZE = "max_logfile_sizeKB";
static const string SKIP_SCAN = "fast_scan";

//------------------------------------------------------------------------------------------------------------
static inline void parseConfigLine( ifstream &f, com::MyString &token, com::MyString &content ) {
	com::MyString str;
	getline ( f, str );
	if ( str.length() == 0 ) return;
	str = str.trim();
	if ( str.at(0) == '#' ) return;
	size_t p = str.find_first_of (SEPARATOR);
	if ( p == string::npos ) {
		content = str;
		return;
	}
	token = str.substr ( 0, p );
	content = str.substr ( p+1 );
	token = token.trim();
	content = content.trim();
}

namespace com {
//============================================================================================================
//class Settings
//============================================================================================================
namespace {
	typedef Loki::SingletonHolder<Settings> FactoryHolder;
} // namespace
//------------------------------------------------------------------------------------------------------------
Settings & getSettings() {
	Settings &res = FactoryHolder::Instance();
	return res;
}
//------------------------------------------------------------------------------------------------------------
Settings & initSettings(const std::string &homeDirectory) {
	Settings &res = FactoryHolder::Instance();
	res.init(homeDirectory);
	sambag::disco::FileResourceManager::init(homeDirectory);
	return res;
}
//------------------------------------------------------------------------------------------------------------
const string Settings::NAME = "vstforx";
const string Settings::VENDOR = "samba godschynski";
const string Settings::PLUG_LOAD_LOGFILE = "init_plug.log";
const string Settings::CONFIG_FILE = NAME + ".conf" ;
const string Settings::FX_BUNDLE_ID = "com.samba_godschynski.VSTForx";
const string Settings::I_BUNDLE_ID = "com.samba_godschynski.iVSTForx";
const string Settings::SCAN_REPORT_FILENAME = "scanReport.txt";
static size_t KILO = 1000;
//------------------------------------------------------------------------------------------------------------
Settings::Settings() : 
windowWidth(MIN_WINDOW_WIDTH), 
windowHeight(MIN_WINDOW_HEIGHT),
maxLogSize( KILO * 2000 ),
fastScan ( true )
{
}
//------------------------------------------------------------------------------------------------------------
void Settings::setHomeDirectory(const std::string &path) {
	homeDir = path;
}
//------------------------------------------------------------------------------------------------------------
string Settings::getPlugCollectionDumpFilename ()  {
	std::string str = getHomeDirectory() + "/" + plugCollectionDumpFile; 
	boost::filesystem::path p(str);
	return absolute(p).string();
}
//------------------------------------------------------------------------------------------------------------
string Settings::getLogFilename() const { 
	std::string str = SETTINGS.getHomeDirectory() + "/" + NAME + ".log";
	boost::filesystem::path p(str);
	return absolute(p).string();
}
//------------------------------------------------------------------------------------------------------------
string Settings::getConfFilename() const { 
	std::string str = SETTINGS.getHomeDirectory() + "/" + CONFIG_FILE; 
	boost::filesystem::path p(str);
	return absolute(p).string();
}
//------------------------------------------------------------------------------------------------------------
string Settings::getPlugInitLogFilename() const { 
	std::string str = SETTINGS.getHomeDirectory() + "/" + PLUG_LOAD_LOGFILE;
	boost::filesystem::path p(str);
	return absolute(p).string();
}
//------------------------------------------------------------------------------------------------------------
void Settings::init(const std::string &homeDirectory) {
	using namespace boost::algorithm;
	setHomeDirectory(homeDirectory);
	outDir = getHomeDirectory();
	loadConfigFile();
	string name = NAME; 
	to_lower(name);
	plugCollectionDumpFile = name + "_plugin_db_dump";
}
//------------------------------------------------------------------------------------------------------------
bool Settings::addVSTFolder ( const string &path ) {
	
	// testen ob path == unterverz. von schon vorhandenen pfad
	PathnameSet::iterator it = pluginDirectories.begin();
	for ( ; it!=pluginDirectories.end(); ++it ) {
		if ( isSubDirectory( sambag::com::Location(*it), sambag::com::Location(path) ) ) {
			throw ppiError::SettingsException ( 
				"given folder is subfolder of " + *it,
				__FILE__,
				__LINE__
			);
		}
		if ( isSubDirectory( sambag::com::Location(path), sambag::com::Location(*it) ) ) {
			throw ppiError::SettingsException ( 
				"given folder is parent folder of " + *it,
				__FILE__,
				__LINE__
			);
		}
	}

	return pluginDirectories.insert(path).second;
}
//------------------------------------------------------------------------------------------------------------
bool Settings::removeVSTFolder ( const string &path ) {
	return pluginDirectories.erase(path) > 0;
}
//------------------------------------------------------------------------------------------------------------
void Settings::loadConfigFile() { // TODO: use boost::Program_options
	// !! keine PPI ausnahmen oder TOLOG oder irgendetwas was indirekt wieder settings init. !!
	ifstream f;
	if ( CONFIG_FILE == "" ) return;
	string conFile = getConfFilename();
	f.open ( conFile.c_str(), ios::in );
	// wenn zugriff verw. aber datei existent
	if ( f.fail() )  {
		if (  boost::filesystem::exists(conFile) ) {
			SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
				string("access to [" + conFile + "] failed. (check protection)" )
			);
		}
		return;
	}
	while ( !f.eof() ) {
		MyString cont, token;
		parseConfigLine (f, token, cont);
		if ( token == IN_DIR ) {
			if ( cont.length() > 0 ) {
				try {
					addVSTFolder(cont); // throws SettingsException if folder==already given subfolder
				} catch ( ppiError::SettingsException &ex ) {continue;}
			}
		}
		if ( token == WINDOW_WIDTH ) {
			if ( cont.length() == 0 ) continue;
			windowWidth = atoi( cont.c_str() );
			if ( windowWidth == 0 || windowWidth == INT_MAX || windowWidth == INT_MIN ) {
				windowWidth = MIN_WINDOW_WIDTH;
			}
		}
		if ( token == WINDOW_HEIGHT ) {
			if ( cont.length() == 0 ) continue;
			windowHeight = atoi( cont.c_str() );
			if ( windowHeight == 0 || windowHeight == INT_MAX || windowHeight == INT_MIN ) {
				windowHeight = MIN_WINDOW_HEIGHT;
			}
		}
		if ( token == MAX_LOGSIZE ) {
			if ( cont.length() == 0 ) continue;
			maxLogSize = (unsigned int)atoi( cont.c_str() ) * KILO;
		}
		if ( token == SKIP_SCAN ) {
			if ( cont.length() == 0 ) continue;
			if ( atoi( cont.c_str() ) == 1 ) fastScan = true;
				else fastScan = false;
		}
	}
	f.close();
}
//------------------------------------------------------------------------------------------------------------
void Settings::saveConfigFile() {  // TODO: use boost::Program_options
	ofstream f;
	if ( CONFIG_FILE == "" ) return;
	f.open ( getConfFilename().c_str(), ios::trunc );
	if ( f.fail() ) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			string("writing to [" + getConfFilename() + "] failed. (check protection)" )
		);
	}
	
	// indirs
	PathnameSet::const_iterator it = pluginDirectories.begin();
	for ( ; it!=pluginDirectories.end(); ++it ) {
		f<<IN_DIR<<"="<<*it<<endl;
	}
	// window metrics
	f<<WINDOW_WIDTH<<"="<<getWindowWidth()<<endl;
	f<<WINDOW_HEIGHT<<"="<<getWindowHeight()<<endl;
	// logfile
	f<<MAX_LOGSIZE<<"="<<( getMaxLogSize() / KILO )<<endl;
	// fastScan
	f<<SKIP_SCAN<<"="<<isFastScan();
	f.close();
}
//--------------------------------------------------------------------------------------------------------
void Settings::setIsDemo(bool val) {
	_isDemo = val;
}
//--------------------------------------------------------------------------------------------------------
string Settings::versionToString( const unsigned int version ) const {
	return std::string("0.9.0") + (isDemo() ? " DEMO VERSION" : "");
}
//--------------------------------------------------------------------------------------------------------
bool Settings::getBooleanValue(const std::string &key) const {
	if (key == "fastScan") {
		return fastScan;
	}
	SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
		"Key: " + key + " not found.");
	return false; 
}
//--------------------------------------------------------------------------------------------------------
void Settings::setBooleanValue(const std::string &key, bool val) {
	if (key == "fastScan") {
		fastScan = val;
		return;
	}
	SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
		"Key: " + key + " not found.");
}
//--------------------------------------------------------------------------------------------------------
std::string Settings::getStringValue(const std::string &key) const {
	if (key=="version") {
		return versionToString();
	}
	SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
		"Key: " + key + " not found.");
	return "";
}
//--------------------------------------------------------------------------------------------------------
void Settings::setStringValue(const std::string &key, const std::string &val) {
	SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
		"Key: " + key + " not found.");
}
//--------------------------------------------------------------------------------------------------------
int Settings::getIntegerValue(const std::string &key) const {
	if (key == "editorWidth") {
		return windowWidth;
	}
	if (key == "editorHeight") {
		return windowHeight;
	}
	SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
		"Key: " + key + " not found.");
	return 0;
}
//--------------------------------------------------------------------------------------------------------
void Settings::setIntegerValue(const std::string &key, int val) {
	if (key == "editorWidth") {
		windowWidth = val;
		return;
	}
	if (key == "editorHeight") {
		windowHeight = val;
		return;
	}
	SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
		"Key: " + key + " not found.");
}
}// namespace com


