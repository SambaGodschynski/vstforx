/*
 * ===========================================================================================================
 * PluginCollectionTest.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */


#include <tuple>
#include <map>
#include <string>
#include <iostream>

//              folder       subfolder
typedef std::multimap< std::string, std::string > ExcpectedFolderMap;


static std::ostream & operator<<(std::ostream &os, const ExcpectedFolderMap& m)
{
    if (m.empty()) {
        os<<"{}";
        return os;
    }
    ExcpectedFolderMap::const_iterator it = m.begin();
    os<<"{"<<std::endl;
    for(; it!=m.end(); ++it) {
        os<<it->first<<" : " <<it->second<<", "<<std::endl;
    }
    os<<"}";
    return os;
}


#include <cppunit/config/SourcePrefix.h>
#include "PluginCollectionTest.hpp"
#include "com/MyString.h"
#include "processing/processing.h"
#include "processing/ConcreteProcessAdapter.h"
#include "com/one4All.h"
#include <boost/assign/list_of.hpp>
#include <processing/dspTools.h>
#include "com/PluginCollectionSQL.h"
#include <iostream>
#include <boost/assign.hpp>
#include <boost/foreach.hpp>

#if WIN32
#define VSTPLUG_EXT ".dll"
#else
#define VSTPLUG_EXT ".vst"
#endif

#define FAST_SCAN_TIME 15.0 //seconds

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::PluginCollectionTest );


namespace tests {
//=============================================================================
processing::Graph::Ptr PluginCollectionTest::createGraph( int blockSize, float samplerate ) 
{
//=============================================================================
	using namespace processing;
	Graph::Ptr graph = Graph::create ( dummyFX );
	Graph::Janitor::Ptr janitor = graph->getJanitor();
	dummyFX->setSampleRate ( samplerate );
	dummyFX->setBlockSize ( blockSize );
	janitor->hostBaseConfigChanged();
	return graph;
}
//=============================================================================
PluginCollectionTest::PluginCollectionTest() : numHandlerCalled(0) {
//=============================================================================
	dummyFX = processing::DummyFX::create( NULL );
	settings = &com::getSettings();
	settings->fastScan = false;
}
//=============================================================================
PluginCollectionTest::~PluginCollectionTest() {
//=============================================================================
	boost::filesystem::remove( settings->getPlugCollectionDumpFilename() );
	assert ( 
		!boost::filesystem::exists( 
			settings->getPlugCollectionDumpFilename() 
		) 
	);
}
//=============================================================================
void PluginCollectionTest::testConstructor() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> std constr.
    PluginCollection::Ptr pC = getPluginCollection();
}
static const char BUSY[] = { '/','-','\\','|' };
static const size_t NUM_BUSYS = sizeof(BUSY) / sizeof(BUSY[0]);
//=============================================================================
void PluginCollectionTest::eventHandler( void *src, const com::OnLoadFile &ev ) {
//=============================================================================
	/*if ( numHandlerCalled == 0 ) {
		cout<<"  "<<BUSY[0];
		++numHandlerCalled;
		return;
	}
	cout<<"\b"<<BUSY[ (numHandlerCalled++) % NUM_BUSYS ];*/
}
//=============================================================================
void PluginCollectionTest::eventHandler( void *src, const com::ScanComplete &ev ) {
//=============================================================================
}

static const size_t NUM_PLUG_COLLECTION = 31;

//=============================================================================
// after call with ROOT_FOLDER exp has to be empty.
void resetPluginCollection( processing::Graph::Ptr graph ) 
{
	using namespace com;
	using namespace processing;
	Settings * settings = &getSettings();
	PluginCollection::Ptr pC = getPluginCollection();
	Settings::PathnameSet tmp = settings->getPluginDirectoryList();
	settings->clearVSTFolders();
	// scan:
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumSucceed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumFailed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumNotChecked() );
	// insert saved directories
	Settings::PathnameSet::const_iterator it = tmp.begin();
	for ( ; it!=tmp.end(); ++it ) {
		settings->addPluginFolder( *it );
	}
}
//=============================================================================

