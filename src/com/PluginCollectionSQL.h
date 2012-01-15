// INSERTED BY PluginCollection.cpp ONLY!
#include <string>
#include <sstream>
#include <sambag/cpsqlite/DataBase.hpp>
#include <list>
#include "com/one4All.h" 
#include <time.h>

namespace com {
namespace sqlcommands {
typedef sambag::cpsqlite::DataBase::Int Int;
typedef sambag::cpsqlite::DataBase::Path Path;
typedef list<Path> PathList;
using namespace std;
//============================================================================================================
struct TblLastScan {
//============================================================================================================
	//--------------------------------------------------------------------------------------------------------
	static string tblName() { return "currentscan"; }
	//--------------------------------------------------------------------------------------------------------
	static string scanstamp() { return "scanstamp"; }
	//--------------------------------------------------------------------------------------------------------
	static string create () {
		return string ("CREATE TABLE IF NOT EXISTS currentscan ( ") +
		"scanstamp INTEGER UNSIGNED NULL);";
	}
	//--------------------------------------------------------------------------------------------------------
	static string getScanStamp() {
		return string("SELECT * FROM currentscan;");
	}
	//--------------------------------------------------------------------------------------------------------
	static string insertScanStamp( time_t timestamp ) {
		stringstream ss;
		ss<<"INSERT INTO currentscan (scanstamp) VALUES("<<timestamp<<");";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string updateScanStamp( time_t timestamp ) {
		stringstream ss;
		ss<<"UPDATE currentscan SET scanstamp = "<<timestamp<<";";
		return ss.str();
	}

};
//============================================================================================================
struct TblFolder {
//============================================================================================================
	//--------------------------------------------------------------------------------------------------------
	static string tblName() { return "folders"; }
	//--------------------------------------------------------------------------------------------------------
	static string root() { return "root"; }
	//--------------------------------------------------------------------------------------------------------
	static string name() { return "name"; }
	//--------------------------------------------------------------------------------------------------------
	static string id() { return "id"; }
	//--------------------------------------------------------------------------------------------------------
	static string location() { return "location"; }
	//--------------------------------------------------------------------------------------------------------
	static string scanstamp() { return "scanstamp"; }
	//--------------------------------------------------------------------------------------------------------
	static string visible() { return "visible"; }
	//--------------------------------------------------------------------------------------------------------
	static string parentFolderID() { return "parentFolderID"; }
	//--------------------------------------------------------------------------------------------------------
	static string create () {
		return string ("CREATE TABLE IF NOT EXISTS folders ( ") +
		"id INTEGER PRIMARY KEY AUTOINCREMENT, " +
		"parentFolderID INTEGER UNSIGNED NULL,"
		"name VARCHAR(45) NOT NULL, " + 
		"location TEXT NOT NULL, " +
		"scanstamp INTEGER UNSIGNED NULL, " +
		"visible BOOLEAN NOT NULL DEFAULT 1, " + // 0=invisible 1=visible
		"FOREIGN KEY(parentFolderID) REFERENCES folders(id) ON UPDATE CASCADE  ON DELETE CASCADE, " +
		"UNIQUE ( location ) );";
	}
	//--------------------------------------------------------------------------------------------------------
	// Use this instead path.string(). Because the db makes differences between folder and folder/. 
	// Removes last character if it "/"
	static string path2String ( const Path &path ) {
		string ret = path.string();
		if ( ret.at( ret.length() - 1 ) == '/' ) {
			return ret.substr( 0, ret.length() - 1 );
		}
		return ret;
	}
	//--------------------------------------------------------------------------------------------------------
	static string insertFolder ( const Path &path, time_t scanstamp, sambag::cpsqlite::ParameterList &out_pL ) 
	{
		using namespace sambag::cpsqlite;
		stringstream ss;
		string parentFolderLoc = path2String( path.parent_path() );

		string subQ = "( SELECT id FROM folders WHERE location = ? )";
		ss<<"INSERT INTO folders ( name, parentFolderID, location, scanstamp ) VALUES ( ?, "<<subQ<<", ?, ? );";
		size_t index = 1;
		out_pL.push_back( TextParameter::create( index++, path.filename() ) );
		out_pL.push_back( TextParameter::create( index++, parentFolderLoc ) );
		out_pL.push_back( TextParameter::create( index++, path2String(path) ) );
		out_pL.push_back( IntParameter::create( index++, scanstamp ) );

		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string updateFolder ( const Path &path, time_t scanstamp, sambag::cpsqlite::ParameterList &out_pL ) 
	{
		using namespace sambag::cpsqlite;
		stringstream ss;
		string parentFolderLoc = path2String( path.parent_path() );
		string q_parentFolderID = " SELECT id FROM folders WHERE location = ? ";
		ss<<"UPDATE folders SET parentFolderID=(" << q_parentFolderID << "), scanstamp=? WHERE location = ?;";
		out_pL.push_back( TextParameter::create( 1, parentFolderLoc ) );
		out_pL.push_back( IntParameter::create( 2, scanstamp ) );
		out_pL.push_back( TextParameter::create( 3, path2String(path) ) );
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string insertFolder ( 
		const Path &path, 
		const Int &parentFolderID, 
		time_t scanstamp,
		sambag::cpsqlite::ParameterList &out_pL ) 
	{
		using namespace sambag::cpsqlite;
		string q = "INSERT INTO folders ( name, parentFolderID, location, scanstamp ) VALUES ( ?, ?, ?, ? );";
		out_pL.push_back( TextParameter::create( 1, path.filename() ) );
		out_pL.push_back( IntParameter::create( 2, parentFolderID ) );
		out_pL.push_back( TextParameter::create( 3, path2String( path ) ) );
		out_pL.push_back( IntParameter::create( 4, scanstamp ) );
		return q;
	}
	//--------------------------------------------------------------------------------------------------------
	static string updateParentFolderID ( const Int &folderID, const Int &newParentFolderID ) {
		stringstream ss;
		ss<<"UPDATE folders SET parentFolderID = "<<newParentFolderID;
		ss<<", scanstamp=(SELECT scanstamp FROM currentscan) WHERE id="<<folderID<<";";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string insertRoot () {
		stringstream ss;
		ss<<"INSERT INTO folders ( name, parentFolderID, location ) VALUES ( 'root', NULL, '/' )";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string getFolder ( const string &name, sambag::cpsqlite::ParameterList &out_pL ) {
		using namespace sambag::cpsqlite;
		stringstream ss;
		ss<<"SELECT * FROM folders WHERE name = ?;";
		out_pL.push_back( TextParameter::create( 1, name ) ); 
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	template < typename PathContainer >
	static string getFolders ( const PathContainer &c, sambag::cpsqlite::ParameterList &out_pL ) {
		using namespace sambag::cpsqlite;
		if ( c.empty() ) return "";
		stringstream ss;
		ss<<"SELECT * FROM folders WHERE location = ?";
		typename PathContainer::const_iterator it = c.begin();
		size_t index = 1;
		out_pL.push_back( TextParameter::create( index++, com::Path(*it++).string() ) ); 
		for ( ; it!=c.end(); ++it ) {
			ss<<" OR location = ?";
			out_pL.push_back( TextParameter::create( index++, com::Path(*it).string() ) ); 
		}
		return ss.str() + ";";
	}
	//--------------------------------------------------------------------------------------------------------
	static string getFolder ( size_t id ) {
		stringstream ss;
		ss<<"SELECT * FROM folders WHERE id = "<<id<<";";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string getFolder ( const Path &location, sambag::cpsqlite::ParameterList &out_pL ) {
		using namespace sambag::cpsqlite;
		stringstream ss;
		string loc = path2String( location );
		ss<<"SELECT * FROM folders WHERE location = ?;";
		out_pL.push_back( TextParameter::create( 1, loc ) ); 
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string getSubFolders ( const Int &folderID, bool getInvisibleFolder = false ) {
		stringstream ss;
		ss<<"SELECT * FROM folders WHERE parentFolderID = "<<folderID;
		if ( !getInvisibleFolder ) {
			ss<<" AND visible = 1";
		}
		ss<<";";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string removeFolder ( const Int &folderID ) {
		stringstream ss;
		ss<<"DELETE FROM folders WHERE id = "<<folderID<<";";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string resetFolderVisibility() {
		return "UPDATE folders SET visible=1;";
	}
	//--------------------------------------------------------------------------------------------------------
	static string setFolderVisible ( const Int &folderID, bool visible ) {
		stringstream ss;
		ss<<"UPDATE folders SET visible = "<< (visible ? "1":"0") <<" WHERE id = "<<folderID<<";";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string getFolder ( 
		const string &name, 
		const Int &parentFolderID, 
		sambag::cpsqlite::ParameterList &out_pL ) 
	{
		using namespace sambag::cpsqlite;
		stringstream ss;
		ss<<"SELECT * FROM folders WHERE parentFolderID = "<<parentFolderID<<" AND name = ?;";
		out_pL.push_back( TextParameter::create( 1, name ) ); 
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string removeUnusedFolders ( time_t scanStamp ) {
		using namespace sambag::cpsqlite;
		stringstream ss; 
		ss<<"DELETE FROM folders WHERE id!=1 AND scanstamp!="<<scanStamp<<";";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string getEmptyFolders () {
		using namespace sambag::cpsqlite;
		/*
			unused folders are:
				all folders except ( all folders without subfolders 
				                     UNION
									 all folders with accessable plugins ) 

			    Assoziativ gesetz fuer Differenzmenge:
				A \ ( B U C ) = A\B\C 
				              
		*/
		return string("SELECT id FROM folders WHERE visible = 1 ") +
			   "EXCEPT " + 
			   "SELECT parentFolderId FROM folders WHERE visible = 1 " +
			   "EXCEPT " +
               "SELECT folderId FROM plugins WHERE access == 0 OR access == 1;";
	}
};
//============================================================================================================
struct TblPlugins {
//============================================================================================================
	//--------------------------------------------------------------------------------------------------------
	static string tblName() { return "plugins"; }
	//--------------------------------------------------------------------------------------------------------
	static string location() { return "location"; }
	//--------------------------------------------------------------------------------------------------------
	static string name() { return "name"; }
	//--------------------------------------------------------------------------------------------------------
	static string access() { return "access"; }
	//--------------------------------------------------------------------------------------------------------
	static string uid() { return "uid"; }
	//--------------------------------------------------------------------------------------------------------
	static string isSynth() { return "is_synth"; }
	//--------------------------------------------------------------------------------------------------------
	static string pluginType() { return "plugin_type"; }
	//--------------------------------------------------------------------------------------------------------
	static string timestamp() { return "timestamp"; } // last changed on filesystem
	//--------------------------------------------------------------------------------------------------------
	static string scanstamp() { return "scanstamp"; }
	//--------------------------------------------------------------------------------------------------------
	static string id() { return "id"; }
	//--------------------------------------------------------------------------------------------------------
	static string folderID() { return "folderID"; }
	//--------------------------------------------------------------------------------------------------------
	static string create () {
		return string ("CREATE TABLE IF NOT EXISTS plugins ( ") +
		"id INTEGER PRIMARY KEY AUTOINCREMENT, " +
		"location TEXT NOT NULL, " +
		"uid INTEGER NOT NULL, " + 
		"plugin_type INTEGER NOT NULL DEFAULT 0, " + 
		"is_synth INTEGER NOT NULL DEFAULT 0, " + 
		"folderID INTEGER UNSIGNED NULL,"
		"name VARCHAR(50) NOT NULL, " + 
		"access INTEGER DEFAULT 0, " + // 0=NOT_CHECKED; 1=SUCCEED; 2=FAILED
		"timestamp INTEGER NOT NULL DEFAULT 0, " + 
		"scanstamp INTEGER NOT NULL DEFAULT 0, " + 
		"FOREIGN KEY(folderID) REFERENCES folders(id) ON UPDATE CASCADE  ON DELETE CASCADE," +
		"UNIQUE ( location ) );";
	}
	//--------------------------------------------------------------------------------------------------------
	static string insertPlugin ( const string &location, 
		                         const string &name,
								 const Int &uid,
								 const Int &isSynth,
								 const Int &plugType,
								 const Int &folderID,
								 const time_t &timestamp,
								 const time_t &scanstamp,
								 const Int &access,
								 sambag::cpsqlite::ParameterList &pL ) 
	{
		using namespace sambag::cpsqlite;
		string q = string("INSERT INTO plugins(location, name, folderID, access, ") + 
			"timestamp, scanstamp, uid, plugin_type, is_synth)";
		q += " VALUES (?,?,?,?,?,?,?,?,?);";
		size_t index = 1;
		pL.push_back( TextParameter::create( index++, location ) );
		pL.push_back( TextParameter::create( index++, name ) );
		pL.push_back( IntParameter::create( index++, folderID ) );
		pL.push_back( IntParameter::create( index++, access ) );
		pL.push_back( Int64Parameter::create( index++, timestamp ) );
		pL.push_back( Int64Parameter::create( index++, scanstamp ) );
		pL.push_back( IntParameter::create( index++, uid ) );
		pL.push_back( IntParameter::create( index++, plugType ) );
		pL.push_back( IntParameter::create( index++, isSynth ) );
		return q;
	}
	//--------------------------------------------------------------------------------------------------------
	static string updatePlugin ( const string &location, 
		                         const string &name,
								 const Int &uid,
								 const Int &isSynth,
								 const Int &plugType,
								 const time_t &timestamp,
								 const time_t &scanstamp,
								 const Int &access,
								 sambag::cpsqlite::ParameterList &pL ) 
	{
		using namespace sambag::cpsqlite;
		string q = string("UPDATE plugins SET name=?, access=?, timestamp=?, ") +
			      "scanstamp=?, uid=?, plugin_type=?, is_synth=? WHERE location=?";
		size_t index = 1;
		pL.push_back( TextParameter::create( index++, name ) );
		pL.push_back( IntParameter::create( index++, access ) );
		pL.push_back( Int64Parameter::create( index++, timestamp ) );
		pL.push_back( Int64Parameter::create( index++, scanstamp ) );
		pL.push_back( IntParameter::create( index++, uid ) );
		pL.push_back( IntParameter::create( index++, plugType ) );
		pL.push_back( IntParameter::create( index++, isSynth ) );
		pL.push_back( TextParameter::create( index++, location ) );
		return q;
	}
	//--------------------------------------------------------------------------------------------------------
	static string updateScanStamp ( const string &location, 
		                            const time_t &scanstamp,  
									sambag::cpsqlite::ParameterList &pL )
	{
		using namespace sambag::cpsqlite;
		string q("UPDATE plugins SET scanstamp=? WHERE location=?;");
		size_t index = 1;
		pL.push_back( Int64Parameter::create( index++, scanstamp ) );
		pL.push_back( TextParameter::create( index++, location ) );
		return q;
	}
	//--------------------------------------------------------------------------------------------------------
	static string getPlugin ( const string &location, sambag::cpsqlite::ParameterList &pL ) {
		using namespace sambag::cpsqlite;
		string q = "SELECT * FROM plugins WHERE location = ?;";
		pL.push_back( TextParameter::create( 1, location ) );
		return q;
	}
	//--------------------------------------------------------------------------------------------------------
	static string getNumPlugins () 
	{
		stringstream ss;
		ss<<"SELECT COUNT(id) FROM plugins;";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string getPluginsByName ( const string &name, sambag::cpsqlite::ParameterList &out_pL ) 
	{
		using namespace sambag::cpsqlite;
		string q("SELECT * FROM plugins WHERE name=?;");
		out_pL.push_back( TextParameter::create( 1, name ) );
		return q;
	}
	//--------------------------------------------------------------------------------------------------------
	static string getPluginsByUid ( const Int &uid, sambag::cpsqlite::ParameterList &out_pL ) 
	{
		using namespace sambag::cpsqlite;
		string q("SELECT * FROM plugins WHERE uid=?;");
		out_pL.push_back( IntParameter::create( 1, uid ) );
		return q;
	}
	//--------------------------------------------------------------------------------------------------------
	// select plugins in folder where access state = succeed if showAll = false.
	// select plugins in folder and nevermind access state if showAll = true.
	static string getPluginsByFolder ( const Int &folderID, bool showall ) 
	{
		using namespace sambag::cpsqlite;
		stringstream ss;
		ss<<"SELECT * FROM plugins WHERE folderID="<<folderID;
		if ( showall ) return ss.str() + ";";
		return ss.str() + " AND access = 1;"; // 1 == succeed
	}
	//--------------------------------------------------------------------------------------------------------
	static string getNumPluginsSucceed () 
	{
		stringstream ss;
		ss<<"SELECT COUNT(id) FROM plugins WHERE access = 1;";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string getNumPluginsFailed () 
	{
		stringstream ss;
		ss<<"SELECT COUNT(id) FROM plugins WHERE access = 2;";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string getNumPluginsNotChecked () 
	{
		stringstream ss;
		ss<<"SELECT COUNT(id) FROM plugins WHERE access = 0;";
		return ss.str();
	}
	//--------------------------------------------------------------------------------------------------------
	static string removeUnusedPlugins ( time_t scanstamp ) {
		// removes all plugins plugin::scanstamp!=scanstamp 
		stringstream ss;
		ss<<"DELETE FROM plugins WHERE scanstamp!="<<scanstamp;
		return ss.str();
	}
};

} // namespace
} // namespace
