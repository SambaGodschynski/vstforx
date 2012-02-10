#ifndef PLUGIN_COLLECTION
#define PLUGIN_COLLECTION

#include "com/one4All.h"
#include "com/PPIError.h"
#include "boost/shared_ptr.hpp"
#include "settings.h"
#include "Events.h"
#include "MyString.h"
#include "processing/Graph.h"
#include "processing/IHostInfo.h"
#include <list>
#include "sambag/com/FileSystem.hpp"
#include "sambag/cpsqlite/DataBase.hpp"
#include "processing/PlugInfo.h"
#include "processing/PlugNode.h"
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"

namespace com {
using namespace events;
class PluginCollection;
//============================================================================================================
// Event: OnLoadFile
//============================================================================================================
struct OnLoadFile : public Event {
	string filename;
	OnLoadFile ( const string &filename ) : filename(filename) {}
};
//============================================================================================================
// Event: OnLoadFile
//============================================================================================================
struct OnFileLoaded : public Event {
	string filename;
	processing::PluginInfo info;
	OnFileLoaded ( const string &filename, const processing::PluginInfo &info ) : 
		info(info), filename(filename) {}
};
//============================================================================================================
// Event: OnScanInterrupted
//============================================================================================================
struct ScanInterrupted : public Event {
	string cause;
	ScanInterrupted ( const string &cause ) : cause(cause) {}
};
//============================================================================================================
// Event: CleaningUpDataBase
//============================================================================================================
struct CleaningUpDataBase : public Event {};
//============================================================================================================
// Event: ScanFinished
//============================================================================================================
struct ScanFinished : public Event {};

//============================================================================================================
class ScanVisitor : public sambag::com::IWalkerVisitor {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	struct TreeError {};
	//--------------------------------------------------------------------------------------------------------
	typedef sambag::com::Location Path;
	//--------------------------------------------------------------------------------------------------------
	typedef sambag::cpsqlite::DataBase::Int FolderID;
private:
	//--------------------------------------------------------------------------------------------------------
	void insert ( const Path &path );
	//--------------------------------------------------------------------------------------------------------
	sambag::cpsqlite::DataBase::Ptr db;
	//--------------------------------------------------------------------------------------------------------
	sambag::cpsqlite::DataBase::Executer::Ptr dbExe;
	//--------------------------------------------------------------------------------------------------------
	PluginCollection *client;
public:
	//--------------------------------------------------------------------------------------------------------
	void setStartFolder ( const Path &startFolder );
	//--------------------------------------------------------------------------------------------------------
	ScanVisitor ( PluginCollection *client );
	//--------------------------------------------------------------------------------------------------------
	virtual bool changeDirectory ( const Path & path );
	//--------------------------------------------------------------------------------------------------------
	virtual void file ( const Path & file );
};

#define GET_FOLDER_ID(folder)   boost::tuples::get<1>( (folder) )
#define GET_FOLDER_NAME(folder) boost::tuples::get<0>( (folder) )

//============================================================================================================
// Klasse PluginCollection : <Singleton>
// Datenbank fuer Plugin Files.
// Die festgelegten root verzeichnisse werden iteriert und alle vorhandenen Plugins
// gesammelt. VSTPlugin objekte sind dann ueber die VST-UID aufrufbar.
// Die angesammelten daten werden auf HD gespeichert und nur bei verzeichniss aenderung aktualisiert.
//============================================================================================================
class PluginCollection : 
	public EventSender<OnLoadFile>, 
	public EventSender<OnFileLoaded>,
	public EventSender<CleaningUpDataBase>, 
	public EventSender<ScanFinished>,
	public EventSender<ScanInterrupted>
{
friend class ScanVisitor;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef std::string PluginIdType;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<PluginCollection> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef list < processing::PluginInfo > PluginInfoList;
	//--------------------------------------------------------------------------------------------------------
	typedef ScanVisitor::FolderID FolderID;
	//--------------------------------------------------------------------------------------------------------
	typedef ScanVisitor::FolderID Int;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::tuples::tuple<std::string, FolderID> Folder;
	//--------------------------------------------------------------------------------------------------------
	typedef list<Folder> Folders;
	//--------------------------------------------------------------------------------------------------------
	typedef std::list<sambag::com::Location> PathList;
	//--------------------------------------------------------------------------------------------------------
	typedef ScanVisitor::Path Path;
	//--------------------------------------------------------------------------------------------------------
	static const FolderID NULL_FOLDER_ID;
	//--------------------------------------------------------------------------------------------------------
	static const FolderID ROOT_FOLDER_ID;
	//--------------------------------------------------------------------------------------------------------
	static const Folder NULL_FOLDER;
private:
	//********************************************************************************************************
	// DataBase
	//--------------------------------------------------------------------------------------------------------
	void initDB();
	//--------------------------------------------------------------------------------------------------------
	void updateScanStamp();
	//--------------------------------------------------------------------------------------------------------
	// loads plugin gets data and inserts into db
	processing::PluginInfo insertPlug ( const Folder &folder, const Path &location );
	//--------------------------------------------------------------------------------------------------------
	// inserts plugin data in db
	void insertPlug ( const Folder &folder, const processing::PluginInfo &info );
	//--------------------------------------------------------------------------------------------------------
	void updatePlug ( processing::PluginInfo &info );
	//--------------------------------------------------------------------------------------------------------
	void updatePlugScanStamp ( processing::PluginInfo &info );
	//--------------------------------------------------------------------------------------------------------
	void removeUnusedFolders();
	//--------------------------------------------------------------------------------------------------------
	void removeUnusedPlugins();
	//--------------------------------------------------------------------------------------------------------
	void checkDataBaseIntegrity();
	//--------------------------------------------------------------------------------------------------------
	bool searchPlugin ( processing::PluginInfo &pI ); 
	//--------------------------------------------------------------------------------------------------------
	Folder getFolder ( FolderID id ) const;
	//--------------------------------------------------------------------------------------------------------
	bool abortScan;
public:
	//--------------------------------------------------------------------------------------------------------
	Folder getFolder ( const Path &loc ) const;
	//--------------------------------------------------------------------------------------------------------
	Folder getRootFolder() const;
	//--------------------------------------------------------------------------------------------------------
	void getSubFolders( FolderID id, Folders &outFolders ) const;
	//--------------------------------------------------------------------------------------------------------
	void getPlugInfoList( const Folder &folder, PluginInfoList &out_l, bool showAll = false ) const;
	//--------------------------------------------------------------------------------------------------------
	processing::PluginInfo getPlugInfo ( const Path &location ) const; 
	//--------------------------------------------------------------------------------------------------------
	// liefert true wenn alle in settings gesetzten verzeichnisse == plugDirectories.
	bool isAllScanned() const;
	//--------------------------------------------------------------------------------------------------------
	size_t getNumSucceed() const;
	//--------------------------------------------------------------------------------------------------------
	size_t getNumFailed() const;
	//--------------------------------------------------------------------------------------------------------
	size_t getNumNotChecked() const;
	//********************************************************************************************************
private:
	//--------------------------------------------------------------------------------------------------------
	time_t scanStamp;
	//--------------------------------------------------------------------------------------------------------
	processing::IHostInfo *tmpHostInfo;
	//--------------------------------------------------------------------------------------------------------
	void scanDirectory( const ScanVisitor::Path &path, ScanVisitor &vis );
	//--------------------------------------------------------------------------------------------------------
	void scanDirectories( const Settings::PathnameSet& );
	//--------------------------------------------------------------------------------------------------------
	sambag::cpsqlite::DataBase::Ptr database;
	//--------------------------------------------------------------------------------------------------------
	com::Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	//PathNameList ;
	//--------------------------------------------------------------------------------------------------------
	Filenames blackList; // files to skip 
	//--------------------------------------------------------------------------------------------------------
	Settings::Ptr settings;
	//--------------------------------------------------------------------------------------------------------
	PluginCollection();
	//--------------------------------------------------------------------------------------------------------
	void processScanLogFile();
	//--------------------------------------------------------------------------------------------------------
	// Wertet filenames aus to-scan aus.
	// Testet -ob file == plugin, - als plug ladbar, -ob schon vorhanden oder veraendert 
	// und fuegt ggf. file hinzu. ** Nur im update prozess aufrufen **
	void checkFile( const Path &path, const Folder &folder );
	//--------------------------------------------------------------------------------------------------------
	// checkt ob plugin und holt informationen aus plugin.
	void peekFile ( processing::PluginInfo &out_info, processing::IHostInfo * );
	//--------------------------------------------------------------------------------------------------------
	// erzeugt neue PluginCollection Instanz und laed falls vorhanden dump file.
	static PluginCollection * create();
	//--------------------------------------------------------------------------------------------------------
	// fuegt log zu ladenden filename hinzu. nach update wird file geloescht.
	// ist beim naechsten start file noch da, war der letzte eintrag wohl schuld an einem absturz
	// beim updatevorgang. Liefert false wenn filename neu angelegt wird.
	static void appendLog ( const string &log_msg );
	//--------------------------------------------------------------------------------------------------------
	// liefert den dateinamen der als letztes versucht wurde zu laden.
	string analyzeLog();
public:
	//--------------------------------------------------------------------------------------------------------
	// liefert Plugin Ptr zu id.
	processing::Plugin::Ptr getPlugNode ( processing::IHostInfo *hostInfo, const PluginIdType &location );
	//--------------------------------------------------------------------------------------------------------
	processing::Plugin::Ptr restorePlugNode ( processing::IHostInfo *hostInfo, processing::PluginInfo &pI );
	//--------------------------------------------------------------------------------------------------------
	processing::PluginInfo restorePluginInfo ( processing::IHostInfo *hostInfo, processing::PluginInfo &pI );
	//--------------------------------------------------------------------------------------------------------
	void stopScanning() { abortScan = true; } 
	//--------------------------------------------------------------------------------------------------------
	bool isScanning();
	//--------------------------------------------------------------------------------------------------------
	sambag::cpsqlite::DataBase::Ptr & getDataBase() { return database; }
	//--------------------------------------------------------------------------------------------------------
	void update( processing::IHostInfo * );
	//--------------------------------------------------------------------------------------------------------
	static Ptr getPluginCollection();
	//--------------------------------------------------------------------------------------------------------
	virtual ~PluginCollection();
};
} // namespace com

#endif