//=============================================================================
void _checkTree( ::com::PluginCollection::Ptr pC, 
			    ::com::PluginCollection::Folder currFolder,
				ExcpectedFolderMap &exp ) 
{
//=============================================================================
	using namespace com;
	PluginCollection::Folders folders;
	pC->getSubFolders ( GET_FOLDER_ID(currFolder), folders );
	
	// iterate db subfolders of currFolder :
	PluginCollection::Folders::iterator it = folders.begin();
	
	for ( ;it!=folders.end(); ++it ) {
		// recursive call
		_checkTree ( pC, *it, exp );
		// find folder=>subfolder
		ExcpectedFolderMap::iterator end, expIt;
		std::tie( expIt, end ) = exp.equal_range( GET_FOLDER_NAME(currFolder) ); // subfolders of currFolder in exp
		bool found = false;
		for ( ;expIt!=end; ++expIt ) {
			// folder found in exp => erase entry
			if ( GET_FOLDER_NAME(*it) == expIt->second ) { 
				exp.erase( expIt );
				found = true;
				break;
			}
		} // for exp.equal_range( GET_FOLDER_NAME(currFolder) )
		if ( !found ) {
			CPPUNIT_ASSERT_MESSAGE ( GET_FOLDER_NAME(*it) + " not expected.", false );
		}
	}
	
}
//=============================================================================
// after call with ROOT_FOLDER, exp has to be empty.
void checkTree( ::com::PluginCollection::Ptr pC, ExcpectedFolderMap &exp ) 
{
//=============================================================================
	using namespace com;
	_checkTree( pC, pC->getRootFolder(), exp );
	CPPUNIT_ASSERT_EQUAL( ExcpectedFolderMap(), exp );
}

