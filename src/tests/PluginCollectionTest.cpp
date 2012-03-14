/*
 * ===========================================================================================================
 * PluginCollectionTest.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include <cppunit/config/SourcePrefix.h>
#include "PluginCollectionTest.hpp"
#include "com/MyString.h"
#include "processing/processing.h"
#include "processing/ConcreteProcessAdapter.h"
#include "com/one4All.h"
#include <iostream>
#include <boost/assign/list_of.hpp>
#include <map>
#include <boost/timer.hpp>
#include "com/PluginCollectionSQL.h"

#if WIN32
#define VSTPLUG_EXT ".dll"
#else __APPLE_CC__
#define VSTPLUG_EXT ".vst"
#endif

#define FAST_SCAN_TIME 12.0 //two seconds

// Registers the fixture into the 'registry'
#ifndef FORX_FASTTEST
//CPPUNIT_TEST_SUITE_REGISTRATION( tests::PluginCollectionTest );
#endif

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
	janitor->hostInfoChanged();
	return graph;
}
//=============================================================================
PluginCollectionTest::PluginCollectionTest() : numHandlerCalled(0) {
//=============================================================================
	dummyFX = new processing::DummyFX ( NULL );
	settings = com::Settings::getSettings();
	settings->fastScan = false;
}
//=============================================================================
PluginCollectionTest::~PluginCollectionTest() {
//=============================================================================
	delete dummyFX;
	
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
	PluginCollection::Ptr pC( PluginCollection::getPluginCollection() );
}
static const char BUSY[] = { '/','-','\\','|' };
static const size_t NUM_BUSYS = sizeof(BUSY) / sizeof(BUSY[0]);
//=============================================================================
void PluginCollectionTest::eventHandler( void *src, const com::OnLoadFile &ev ) {
//=============================================================================
	if ( numHandlerCalled == 0 ) {
		cout<<"  "<<BUSY[0];
		++numHandlerCalled;
		return;
	}
	cout<<"\b"<<BUSY[ (numHandlerCalled++) % NUM_BUSYS ];
}
//=============================================================================
void PluginCollectionTest::eventHandler( void *src, const com::ScanComplete &ev ) {
//=============================================================================
}

/* 
	07-12-2011 Changed because bug: 0000084
	caused by SQL Exception ( ' character in filenames )

   Ordner Struktur:
   ----------------
   \testVstFolder           <- no plugs
   \testVstFolder\A         <- mda collection
   \testVstFolder\B         <- mda collection + no plug files 
   \testVstFolder\C'		<- one plug that filename has special character
   \testVstFolder\B\B1
   \testVstFolder\B\B1\B1_1 <- textfile
   \testVstFolder\B\B2		<- mda collection but overdrive == PLUGIN_LOACTION_1 => renamed
   \testVstFolder\B\B3      <- no plug files

	
	Testplugins = mda PluginCollection = win(31)/mac(36) plugins 
*/


#if WIN32
static const size_t NUM_PLUG_COLLECTION = 31;
#elif __APPLE_CC__
static const size_t NUM_PLUG_COLLECTION = 36;
#endif

//              folder       subfolder
typedef multimap< std::string, std::string > ExcpectedFolderMap; 

