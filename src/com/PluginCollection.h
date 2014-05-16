/*
 * ===========================================================================================================
 * PluginCollection.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

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
#include "processing/Plugin.h"
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"

namespace com {
class PluginCollection;
//============================================================================================================
extern void ShowDatabaseConnectionFailedMSG();
//============================================================================================================
/**
 * @class OnLoadFile.
 * Event: Datei wird geoeffnet.
 */
struct OnLoadFile : public com::events::Event {
//============================================================================================================
	std::string filename;
	OnLoadFile ( const std::string &filename ) : filename(filename) {}
};

//============================================================================================================
/**
 * @class OnFileLoaded.
 * Event: Datei wird geschlossen.
 */
struct OnFileLoaded : public com::events::Event {
//============================================================================================================
	std::string filename;
	processing::PluginInfo info;
	OnFileLoaded ( const std::string &filename, const processing::PluginInfo &info ) : 
		info(info), filename(filename) {}
};

//============================================================================================================
/**
 * @class ScanInterrupted.
 * Event: Scan wurde unterbrochen
 */
struct ScanInterrupted : public com::events::Event {
//============================================================================================================
	std::string cause;
	ScanInterrupted ( const std::string &cause ) : cause(cause) {}
};

//============================================================================================================
/**
 *  @class CleaningUpDataBase.
 *  Datenbank wird bereinigt.
 */
struct CleaningUpDataBase : public com::events::Event {};

//============================================================================================================
/**
 * @class ScanComplete.
 * Event: Scanvorgang beendet.
 */
struct ScanComplete : public com::events::Event {};
//============================================================================================================

//============================================================================================================
/**
 * @class ScanVisitor.
 * Konkreter Visitor zu sambag::com::dirWalker().
 * Bildet Vst-Plugin-Pfadstruktur auf Datenbank ab.
 */
class ScanVisitor : public sambag::com::IWalkerVisitor {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @exception InvalidPathEx
	 * Ausnahme: ungueltiger Path.
	 */
	struct InvalidPathEx {};
	//--------------------------------------------------------------------------------------------------------
	typedef sambag::com::Location Path;
	//--------------------------------------------------------------------------------------------------------
	typedef sambag::cpsqlite::DataBase::Int FolderID;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt Pfad in Datenbank ein.
	 * @param path
	 */
	void insert ( const Path &path );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Datenbank-Handler.
	 */
	sambag::cpsqlite::DataBase::Ptr db;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Datenbank-SQL-Ausfuehrer
	 */
	sambag::cpsqlite::DataBase::Executer::Ptr dbExe;
	//--------------------------------------------------------------------------------------------------------
	PluginCollection *client;
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Start-folder == root folder in Datenbank
	 * @param startFolder
	 */
	void setStartFolder ( const Path &startFolder );
	//--------------------------------------------------------------------------------------------------------
	ScanVisitor ( PluginCollection *client );
    //--------------------------------------------------------------------------------------------------------
    ~ScanVisitor();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Event: Walker wechselt Verzeichnis.
	 * @param path
	 * @return true, wenn Walker Verzeichnis weiter untersuchen soll.
	 */
	virtual bool changeDirectory ( const Path & path );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Event: datei wurde in Verzeichniss gefunden.
	 * @param file
	 */
	virtual void file ( const Path & file );
};

#define GET_FOLDER_ID(folder)   boost::tuples::get<1>( (folder) )
#define GET_FOLDER_NAME(folder) boost::tuples::get<0>( (folder) )

//============================================================================================================
/**
 * @class PluginCollection
 * Bietet Methoden zum Zugriff auf Konkrete Datenbank.
 */