//=============================================================================
void PluginCollectionTest::testScan() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	using namespace boost::assign;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan whole folder. expect 93 plugs 
	// setup folders
	settings->clearVSTFolders();
	sambag::com::Location path = boost::filesystem::absolute("testVstFolder");
	settings->addPluginFolder( path.string() );
	// start scan
	PluginCollection::Ptr pC = getPluginCollection();
	CPPUNIT_ASSERT ( !pC->isAllScanned() );
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	pC->com::events::EventSender<com::OnLoadFile>::addEventListener( this );
	pC->com::events::EventSender<com::ScanComplete>::addEventListener( this );
	pC->update( graph->getHostInfo() );
	// compare scanned tree with ExpMap
	ExcpectedFolderMap exp;
	exp = map_list_of ( "root", "testVstFolder" )
					  ( "testVstFolder", "A" )
				      ( "testVstFolder", "B" )
					  ( "testVstFolder", "C'" )
					 // ( "B", "B1" ) <= empty
					  ( "B", "B2" ).to_container(exp);
					// ( "B", "B3" ); <= no plug files

	CPPUNIT_ASSERT ( !exp.empty() );
	checkTree( pC, exp );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 3 + 1, pC->getNumSucceed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)2, pC->getNumFailed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumNotChecked() );
	CPPUNIT_ASSERT ( pC->isAllScanned() );
	//>>>>>>>>>>>>>>>>rescan, excpect fast execute because plugs already in db
	Timer timer;
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT ( !timer.isElapsed(FAST_SCAN_TIME) );
}
//=============================================================================
void PluginCollectionTest::testFastScan() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	using namespace boost::assign;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan empty folders expect 0 plugins
	// setup folders
	PluginCollection::Ptr pC = getPluginCollection();
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	pC->com::events::EventSender<com::OnLoadFile>::addEventListener( this );
	pC->com::events::EventSender<com::ScanComplete>::addEventListener( this );
	resetPluginCollection( graph );
	//>>>>>>>>>>>>>>>>>>>>>>>>scan whole folder-fast. expect 94 potential plugs 
	// setup folders
	settings->clearVSTFolders();
	settings->fastScan = true;
	sambag::com::Location path =  boost::filesystem::absolute("testVstFolder");
	settings->addPluginFolder( path.string() );
	// start scan
	Timer timer;
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT ( !timer.isElapsed(FAST_SCAN_TIME) );
	settings->fastScan = false;
	//checkTree ( pC );
	// compare scanned tree with ExpMap
	ExcpectedFolderMap exp;
	exp = map_list_of ( "root", "testVstFolder" )
					  ( "testVstFolder", "A" )
				      ( "testVstFolder", "B" )
					  ( "testVstFolder", "C'" )
					  // ( "B", "B1" ) <= empty
					  ( "B", "B2" )
					  ( "B", "B3" ).to_container(exp); // <= no plug files but not checked

	CPPUNIT_ASSERT ( !exp.empty() );
	checkTree( pC, exp );
	CPPUNIT_ASSERT ( exp.empty() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumSucceed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumFailed() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 3 + 3, pC->getNumNotChecked() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>check pluginfo list
	PluginCollection::PluginInfoList pL;
	sambag::com::Location pathA = sambag::com::Location( path.string() + "/A" );
	pC->getPlugInfoList( pC->getFolder( pathA ), pL, true );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION, pL.size() );
}
//=============================================================================
// Database has to recognize subfolders as a part of an parent Folder.
void PluginCollectionTest::testFolderIntegrity1(){
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>open corrupt database file
	sambag::com::Location pathA =  boost::filesystem::absolute("testVstFolder");
	sambag::com::Location db = settings->getPlugCollectionDumpFilename();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>copy corrupt file to database file
	sambag::com::Location corruptDatabase =  boost::filesystem::absolute("testVstFolder/corrupt_database");
	if ( exists( db ) ) remove(db);
	copy ( corruptDatabase, db );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>start scan
	PluginCollection::Ptr pC = getPluginCollection();
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	pC->com::events::EventSender<com::OnLoadFile>::addEventListener( this );
	pC->com::events::EventSender<com::ScanComplete>::addEventListener( this );
	settings->clearVSTFolders();
	settings->addPluginFolder( pathA.string() );
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 3 + 1, pC->getNumSucceed() );
	//>>>>>>>>>>>>>>>>has to throw something like "given folder == sub folder"
	sambag::com::Location pathB =  boost::filesystem::absolute("testVstFolder/A");
	settings->addPluginFolder( pathA.string() );
	CPPUNIT_ASSERT_THROW ( 
		settings->addPluginFolder( pathB.string() ), 
		com::ppiError::SettingsException 
	);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>detected bug during port. test:
	// when a given(settings) folder was previously a subfolder and the
	// scanned data is still in the db then the whole content dissapears.
	// because after scan unused folders will be removed => parent folder
	// of given folder => given folder also removed ( foreign key )
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	settings->clearVSTFolders();
	settings->addPluginFolder( pathB.string() );
	Timer t;
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION, pC->getNumSucceed() );
	CPPUNIT_ASSERT ( !t.isElapsed(FAST_SCAN_TIME) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan testVstFolder/ instead testVstFolder
	pathA =  boost::filesystem::absolute("testVstFolder/");
	settings->clearVSTFolders();
	settings->addPluginFolder( pathA.string() );
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 3 + 1, pC->getNumSucceed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)2, pC->getNumFailed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumNotChecked() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>detected bug during man. test in with GUI:
	// when a given(settings) folder changed to his parent location and the
	// scanned data is still in the db then the parentFolderID of the origin folder
	// won't be updated. so he is shown as root.
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	using namespace sambag::cpsqlite;
	DataBase::Results res;
	DataBase::Executer::Ptr exec = pC->getDataBase()->getExecuter();
	std::string qParentFolderID = "SELECT parentFolderID FROM folders WHERE location='" + pathB.string() + "'";
	std::string query = "SELECT location FROM folders WHERE id=(" + qParentFolderID + ");";
	exec->execute( query, res ); 
	CPPUNIT_ASSERT ( !res.empty() );
	CPPUNIT_ASSERT_EQUAL ( pathA.string(), res[0]->get("location") + "/" );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan empty folders expect 0 plugins
	// setup folders
	settings->clearVSTFolders();
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumSucceed() );
}
//=============================================================================
// Remove File.   Excpect lesser plugins
// Remove Folder. Excpect lesser plugins
void PluginCollectionTest::testFolderIntegrity2(){
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	using namespace boost::filesystem;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>reset PluginCollection
	PluginCollection::Ptr pC = getPluginCollection();
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	resetPluginCollection( graph );
	pC->com::events::EventSender<com::OnLoadFile>::addEventListener( this );
	pC->com::events::EventSender<com::ScanComplete>::addEventListener( this );
	resetPluginCollection( graph );
	////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>setup folders
	settings->clearVSTFolders();
	sambag::com::Location pathA =  boost::filesystem::absolute("testVstFolder");
	sambag::com::Location pathB =  pathA.string() + "/NeuFolder";
	create_directory(pathB);
	string filename1  = string("mda Delay")  + VSTPLUG_EXT;
	string filename2  = string("mda Detune") + VSTPLUG_EXT;
	string filename3  = string("mda Dither") + VSTPLUG_EXT;
	settings->addPluginFolder( pathA.string() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// bug 07-15-2011 :
	// rootfolder not visible: 
	// - scan empty folder
	// - scan filled folder
	// => folder invisible
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan empty folder
	using namespace sambag::cpsqlite;
	using namespace com::sqlcommands;
	pC->update( graph->getHostInfo() );
	DataBase::Results res;
	DataBase::Executer::Ptr exec = pC->getDataBase()->getExecuter();
	std::string qGetNeuFolder = "SELECT * FROM folders WHERE name='NeuFolder';";
	exec->execute( qGetNeuFolder, res );
	try {
		CPPUNIT_ASSERT ( !res.empty() );
		CPPUNIT_ASSERT_EQUAL ( string("0"), res[0]->get(TblFolder::visible()) );
	} catch (...) {
		remove_all ( pathB );
		throw;
	}
	////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>fill new folder
	// TODO: fails on mac; all copies are empty
    copy_file ( pathA.string() + "/A/" + filename1,
			    pathB.string() + "/" + filename1 );
	copy_file ( pathA.string() + "/A/" + filename2,
			    pathB.string() + "/" + filename2 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan
	pC->update( graph->getHostInfo() );
	// get NeuFolder from DB 
	res.clear();
	exec->execute( qGetNeuFolder, res );
	try {
		// NeuFolder has to be visible
		CPPUNIT_ASSERT ( !res.empty() );
		CPPUNIT_ASSERT_EQUAL ( string("1"), res[0]->get(TblFolder::visible()) );

		CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 3 + 3, pC->getNumSucceed() );
		CPPUNIT_ASSERT_EQUAL ( (size_t)2, pC->getNumFailed() );
		CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumNotChecked() );
	} catch (...) {
		remove_all ( pathB );
		throw;
	}
	////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>update file 2
	remove (  pathB.string() + "/" + filename2 );
	copy_file ( pathA.string() + "/B/B3/noplug" + VSTPLUG_EXT,
			    pathB.string() + "/" + filename2 );
	pC->update( graph->getHostInfo() );
	try {
		CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 3 + 2, pC->getNumSucceed() );
		CPPUNIT_ASSERT_EQUAL ( (size_t)2 + 1, pC->getNumFailed() );
		CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumNotChecked() );
	} catch (...) {
		remove_all ( pathB );
		throw;
	}
	////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>remove file2
	remove ( pathB.string() + "/" + filename2 );
	///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan
	pC->update( graph->getHostInfo() );
	try {
		CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 3 + 2, pC->getNumSucceed() );
		CPPUNIT_ASSERT_EQUAL ( (size_t)2, pC->getNumFailed() );
		CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumNotChecked() );
	} catch (...) {
		remove_all ( pathB );
		throw;
	}
	////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>remove directory
	remove_all ( pathB );
	///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 3 + 1, pC->getNumSucceed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)2, pC->getNumFailed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumNotChecked() );
	
}
//=============================================================================
// Database have to refind a plugin when location changed.
void PluginCollectionTest::testPortability() {
//=============================================================================
    throw std::runtime_error("need to be reimplemented.");
	/*
    using namespace std;
	using namespace com;
	using namespace processing;
	static const string PLUGIN_LOACTION_1 = string("/mda Overdrive") + VSTPLUG_EXT; 
	static const string PLUGIN_LOACTION_2 = string("/mda Overdrive_renamed") + VSTPLUG_EXT; 
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>set/scan folder(A). 
	// setup folders
	settings->clearVSTFolders();
	sambag::com::Location path =  boost::filesystem::absolute("testVstFolder/A");
	sambag::com::Location plugLocation = path.string() + PLUGIN_LOACTION_1;
	settings->addPluginFolder( path.string() );
	// start scan
	PluginCollection::Ptr pC = getPluginCollection();
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	pC->com::events::EventSender<com::OnLoadFile>::addEventListener( this );
	pC->com::events::EventSender<com::ScanComplete>::addEventListener( this );
	pC->update( graph->getHostInfo() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>getPlugin_1 has to be found
    PluginInfo pluginInfo;
    pluginInfo.location = plugLocation.string();
    pC->restorePluginInfo ( graph->getHostInfo(), pluginInfo );
	plugLocation = path.string() + PLUGIN_LOACTION_1;
	CPPUNIT_ASSERT_EQUAL ( plugLocation.string(), plug->getLocation() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>clear folders
	settings->clearVSTFolders();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan folder(B). 
	pC->com::events::EventSender<com::OnLoadFile>::addEventListener( this );
	pC->com::events::EventSender<com::ScanComplete>::addEventListener( this );
	pC->update( graph->getHostInfo() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>re-set folders
	settings->clearVSTFolders();
	path =  boost::filesystem::absolute("testVstFolder/B");
	settings->addPluginFolder( path.string() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan folder(B). 
	pC->com::events::EventSender<com::OnLoadFile>::addEventListener( this );
	pC->com::events::EventSender<com::ScanComplete>::addEventListener( this );
	pC->update( graph->getHostInfo() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>getPlugin_1 has to be found 
	plug = pC->restorePlugNode ( graph->getHostInfo(), pluginInfo );
	CPPUNIT_ASSERT ( plug );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>re-set folders
	settings->clearVSTFolders();
	path =  boost::filesystem::absolute("testVstFolder/B/B2");
	settings->addPluginFolder( path.string() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan folder(B2). 
	pC->com::events::EventSender<com::OnLoadFile>::addEventListener( this );
	pC->com::events::EventSender<com::ScanComplete>::addEventListener( this );
	pC->update( graph->getHostInfo() );
	//>>>>>>>>>>>>>>>>>>>>>>getPlugin_1 has to be found as PLUGIN_LOACTION_2
	plugLocation = path.string() + PLUGIN_LOACTION_2;
	CPPUNIT_ASSERT_EQUAL ( plugLocation.string(), plug->getLocation() );*/
}
//=============================================================================
void PluginCollectionTest::testMultipleDirectories() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>set two folder
	// 1. testVstFolder\B\B2 ( mda coll. )
    // 2. testVstFolder\B\B1 ( empty folder scanning should be fast )
	// 2. testVstFolder\A    ( mda coll. )
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>prepare pluginCollection
	PluginCollection::Ptr pC = getPluginCollection();
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	pC->com::events::EventSender<com::OnLoadFile>::addEventListener( this );
	pC->com::events::EventSender<com::ScanComplete>::addEventListener( this );
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>setup folders
	settings->clearVSTFolders();
	resetPluginCollection(graph);
	sambag::com::Location pathB1 = boost::filesystem::absolute("testVstFolder/B/B1");
	sambag::com::Location pathB2 = boost::filesystem::absolute("testVstFolder/B/B2");
	sambag::com::Location pathA = boost::filesystem::absolute("testVstFolder/A");
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add B2
	// detected bug:
	// path comparisation in isAllScanned failed because db saves "/" 
	// and isAllScanned searchs for "\".
	// directory_string returns windows path style (when runing in windows)
	settings->addPluginFolder( pathB2.string() );
	CPPUNIT_ASSERT ( !pC->isAllScanned() );
	// start scan
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION, pC->getNumSucceed() );
	CPPUNIT_ASSERT ( pC->isAllScanned() );
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add B1
	settings->addPluginFolder( pathB1.string() );
	CPPUNIT_ASSERT ( !pC->isAllScanned() );
	// start scan
	Timer timer;
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT ( !timer.isElapsed(FAST_SCAN_TIME) ); 
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION, pC->getNumSucceed() );
	CPPUNIT_ASSERT ( pC->isAllScanned() );
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add A
	settings->addPluginFolder( pathA.string() );
	CPPUNIT_ASSERT ( !pC->isAllScanned() );
	// start scan
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 2, pC->getNumSucceed() );
	CPPUNIT_ASSERT ( pC->isAllScanned() );
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan empty dir
	settings->clearVSTFolders();
	// start scan
	CPPUNIT_ASSERT ( !pC->isAllScanned() );
	pC->update( graph->getHostInfo() );
	CPPUNIT_ASSERT ( pC->isAllScanned() );
}
//=============================================================================
// #416: Plugin folders hierarchy misinterpreted.
void PluginCollectionTest::testIssue416() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	std::list<std::string> dos = boost::assign::list_of
        ("VSTPlugins-32")
        ("VSTPlugins-32-only")
        ("VSTPlugins-32-only_JBto-64")
        ("VSTPlugins-32-Waves")
        ("VSTPlugins-64")
        ("VSTPlugins-64-JBto-32")
        ("VSTPlugins-64-Waves")
        ("VSTPlugins-REMOVED")
        ("VSTPlugins_Special")
        //
        ("workspace/vsts")
        ("workspace/vsts2")
        ;

	std::list<std::string> donts = boost::assign::list_of
        ("VSTPlugins-32/subfolder")
        //
        ("workspace/vsts/extra3")
        ("workspace")
        ("workspace/")
        ;

	std::list<std::string> ignores = boost::assign::list_of
        ("VSTPlugins-32")
        ("workspace/vsts")
        ("")
        ;


   
    BOOST_FOREACH( const std::string &path, dos ) {
        boost::filesystem::create_directories(path);
        CPPUNIT_ASSERT(settings->addPluginFolder(path));
    }
    
    
    BOOST_FOREACH( const std::string &path, donts ) {
        boost::filesystem::create_directories(path);
        CPPUNIT_ASSERT_THROW_MESSAGE (
            path + " should raise an error.",
            settings->addPluginFolder( path ),
            com::ppiError::SettingsException 
        );
    }

    BOOST_FOREACH( const std::string &path, ignores ) {
        CPPUNIT_ASSERT ( !settings->addPluginFolder(path) );
    }
    
    // clean up
    BOOST_FOREACH( const std::string &path, dos ) {
        boost::filesystem::remove_all(path);
        settings->removePluginFolder(path);
    }
}
} // namespace tests