//=============================================================================
// after call with ROOT_FOLDER exp has to be empty.
void resetPluginCollection( processing::Graph::Ptr graph ) 
{
	using namespace com;
	using namespace processing;
	Settings::Ptr settings = Settings::getSettings();
	PluginCollection::Ptr pC( PluginCollection::getPluginCollection() );
	Settings::PathnameSet tmp = settings->getPluginDirectoryList();
	settings->clearVSTFolders();
	// scan:
	pC->update( graph.get() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumSucceed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumFailed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumNotChecked() );
	// insert saved directories
	Settings::PathnameSet::const_iterator it = tmp.begin();
	for ( ; it!=tmp.end(); ++it ) {
		settings->addVSTFolder( *it );
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
		boost::tie( expIt, end ) = exp.equal_range( GET_FOLDER_NAME(currFolder) ); // subfolders of currFolder in exp
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
	CPPUNIT_ASSERT( exp.empty() );
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
	sambag::com::Location path = boost::filesystem::complete("testVstFolder");
	settings->addVSTFolder( path.string() );
	// start scan
	PluginCollection::Ptr pC( PluginCollection::getPluginCollection() );
	CPPUNIT_ASSERT ( !pC->isAllScanned() );
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	pC->EventSender<com::OnLoadFile>::addEventListener( this );
	pC->EventSender<com::ScanComplete>::addEventListener( this );
	pC->update( graph.get() );
	// compare scanned tree with ExpMap
	ExcpectedFolderMap exp;
	exp = map_list_of ( "root", "testVstFolder" )
					  ( "testVstFolder", "A" )
				      ( "testVstFolder", "B" )
					  ( "testVstFolder", "C'" )
					 // ( "B", "B1" ) <= empty
					  ( "B", "B2" );
					// ( "B", "B3" ); <= no plug files

	CPPUNIT_ASSERT ( !exp.empty() );
	checkTree( pC, exp );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 3 + 1, pC->getNumSucceed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)2, pC->getNumFailed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumNotChecked() );
	CPPUNIT_ASSERT ( pC->isAllScanned() );
	//>>>>>>>>>>>>>>>>rescan, excpect fast execute because plugs already in db
	boost::timer timer;
	pC->update( graph.get() );
	CPPUNIT_ASSERT ( timer.elapsed() < FAST_SCAN_TIME );
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
	PluginCollection::Ptr pC( PluginCollection::getPluginCollection() );
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	pC->EventSender<com::OnLoadFile>::addEventListener( this );
	pC->EventSender<com::ScanComplete>::addEventListener( this );
	resetPluginCollection( graph );
	//>>>>>>>>>>>>>>>>>>>>>>>>scan whole folder-fast. expect 94 potential plugs 
	// setup folders
	settings->clearVSTFolders();
	settings->fastScan = true;
	sambag::com::Location path =  boost::filesystem::complete("testVstFolder");
	settings->addVSTFolder( path.string() );
	// start scan
	boost::timer timer; 
	pC->update( graph.get() );
	CPPUNIT_ASSERT ( timer.elapsed() < FAST_SCAN_TIME ); 
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
					  ( "B", "B3" ); // <= no plug files but not checked

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
	sambag::com::Location pathA =  boost::filesystem::complete("testVstFolder");
	sambag::com::Location db = settings->getPlugCollectionDumpFilename();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>copy corrupt file to database file
	sambag::com::Location corruptDatabase =  boost::filesystem::complete("testVstFolder/corrupt_database");
	if ( exists( db ) ) remove(db);
	copy_file ( corruptDatabase, db ); 
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>start scan
	PluginCollection::Ptr pC( PluginCollection::getPluginCollection() );
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	pC->EventSender<com::OnLoadFile>::addEventListener( this );
	pC->EventSender<com::ScanComplete>::addEventListener( this );
	settings->clearVSTFolders();
	settings->addVSTFolder( pathA.string() );
	pC->update( graph.get() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 3 + 1, pC->getNumSucceed() );
	//>>>>>>>>>>>>>>>>has to throw something like "given folder == sub folder"
	sambag::com::Location pathB =  boost::filesystem::complete("testVstFolder/A");
	settings->addVSTFolder( pathA.string() );
	CPPUNIT_ASSERT_THROW ( 
		settings->addVSTFolder( pathB.string() ), 
		com::ppiError::SettingsException 
	);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>detected bug during port. test:
	// when a given(settings) folder was previously a subfolder and the
	// scanned data is still in the db then the whole content dissapears.
	// because after scan unused folders will be removed => parent folder
	// of given folder => given folder also removed ( foreign key )
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	settings->clearVSTFolders();
	settings->addVSTFolder( pathB.string() );
	boost::timer t;
	pC->update( graph.get() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION, pC->getNumSucceed() );
	CPPUNIT_ASSERT ( t.elapsed() < FAST_SCAN_TIME ); 
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan testVstFolder/ instead testVstFolder
	pathA =  boost::filesystem::complete("testVstFolder/");
	settings->clearVSTFolders();
	settings->addVSTFolder( pathA.string() );
	pC->update( graph.get() );
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
	pC->update( graph.get() );
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
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>reset PluginCollection
	PluginCollection::Ptr pC( PluginCollection::getPluginCollection() );
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	resetPluginCollection( graph );
	pC->EventSender<com::OnLoadFile>::addEventListener( this );
	pC->EventSender<com::ScanComplete>::addEventListener( this );
	resetPluginCollection( graph );
	////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>setup folders
	settings->clearVSTFolders();
	sambag::com::Location pathA =  boost::filesystem::complete("testVstFolder");
	sambag::com::Location pathB =  pathA.string() + "/NeuFolder";
	create_directory(pathB);
	string filename1  = string("mda Delay")  + VSTPLUG_EXT;
	string filename2  = string("mda Detune") + VSTPLUG_EXT;
	string filename3  = string("mda Dither") + VSTPLUG_EXT;
	settings->addVSTFolder( pathA.string() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// bug 07-15-2011 :
	// rootfolder not visible: 
	// - scan empty folder
	// - scan filled folder
	// => folder invisible
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan empty folder
	using namespace sambag::cpsqlite;
	using namespace com::sqlcommands;
	pC->update( graph.get() );
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
	copy_file ( pathA.string() + "/A/" + filename1,
			    pathB.string() + "/" + filename1 );
	copy_file ( pathA.string() + "/A/" + filename2,
			    pathB.string() + "/" + filename2 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan
	pC->update( graph.get() );
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
	copy_file ( pathA.string() + "/B/B3/keinVstPlugin" + VSTPLUG_EXT,
			    pathB.string() + "/" + filename2 );
	pC->update( graph.get() );
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
	pC->update( graph.get() );
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
	pC->update( graph.get() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 3 + 1, pC->getNumSucceed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)2, pC->getNumFailed() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, pC->getNumNotChecked() );
	
}
//=============================================================================
// Database have to refind a plugin when location changed.
void PluginCollectionTest::testPortability() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	static const string PLUGIN_LOACTION_1 = string("/mda Overdrive") + VSTPLUG_EXT; 
	static const string PLUGIN_LOACTION_2 = string("/mda Overdrive_renamed") + VSTPLUG_EXT; 
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>set/scan folder(A). 
	// setup folders
	settings->clearVSTFolders();
	sambag::com::Location path =  boost::filesystem::complete("testVstFolder/A");
	sambag::com::Location plugLocation = path.string() + PLUGIN_LOACTION_1;
	settings->addVSTFolder( path.string() );
	// start scan
	PluginCollection::Ptr pC( PluginCollection::getPluginCollection() );
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	pC->EventSender<com::OnLoadFile>::addEventListener( this );
	pC->EventSender<com::ScanComplete>::addEventListener( this );
	pC->update( graph.get() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>getPlugin_1 has to be found
	Plugin::Ptr plug = pC->getPlugNode ( graph.get(), plugLocation.string() );
	CPPUNIT_ASSERT ( plug );
	PluginInfo pluginInfo = plug->getPluginInfo();
	plugLocation = path.string() + PLUGIN_LOACTION_1;
	CPPUNIT_ASSERT_EQUAL ( plugLocation.string(), plug->getLocation() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>clear folders
	pC.reset();
	settings->clearVSTFolders();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan folder(B). 
    pC = PluginCollection::getPluginCollection();
	pC->EventSender<com::OnLoadFile>::addEventListener( this );
	pC->EventSender<com::ScanComplete>::addEventListener( this );
	pC->update( graph.get() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>getPlugin_1 cannot found
	plug = pC->restorePlugNode ( graph.get(), pluginInfo );
	CPPUNIT_ASSERT ( !plug );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>re-set folders
	pC.reset();
	settings->clearVSTFolders();
	path =  boost::filesystem::complete("testVstFolder/B");
	settings->addVSTFolder( path.string() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan folder(B). 
    pC = PluginCollection::getPluginCollection();
	pC->EventSender<com::OnLoadFile>::addEventListener( this );
	pC->EventSender<com::ScanComplete>::addEventListener( this );
	pC->update( graph.get() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>getPlugin_1 has to be found 
	plug = pC->restorePlugNode ( graph.get(), pluginInfo );
	CPPUNIT_ASSERT ( plug );
	plugLocation = path.string() + PLUGIN_LOACTION_1;
	CPPUNIT_ASSERT_EQUAL ( plugLocation.string(), plug->getLocation() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>re-set folders
	pC.reset();
	settings->clearVSTFolders();
	path =  boost::filesystem::complete("testVstFolder/B/B2");
	settings->addVSTFolder( path.string() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan folder(B2). 
    pC = PluginCollection::getPluginCollection();
	pC->EventSender<com::OnLoadFile>::addEventListener( this );
	pC->EventSender<com::ScanComplete>::addEventListener( this );
	pC->update( graph.get() );
	//>>>>>>>>>>>>>>>>>>>>>>getPlugin_1 has to be found as PLUGIN_LOACTION_2 
	plug = pC->restorePlugNode ( graph.get(), pluginInfo );
	CPPUNIT_ASSERT ( plug );
	plugLocation = path.string() + PLUGIN_LOACTION_2;
	CPPUNIT_ASSERT_EQUAL ( plugLocation.string(), plug->getLocation() );
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
	PluginCollection::Ptr pC( PluginCollection::getPluginCollection() );
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	pC->EventSender<com::OnLoadFile>::addEventListener( this );
	pC->EventSender<com::ScanComplete>::addEventListener( this );
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>setup folders
	settings->clearVSTFolders();
	resetPluginCollection(graph);
	sambag::com::Location pathB1 = boost::filesystem::complete("testVstFolder/B/B1");
	sambag::com::Location pathB2 = boost::filesystem::complete("testVstFolder/B/B2");
	sambag::com::Location pathA = boost::filesystem::complete("testVstFolder/A");
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add B2
	// detected bug:
	// path comparisation in isAllScanned failed because db saves "/" 
	// and isAllScanned searchs for "\".
	// directory_string returns windows path style (when runing in windows)
	settings->addVSTFolder( pathB2.directory_string() );
	CPPUNIT_ASSERT ( !pC->isAllScanned() );
	// start scan
	pC->update( graph.get() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION, pC->getNumSucceed() );
	CPPUNIT_ASSERT ( pC->isAllScanned() );
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add B1
	settings->addVSTFolder( pathB1.string() );
	CPPUNIT_ASSERT ( !pC->isAllScanned() );
	// start scan
	boost::timer timer;
	pC->update( graph.get() );
	CPPUNIT_ASSERT ( timer.elapsed() < FAST_SCAN_TIME ); 
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION, pC->getNumSucceed() );
	CPPUNIT_ASSERT ( pC->isAllScanned() );
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add A
	settings->addVSTFolder( pathA.string() );
	CPPUNIT_ASSERT ( !pC->isAllScanned() );
	// start scan
	pC->update( graph.get() );
	CPPUNIT_ASSERT_EQUAL ( NUM_PLUG_COLLECTION * 2, pC->getNumSucceed() );
	CPPUNIT_ASSERT ( pC->isAllScanned() );
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>scan empty dir
	settings->clearVSTFolders();
	// start scan
	CPPUNIT_ASSERT ( !pC->isAllScanned() );
	pC->update( graph.get() );
	CPPUNIT_ASSERT ( pC->isAllScanned() );
}
} // namespace tests
