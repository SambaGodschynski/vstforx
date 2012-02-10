#include "PluginCollection.h"
#include "Log.h"
#include "one4All.h"
#include <sstream>
#include "PluginCollectionSQL.h"
#include <boost/filesystem.hpp>


#define DB_QUERY(x)											\
	try {x}													\
	catch ( ::sambag::cpsqlite::DataBaseQueryFailed &ex ) { \
		TOLOG(ex.errorMessage + " => " + ex.query );		\
	    throw;												\
	}

namespace com {
//============================================================================================================
// class ScanVisitor
//------------------------------------------------------------------------------------------------------------
void ScanVisitor::insert ( const ScanVisitor::Path &path ) 
{
	using namespace sambag;
	using namespace cpsqlite;
	using namespace sqlcommands;

	DataBase::Results res;
	DB_QUERY (
		// path schon in db ?
		sambag::cpsqlite::ParameterList pL;
		string q = TblFolder::getFolder( path, pL );
		dbExe->execute ( q, pL, res );
		if ( !res.empty() ) { // ja:
			sambag::cpsqlite::ParameterList pL;
			dbExe->execute( TblFolder::updateFolder(path, client->scanStamp, pL), pL );
		}
		else {
			sambag::cpsqlite::ParameterList pL;
			string q = TblFolder::insertFolder( path, client->scanStamp, pL );
			dbExe->execute(q, pL);
		}
	)
}
//------------------------------------------------------------------------------------------------------------
void ScanVisitor::setStartFolder ( const ScanVisitor::Path &startFolder ) {
	using namespace sambag;
	using namespace cpsqlite;
	using namespace sqlcommands;
	// insert
	DataBase::Results res;
	DB_QUERY (
		// folder schon in db ?
		sambag::cpsqlite::ParameterList pL;
		string q =  TblFolder::getFolder( startFolder, pL );
		dbExe->execute ( q, pL, res );
		if ( res.empty() ) { // nein:
			sambag::cpsqlite::ParameterList pL;
			dbExe->execute( TblFolder::insertFolder(
				startFolder, 
				PluginCollection::ROOT_FOLDER_ID, 
				client->scanStamp,
				pL ), pL );
		} else { // ja:
			// folder war mal child folder => update nach root
			if ( res[0]->get("parentFolderId") != "NULL" )
				dbExe->execute( TblFolder::updateParentFolderID( res[0]->getConv<FolderID>("id"), 
				                                                 PluginCollection::ROOT_FOLDER_ID ) );
		}
	)
}
//------------------------------------------------------------------------------------------------------------
ScanVisitor::ScanVisitor ( PluginCollection *client ) : 
	client(client), db( client->getDataBase() ), dbExe( db->getExecuter() ) 
{
}
//------------------------------------------------------------------------------------------------------------
bool ScanVisitor::changeDirectory ( const ScanVisitor::Path & path ) {
	
	// ausnahme macosx: .vst/.app == ordner => wie datei behandeln
	if ( !::com::isDirectory( path.string() ) ) {
		file(path);
		return false; // skip folder scan
	}
	insert ( path );
	return true;
}
//------------------------------------------------------------------------------------------------------------
void ScanVisitor::file ( const ScanVisitor::Path &loc ) {
	Path p = loc.parent_path();
	FolderID id = GET_FOLDER_ID( client->getFolder ( p ) );
	if ( id==PluginCollection::NULL_FOLDER_ID ) throw TreeError();
	PluginCollection::Folder folder = client->getFolder( id );
	if ( folder == PluginCollection::NULL_FOLDER ) throw TreeError();
	client->checkFile ( loc, folder );
}
//============================================================================================================
// Klasse PluginCollection : <Singleton>
// Datenbank fuer Plugin Files.
// Die festgelegten root verzeichnisse werden iteriert und alle vorhandenen Plugins gesammelt.
// Die angesammelten daten werden auf HD gespeichert und nur bei verzeichniss aenderung aktualisiert.
//============================================================================================================
//============================================================================================================
const PluginCollection::FolderID PluginCollection::NULL_FOLDER_ID = 0;
//------------------------------------------------------------------------------------------------------------
const PluginCollection::FolderID PluginCollection::ROOT_FOLDER_ID = 1;
//------------------------------------------------------------------------------------------------------------
const PluginCollection::Folder   PluginCollection::NULL_FOLDER = PluginCollection::Folder( "", UINT_MAX );
//------------------------------------------------------------------------------------------------------------
namespace {
boost::weak_ptr<PluginCollection> highlander; // es darf nur einen geben
}
//------------------------------------------------------------------------------------------------------------
PluginCollection::PluginCollection() : 
	settings( SETTINGS ), tmpHostInfo(NULL), abortScan(false), scanStamp(0)
{
	using namespace sambag::cpsqlite;
	using namespace com::sqlcommands;
	try {
		database = DataBase::getDataBase ( settings->getPlugCollectionDumpFilename() );
		// test access
		checkDataBaseIntegrity(); // throws database connection failed
		// check whether timestamp exists. throws DataBaseQueryFailed if not
		DataBase::Executer::Ptr exec(database->getExecuter());
		DataBase::Results res;
		exec->execute(TblLastScan::getScanStamp(), res);
		// init db
		initDB();
	} catch ( ... ) {
		// remove file, try again
		database.reset();
		boost::filesystem::remove( settings->getPlugCollectionDumpFilename() );
		database = DataBase::getDataBase ( settings->getPlugCollectionDumpFilename() );
		initDB();
	}
}
//------------------------------------------------------------------------------------------------------------
bool PluginCollection::isScanning() {
	boost::unique_lock<boost::timed_mutex> lock( mutex, boost::try_to_lock);
	if (!lock.owns_lock()) return true;
	return false;
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::processScanLogFile() {
	// Logfile Fehlerbehandlung
	if ( exists( Filename(Settings::getPlugInitLogFilename()) ) ) { // ja: 
		Filename f = Filename ( analyzeLog () );
		if (exists (f)) {
			MessageBoxReturn ret = 
				com::MessageBox ( "Attention!", "The last try to access on " + f.string() + 
								  " failed!\n Do you want to skip this file?", com::MSG_QUESTION );
			if ( ret == com::MSG_RET_YES ) {
				appendLog ( "?" + f.string() ); 
				blackList.push_back ( f.string() );
			}
		}
	} // Logfile Fehlerbehandlung
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::scanDirectories ( const Settings::PathnameSet &pathSet ) {
	using namespace sqlcommands;
	using namespace sambag::cpsqlite;
	
	ScanVisitor vis( this );

	DataBase::Executer::Ptr exec = database->getExecuter();
	if ( settings->isFastScan() ) // accerlate db writein if fastscan setted
		exec->execute("BEGIN TRANSACTION;");
	
	Settings::PathnameSet::const_iterator it = pathSet.begin();
	for ( ; it!=pathSet.end(); ++it ) {
		scanDirectory( ScanVisitor::Path(*it), vis );
	}

	if ( settings->isFastScan() )
		exec->execute("COMMIT TRANSACTION;");

	// scan complete now clean up db
	EventSender<CleaningUpDataBase>::notifyEventListeners ( this, CleaningUpDataBase() );

	removeUnusedPlugins();  
	removeUnusedFolders();
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::scanDirectory ( const ScanVisitor::Path &path, ScanVisitor &vis ) {
	vis.setStartFolder ( path );
	abortScan = false;
	sambag::com::dirWalker( path, vis, &abortScan );
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::update(  processing::IHostInfo *hostInfo ) {
	TRY_TO_LOCK_TIMED (mutex);
	TOLOG ( "update plugin collection." );
	processScanLogFile();
	appendLog ( "plugin init log:" );
	// hole plugin verzeichnisse aus settings
	Settings::PathnameSet const &pathSet = settings->getPluginDirectoryList();
	//!!
	tmpHostInfo = hostInfo;
	
	updateScanStamp();
	//!!
	// scan setted directories
	try {
		if ( pathSet.empty() ) {
			removeUnusedFolders();
		} else scanDirectories( pathSet );
	} catch ( const sambag::cpsqlite::DataBaseException &ex ) {
		// send interrupt
		EventSender<ScanInterrupted>::notifyEventListeners (
			this,
			ScanInterrupted("Database Exception")
		);
		return;
	} catch ( ... ) {
		// send interrupt
		EventSender<ScanInterrupted>::notifyEventListeners (
			this,
			ScanInterrupted("Unkown Exception")
		);
		return;
	}
	//!!
	tmpHostInfo = NULL;
	//!!
	try {
		std::remove ( Settings::getPlugInitLogFilename().c_str() ); // log wieder loeschen
	} catch (...) {
		return;
	}
	EventSender<ScanFinished>::notifyEventListeners ( this, ScanFinished() );
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::appendLog ( const string &log_msg ) {
	ofstream f;
	try {
		f.open ( Settings::getPlugInitLogFilename().c_str(), ios::app );
		f<<log_msg<<endl;
	}
	catch ( ... ) {
		f.close();
		return;
	}
	f.close();
}
//------------------------------------------------------------------------------------------------------------
string PluginCollection::analyzeLog() {
	ifstream f;
	string str;
	try {
		f.open ( Settings::getPlugInitLogFilename().c_str() );
		while ( !f.eof() ) {
			MyString l;
			getline(f, l);
			if ( l.trim().length() > 0 ) str = l;
			else continue;
			// file to skip schon vorhanden ( 2 nicht funz. plugs )
			if ( l.at(0) == '?' ) blackList.push_back (  l.substr ( 1 ) );
		}
	}
	catch ( ... ) {
		f.close();
	}
	f.close();
	return str;
}
//------------------------------------------------------------------------------------------------------------
processing::Plugin::Ptr PluginCollection::getPlugNode ( processing::IHostInfo *hostInfo, 
														  const PluginCollection::PluginIdType &location ) 
{
	using namespace processing;
	PluginInfo pI = getPlugInfo ( location );
	// plugin not in db => return NULL
	if ( !pI.isValid() ) return processing::Plugin::Ptr();
	return PluginFactory::createPlugNode ( hostInfo, pI.location ); 
}
//------------------------------------------------------------------------------------------------------------
processing::PluginInfo PluginCollection::restorePluginInfo ( processing::IHostInfo *hostInfo, 
															 processing::PluginInfo &info ) 
{
	using namespace processing;
	PluginInfo pI = getPlugInfo ( info.location );
	// plugin not in db => search in db
	if ( !pI.isValid() ) {
		tmpHostInfo = hostInfo;
		bool b = searchPlugin( info );
		tmpHostInfo = NULL;
		if ( !b ) // plugin not found
			return processing::PluginInfo(); // NULL
	}
	return info;
}
//------------------------------------------------------------------------------------------------------------
processing::Plugin::Ptr PluginCollection::restorePlugNode ( processing::IHostInfo *hostInfo, 
															 processing::PluginInfo &info ) 
{
	using namespace processing;
	PluginInfo pI = restorePluginInfo ( hostInfo, info );
	if ( !pI.isValid() ) return processing::Plugin::Ptr(); // NULL
	return PluginFactory::createPlugNode ( hostInfo, info.location ); 
}
//------------------------------------------------------------------------------------------------------------
PluginCollection::Ptr PluginCollection::getPluginCollection() {
	PluginCollection::Ptr pC = highlander.lock();
	if (!pC) {
		highlander = pC = Ptr ( new PluginCollection() );
	}
	return pC;
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::peekFile ( processing::PluginInfo &out_info, processing::IHostInfo *hostinfo )
{
	using namespace processing;
	if (!hostinfo) throw com::ppiError::NullPointer("null pointer",__FILE__,__LINE__);
	// is fastscan?
	if ( settings->isFastScan() ) {
		out_info.access = PluginInfo::NOT_CHECKED;
		out_info.name = Path( out_info.location ).filename();
		// set not the timestamp! because if rescan without the fast option we want to peek in plug
		// out_info.timestamp = last_write_time(out_info.location);
		return;
	}
	TOLOG ("peek " + out_info.location );
	appendLog ( out_info.location );		   // eintrag ins scan log	
	Plugin::Ptr n = PluginFactory::createPlugNode ( hostinfo, out_info.location );
	if ( !n ) { // loading failed
		appendLog ( "?" + out_info.location );
		                                   // nochmal ins log damit nach einem evntl. absturz
										   // im scan diese datei nicht nochmal versucht wird zu laden. 
		out_info.access = PluginInfo::FAILED;
		// set timestamp
		out_info.timestamp = last_write_time(out_info.location);
		return;
	}
	if ( ! n->isAccessable() ) {
		out_info.access = PluginInfo::FAILED;
		// set timestamp
		out_info.timestamp = last_write_time(out_info.location);
		return;
	}
	// fill out
	out_info = n->getPluginInfo();
	out_info.access = PluginInfo::SUCCEED;
	// set timestamp
	out_info.timestamp = last_write_time(out_info.location);
	return;
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::checkFile( const PluginCollection::Path &path, const PluginCollection::Folder &folder ) {
	using namespace processing;
	if ( !::com::isPlugFilename( path.string() ) ) return;
	// . update OnLoad listeners:
	EventSender<OnLoadFile>::notifyEventListeners ( this, OnLoadFile ( path.string() ) );

	// . datei schon in db ?
	PluginInfo tmp = getPlugInfo ( path );
	if ( tmp.isValid() ) { // ja, schon vorhanden!
		if ( tmp.hasChanged ( last_write_time (path) )  ) { // hatt sich geaendert
			updatePlug ( tmp );
			EventSender<OnFileLoaded>::notifyEventListeners ( this, OnFileLoaded ( path.string(), tmp ) );
			return;
		}
		updatePlugScanStamp(tmp);
		EventSender<OnFileLoaded>::notifyEventListeners ( this, OnFileLoaded ( path.string(), tmp ) );
		return; // already in db => return
	}
	
	// . nicht in db aber auf schwarzer liste?
	if ( contains<Filenames> ( blackList, path.string() ) ) {
		PluginInfo info;
		info.location  = path.string();
		info.timestamp = last_write_time ( path );
		info.access = PluginInfo::FAILED;
		insertPlug ( folder, info );
		EventSender<OnFileLoaded>::notifyEventListeners ( this, OnFileLoaded ( path.string(), info ) );
		return;
	}
	//
	PluginInfo info = insertPlug ( folder, path ); // opens plugin and inserts into db
	// notify listeners
	EventSender<OnFileLoaded>::notifyEventListeners ( this, OnFileLoaded ( path.string(), info ) );
}
//------------------------------------------------------------------------------------------------------------
PluginCollection::~PluginCollection () {
	TOLOG ("closing plugin collection");
}
//************************************************************************************************************
// DataBase
//------------------------------------------------------------------------------------------------------------
namespace {
	using namespace sambag::cpsqlite;
	using namespace sqlcommands;
	using namespace processing;
	typedef list<PluginInfo> PluginInfoList;
	bool extractAndAdd ( const DataBase::Results &results, PluginInfoList &pL ) {
		if ( results.empty() ) return false;
		for ( size_t i=0; i<results.size(); ++i) {
			DataBase::Result::Ptr res = results[i];
			PluginInfo info;
			PluginCollection::Path p = res->get( TblPlugins::location() );
		    info.location   = p.string();
	        info.name       = res->get( TblPlugins::name() );
	        info.timestamp  = res->getConv<time_t>( TblPlugins::timestamp() );
			info.access     = (PluginInfo::AccessState)res->getConv<int>( TblPlugins::access() );
			info.uid        = res->getConv<int>( TblPlugins::uid() );
			info.isSynth    = res->getConv<bool>( TblPlugins::isSynth() );
			info.pluginType = (PluginInfo::PluginType)res->getConv<int>( TblPlugins::pluginType() );
			pL.push_back( info );
		}
		return true;
	}
	// copmare two plugininfo objects. ignores location.
	bool compare( const PluginInfo &a, const PluginInfo &b ) {
		return a.uid		== b.uid        &&	// this should be enough, but who knows?
			   a.pluginType == b.pluginType &&
			   a.isSynth	== b.isSynth; 
	}
} // namespace
//------------------------------------------------------------------------------------------------------------
void PluginCollection::checkDataBaseIntegrity() {
	using namespace sambag::cpsqlite;
	DataBase::Results res;
	DataBase::Executer::Ptr ex = database->getExecuter();
	ex->execute( "PRAGMA quick_check(1);", res );
	if ( res.empty() ) throw DataBaseConnectionFailed();
	if ( res[0]->get("integrity_check") != "ok" ) throw DataBaseConnectionFailed();
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::removeUnusedFolders() {	
	using namespace sambag::cpsqlite;
	using namespace sqlcommands;

	// remove unused folders
	ParameterList pL;
	std::string q = TblFolder::removeUnusedFolders(scanStamp);
	DataBase::Executer::Ptr exec = database->getExecuter();
	DB_QUERY(
		exec->execute(q); 
	)
	
	// reset folder visibility
	DB_QUERY(
		exec->execute( TblFolder::resetFolderVisibility() ); 
	)

	// hide folders without related content
	// while ( s = hole ordner ohne inhalt ):
	//     for x in s:
	//         delete x
	DataBase::Results res;
	DB_QUERY(
		while (true) {
			exec->execute( TblFolder::getEmptyFolders(), res );
			if ( res.empty() ) break;
			for ( size_t i=0; i<res.size(); ++i ) {
				FolderID id = res[i]->getConv<FolderID>("id");
				exec->execute( TblFolder::setFolderVisible( id, false ) );
			}
		}
	)
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::removeUnusedPlugins() {	
	using namespace sambag::cpsqlite;
	using namespace sqlcommands;
	std::string q = TblPlugins::removeUnusedPlugins( scanStamp );
	DataBase::Executer::Ptr exec = database->getExecuter();
	DB_QUERY(
		exec->execute( q ); 
	)
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::initDB() {
	using namespace sambag;
	using namespace cpsqlite;
	using namespace sqlcommands;

	DataBase::Executer::Ptr exec = database->getExecuter(); 
	DataBase::Results res;
	DB_QUERY (
		// create tables ( if not exsits ): // throws DataBaseQueryFailed, DataBaseQueryTimeout
		exec->execute( TblFolder::create() ); 
		exec->execute( TblPlugins::create() ); 
		exec->execute( TblLastScan::create() ); 
		sambag::cpsqlite::ParameterList pL;
		string q = TblFolder::getFolder( TblFolder::root(), pL );
		exec->execute ( q, pL, res );
		if (  res.empty() ) { // exsists root
			exec->execute( TblFolder::insertRoot() );
			LOG_ASSERT ( exec->lastInsertRowId() == ROOT_FOLDER_ID );
		}
	)
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::updateScanStamp() {
	DB_QUERY (
		// get/update timestamp:
		// this timestamp will be setted while scanning to every plugin entry.
		// so its easy to detect the plugins which are removed from folder:
		// all plugins where plugin::timestamp!=LastScan::timestamp
		DataBase::Executer::Ptr exec = database->getExecuter(); 
		DataBase::Results res;
		time_t oldTimestamp;
		exec->execute(TblLastScan::getScanStamp(), res);
		if ( res.empty() ) {
			oldTimestamp = 0;
		} else {
			oldTimestamp = res[0]->getConv<time_t>(TblLastScan::scanstamp(), 0);
		}
		scanStamp = oldTimestamp;
		while (scanStamp==oldTimestamp) {
			scanStamp = ::time(NULL);
			if (scanStamp==0) {
				LOG_ASSERT(false);
				break;
			}
		}
		if (oldTimestamp==0) { // first entry
			exec->execute(TblLastScan::insertScanStamp(scanStamp));
			return;
		} 
		exec->execute(TblLastScan::updateScanStamp(scanStamp));
	)
}
//------------------------------------------------------------------------------------------------------------
size_t PluginCollection::getNumSucceed() const {
	using namespace sqlcommands;
	using namespace sambag::cpsqlite;
	DataBase::Executer::Ptr exec = database->getExecuter(); 
	DataBase::Results res1;
	DB_QUERY (
		exec->execute( TblPlugins::getNumPluginsSucceed(), res1 );
	)
	return res1[0]->getConv<size_t>("COUNT(id)");
}
//------------------------------------------------------------------------------------------------------------
size_t PluginCollection::getNumFailed() const {
	using namespace sqlcommands;
	using namespace sambag::cpsqlite;
	DataBase::Executer::Ptr exec = database->getExecuter(); 
	DataBase::Results res1;
	DB_QUERY (
		exec->execute( TblPlugins::getNumPluginsFailed(), res1 );
	)
	return res1[0]->getConv<size_t>("COUNT(id)");
}
//------------------------------------------------------------------------------------------------------------
size_t PluginCollection::getNumNotChecked() const {
	using namespace sqlcommands;
	using namespace sambag::cpsqlite;
	DataBase::Executer::Ptr exec = database->getExecuter(); 
	DataBase::Results res1;
	DB_QUERY (
		exec->execute( TblPlugins::getNumPluginsNotChecked(), res1 );
	)
	return res1[0]->getConv<size_t>("COUNT(id)");
}
//------------------------------------------------------------------------------------------------------------
PluginCollection::Folder PluginCollection::getRootFolder() const {
	using namespace sqlcommands;
	return Folder( TblFolder::root(), ROOT_FOLDER_ID );
}
//------------------------------------------------------------------------------------------------------------
PluginCollection::Folder PluginCollection::getFolder( PluginCollection::FolderID id ) const {
	using namespace sqlcommands;
	using namespace sambag::cpsqlite;
	DataBase::Executer::Ptr exec = database->getExecuter(); 
	DataBase::Results res;
	DB_QUERY (
		exec->execute( TblFolder::getFolder(id), res );
	)
	if ( res.empty() ) return Folder( "", NULL_FOLDER_ID );
	return Folder( res[0]->get( TblFolder::name() ), id );
}
//------------------------------------------------------------------------------------------------------------
PluginCollection::Folder PluginCollection::getFolder( const PluginCollection::Path &loc ) const {
	using namespace sqlcommands;
	using namespace sambag::cpsqlite;
	DataBase::Executer::Ptr exec = database->getExecuter(); 
	DataBase::Results res;
	DB_QUERY (
		sambag::cpsqlite::ParameterList pL;
		string q =  TblFolder::getFolder( loc, pL );
		exec->execute( q, pL, res );
	)
	if ( res.empty() ) return Folder( "", NULL_FOLDER_ID );
	return Folder( res[0]->get( TblFolder::name() ), res[0]->getConv<Int>( TblFolder::id() ) );
}
//------------------------------------------------------------------------------------------------------------
processing::PluginInfo PluginCollection::getPlugInfo ( const PluginCollection::Path &path ) const {
	using namespace sambag::cpsqlite;
	using namespace sqlcommands;
	using namespace processing;
	DataBase::Executer::Ptr exec = database->getExecuter();
	DataBase::Results results;
	DB_QUERY (
		sambag::cpsqlite::ParameterList pL;
		string q = TblPlugins::getPlugin( path.string(), pL );
		exec->execute( q, pL, results );
	)
	PluginInfoList l;
	if ( extractAndAdd ( results, l ) ) return l.front();
	else return PluginInfo();
}
//------------------------------------------------------------------------------------------------------------
bool PluginCollection::searchPlugin ( processing::PluginInfo &pI ) {
	// P+= plugs by name
	// P+= plugs by uid
	// for x in P:
	//   if x(uid, type, isSynth, numIOs) == plugin: return x
	// return null
	using namespace sambag::cpsqlite;
	using namespace sqlcommands;
	using namespace processing;
	PluginInfoList p;
	DataBase::Results res;
	DataBase::Executer::Ptr exec = database->getExecuter(); 
	ParameterList pL;
	// fill P
	DB_QUERY (
		exec->execute( TblPlugins::getPluginsByName( pI.name, pL ), pL, res );
		extractAndAdd ( res, p );
		exec->execute( TblPlugins::getPluginsByUid( pI.uid, pL ), pL, res );
		extractAndAdd ( res, p );
	)
	PluginInfoList::iterator it = p.begin();
	for ( ; it!=p.end(); ++it ) {
		if ( compare( *it, pI ) ) {
			pI = *it;
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------------------------------------
processing::PluginInfo PluginCollection::insertPlug ( const PluginCollection::Folder &folder, const PluginCollection::Path &loc ) 
{
	using namespace sambag::cpsqlite;
	using namespace sqlcommands;
	using namespace processing;
	if ( !exists(loc) ) return PluginInfo();
	PluginInfo pi;
	pi.location = loc.string();
	//get pluginfo by open plugin
	peekFile ( pi, tmpHostInfo );
	insertPlug ( folder, pi );
	return pi;
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::insertPlug ( const PluginCollection::Folder &folder, const processing::PluginInfo &pi )
{
	using namespace sambag::cpsqlite;
	using namespace sqlcommands;
	using namespace processing;
	if ( !pi.isValid() ) return;

	DataBase::Executer::Ptr exec = database->getExecuter(); 
	ParameterList pL;
	string query = TblPlugins::insertPlugin ( pi.location, 
											  pi.name,
											  pi.uid,
											  pi.isSynth,
											  pi.pluginType,
											  GET_FOLDER_ID(folder),
											  pi.timestamp,
											  scanStamp,
											  pi.access,
											  pL );
	DB_QUERY (
		exec->execute(query, pL);
	)
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::updatePlug ( processing::PluginInfo &pi ) {
	using namespace sambag::cpsqlite;
	using namespace sqlcommands;
	using namespace processing;

	if ( !pi.isValid() ) return;
	
	//get pluginfo by open plugin
	peekFile ( pi, tmpHostInfo );
	ParameterList pL;
	DataBase::Executer::Ptr exec = database->getExecuter(); 
	string query = TblPlugins::updatePlugin ( pi.location, 
											  pi.name,
											  pi.uid,
											  pi.isSynth,
											  pi.pluginType,
											  pi.timestamp,
											  scanStamp,
											  pi.access,
											  pL );
	DB_QUERY (
		exec->execute( query, pL );
	)
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::updatePlugScanStamp ( processing::PluginInfo &pi ) {
	using namespace sambag::cpsqlite;
	using namespace sqlcommands;
	using namespace processing;

	if ( !pi.isValid() ) return;
	
	//get pluginfo by open plugin
	ParameterList pL;
	DataBase::Executer::Ptr exec = database->getExecuter(); 
	string query = TblPlugins::updateScanStamp ( pi.location, 
											     scanStamp,
											     pL );
	DB_QUERY (
		exec->execute( query, pL );
	)
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::getPlugInfoList( const Folder &folder, 
									    PluginCollection::PluginInfoList &l,
									    bool showAll ) const
{
	using namespace sambag::cpsqlite;
	DataBase::Executer::Ptr exec = database->getExecuter(); 
	DataBase::Results res;
	
	DB_QUERY (
		exec->execute( 
		sqlcommands::TblPlugins::getPluginsByFolder( GET_FOLDER_ID(folder), showAll ),
		res 
		);
	)
	extractAndAdd ( res, l );
}
//------------------------------------------------------------------------------------------------------------
void PluginCollection::getSubFolders( FolderID id, Folders &outFolders ) const {
	using namespace sambag::cpsqlite;
	DataBase::Executer::Ptr exec = database->getExecuter(); 
	DataBase::Results res;
	DB_QUERY (
		exec->execute( sqlcommands::TblFolder::getSubFolders( id ), res );
	)
	if ( res.empty() ) return;
	for ( size_t i=0; i<res.size(); ++i ) {
		outFolders.push_back( 
			Folder( 
				res[i]->get( sqlcommands::TblFolder::name() ), 
				res[i]->getConv<FolderID>( sqlcommands::TblFolder::id() )   
			) 
		);
	}
}
//------------------------------------------------------------------------------------------------------------
bool PluginCollection::isAllScanned() const {
	using namespace sambag::cpsqlite;
	using namespace sqlcommands;
	using namespace processing;
	Settings::PathnameSet const &l = settings->getPluginDirectoryList();
	ParameterList pL;
	DataBase::Results res;
	DataBase::Executer::Ptr exec = database->getExecuter(); 

	if ( l.empty() ) {
		// no folders setted.
		// there should not exists any subfolders in root.
		exec->execute( 	TblFolder::getSubFolders( GET_FOLDER_ID( getRootFolder() ), true ), res );
		return res.empty();
	}

	string query = TblFolder::getFolders<Settings::PathnameSet>( l, pL );
	DB_QUERY (
		exec->execute( query, pL, res );
	)
	return res.size() == l.size();
}
} //namespace com 