//============================================================================================================
class PluginCollection : 
	public com::events::EventSender<OnLoadFile>, 
	public com::events::EventSender<OnFileLoaded>,
	public com::events::EventSender<CleaningUpDataBase>, 
	public com::events::EventSender<ScanComplete>,
	public com::events::EventSender<ScanInterrupted>
{
friend class ScanVisitor;
friend boost::shared_ptr<PluginCollection> getPluginCollection();
public:
	//--------------------------------------------------------------------------------------------------------
	typedef com::events::EventSender<OnLoadFile> OnLoadFileSender;
	typedef com::events::EventSender<OnFileLoaded> OnFileLoadedSender;
	typedef com::events::EventSender<CleaningUpDataBase> CleaningUpDataBaseSender;
	typedef com::events::EventSender<ScanComplete> ScanCompleteSender;
	typedef com::events::EventSender<ScanInterrupted> ScanInterruptedSender;
	//--------------------------------------------------------------------------------------------------------
	typedef std::string PluginIdType;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<PluginCollection> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef std::list < processing::PluginInfo > PluginInfoList;
	//--------------------------------------------------------------------------------------------------------
	typedef ScanVisitor::FolderID FolderID;
	//--------------------------------------------------------------------------------------------------------
	typedef ScanVisitor::FolderID Int;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::tuples::tuple<std::string, FolderID> Folder;
	//--------------------------------------------------------------------------------------------------------
	typedef std::list<Folder> Folders;
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
	/**
	 * initalisiert Datenbank.
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void initDB();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * aktualisiert scan-timestamp des Plugin.
	 * Wird benoetigt um Plugins zu ermitteln welche NICHT vom Scan untersucht wurden.
	 * Dies ist der Fall wenn zB. ein Plugin aus Verzeichnis entfernt wurde.
	 *
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void updateScanStamp();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Erzeugt anhand Pluginpfad PluginInfo-Objekt. Fuegt dieses DB hinzu.
	 * @param folder Database FolderID
	 * @param location pfad zu plugin.
	 * @return PluginInfo Objekt
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	processing::PluginInfo insertPlug ( const Folder &folder, const Path &location );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt PluginInfo-Objekt in DB ein.
	 * @param folder Database FolderID
	 * @param info
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void insertPlug ( const Folder &folder, const processing::PluginInfo &info );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * aktalisiert Plugin-Info Objekt in DB.
	 * @param info
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void updatePlug ( processing::PluginInfo &info );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Aktualisiert Plugin-Timestamp. Wird benoetigt um zu ermitteln, ob Datei sich veraendert hatt.
	 * @param info
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void updatePlugScanStamp ( processing::PluginInfo &info );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Entfernt Verzeichnisse aus DB die keinen Plugin-Inhalt bieten.
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void removeUnusedFolders();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Entfernt Plugins aus DB, dessen ScanStamp!=aktueller ScanStamp
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void removeUnusedPlugins();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * ueberprueft ob DB-datei valid.
	 * @throws DataBaseConnectionFailed
	 */
	void checkDataBaseIntegrity();
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @param pI
	 * @return true, wenn zu PluginInfo gehoeriges Plugin in Datenbank enthalten
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	bool searchPlugin ( processing::PluginInfo &pI );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param id
	 * @return Folder-Objekt zu FolderID
	 */
	Folder getFolder ( FolderID id ) const;
	//--------------------------------------------------------------------------------------------------------
	bool abortScan;
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @param the plugininfo object
	 */
	void addToHistory ( const processing::PluginInfo &pI );
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @param the outgoing plugininfo list
     * @param outgoing list with details
     * @note len(details) isAlways len(outList)
	 */
	void getRecentPlugins ( PluginInfoList &outList, std::vector<std::string> &details );
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @param the outgoing plugininfo list
     * @param outgoing list with details
     * @note len(details) isAlways len(outList)
	 */
	void getFavouritePlugins ( PluginInfoList &outList, std::vector<std::string> &details );
    //--------------------------------------------------------------------------------------------------------
	/**
	 * @param loc
	 * @return Folder-Objekt zu Pfad-String
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	Folder getFolder ( const Path &loc ) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return DB root
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	Folder getRootFolder() const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Unterordner zu Folder-id
	 * @param id
	 * @param outFolders Ziel Folder-Kontainer
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void getSubFolders( FolderID id, Folders &outFolders ) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Alle PluginInfo-Objekte eines Folder-Objektes.
	 * @param folder Folder-Objekt
	 * @param out_l Ziel PluginInfo-Kontainer
	 * @param showAll auch die Plugins hinzufuegen die nicht geoffnet werden konnten.
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void getPlugInfoList( const Folder &folder, PluginInfoList &out_l, bool showAll = false ) const;
	void getPlugInfoList( const FolderID &folderID, PluginInfoList &out_l, bool showAll = false ) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param location
	 * @return PluginInfo-Objekt zu Plugin-Pfad.
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	processing::PluginInfo getPlugInfo ( const Path &location ) const; 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn alle in Settings gesetzten Pluginverzeichnisse gescannt.
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	bool isAllScanned() const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl der erfolgreich geoeffneten Plugins.
	 */
	size_t getNumSucceed() const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl der Plugins die nicht geoeffnet werden konnten.
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	size_t getNumFailed() const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl der nicht untersuchten Plugins.
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	size_t getNumNotChecked() const;
	//********************************************************************************************************
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * aktueller Scanstamp
	 */
	time_t scanStamp;
	//--------------------------------------------------------------------------------------------------------
	frx::processing::IHostInfo::WPtr tmpHostInfo;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Scannt Verzeichnis.
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 * @param path
	 * @param vis
	 */
	void scanDirectory( const ScanVisitor::Path &path, ScanVisitor &vis );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Scannt alle Verzeichnisse
	 * @param Pfad-Set
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void scanDirectories( const Settings::PathnameSet& );
	//--------------------------------------------------------------------------------------------------------
	sambag::cpsqlite::DataBase::Ptr database;
	//--------------------------------------------------------------------------------------------------------
	mutable com::Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	//PathNameList ;
	//--------------------------------------------------------------------------------------------------------
	Filenames blackList; // files to skip 
	//--------------------------------------------------------------------------------------------------------
	Settings &settings;
	//--------------------------------------------------------------------------------------------------------
	PluginCollection();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * analysiert scanlog auf fehlgeschlagene Plugin-zugriffe (Absturz).
	 * Gibt Warnmeludng aus wenn fehlgeschlagenes Plugin gefunden.
	 * TODO: entferne Warnmeldung aus Geschaeftslogik.
	 */
	void processScanLogFile();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Testet -ob file == plugin, - als plug ladbar, -ob schon vorhanden oder veraendert
	 * und fuegt ggf. file hinzu.
	 * @param path
	 * @param folder
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void checkFile( const Path &path, const Folder &folder );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * checkt ob, als Plugin ladbar und holt informationen aus plugin.
	 * @param out_info
	 * @param hostInfo
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	void peekFile ( processing::PluginInfo &out_info, frx::processing::IHostInfo::Ptr hostInfo);
	//--------------------------------------------------------------------------------------------------------
	static PluginCollection * create();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt log zu ladenden filename hinzu. Nach Scan wird File geloescht.
	 * Ist beim naechsten Start file noch da, war der letzte Eintrag wohl schuld an einem Absturz.
	 * @param log_msg
	 */
	static void appendLog ( const std::string &log_msg );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return den Dateinamen aus Log, der als letztes versucht wurde zu laden.
	 */
	std::string analyzeLog();
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @param hostInfo
	 * @param pI
	 * @return Plugin-Objekt zu PluginInfo-Objekt
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	frx::processing::Plugin::Ptr restorePlugNode ( frx::processing::IHostInfo::Ptr hostInfo, processing::PluginInfo &pI );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * aktualisiert Pluginfo-Objekt
	 * @param hostInfo
	 * @param pI
	 * @return neues aktualisertes Pluginfo-Objekt
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 */
	processing::PluginInfo restorePluginInfo ( processing::PluginInfo &pI );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * bricht scan ab.
	 */
	void stopScanning() { abortScan = true; } 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn Scanvorgang aktiv.
	 */
	bool isScanning() const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * sambag::cpsqlite::DataBase-Objekt
	 * @return
	 */
	sambag::cpsqlite::DataBase::Ptr getDataBase() const { return database; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Initiiert Scanvorgang
	 * @throw sambag::cpsqlite::DataBaseQueryFailed
	 * @param hostInfo
	 */
	void update( frx::processing::IHostInfo::Ptr hostInfo);
	//--------------------------------------------------------------------------------------------------------
	virtual ~PluginCollection();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern PluginCollection::Ptr getPluginCollection();
} // namespace com

#endif
