<?php 

$ARMED = 1;
$VERBOSE = 0;

/**
 * get joomla access.
 * altenative: impl. as extra joomla-module. 
 */
// Definiere J!
define( '_JEXEC', 1 );
define( 'NOT_SETTED', '##variable_not_setted++' );
// J! Pfad setzen - BITTE ANPASSEN !!!
define( 'JPATH_BASE', '../' );
define( 'DS', DIRECTORY_SEPARATOR );
// Dateien des J! Frameworks einbinden
require_once ( JPATH_BASE.DS.'includes'.DS.'defines.php' );
require_once ( JPATH_BASE.DS.'includes'.DS.'framework.php' );  	
defined('_JEXEC') OR defined('_VALID_MOS') OR die( "Direct Access Is Not Allowed" );


function getArg($name, $req=true) {
	$res = $_POST[$name];
	if (isset($res) || !$req) {
		return $res;		
	}
	throw new Exception( "missing argument: " . $name  );	
}
function getDB() {
	$mainframe =& JFactory::getApplication('site');
	$db = JFactory::getDBO();
	if (!$db) {
		throw new Exception( "Database query failed.");
	}	
	return $db;
}

function update($db, $id, $introtext, $fulltext) {	
	$query = "UPDATE j16_content SET j16_content.introtext = '" . $introtext .
			  "', j16_content.fulltext = '" . $fulltext . 
			  "' WHERE j16_content.id = ". $db->quote($id) ."; ";
	$db->setQuery($query);
	if ( !$db->query() ) {
		throw new Exception( "Database query failed: " . $db->getErrorMsg() );	
	}	
}

function getTitle($db, $id) {
	$mainframe =& JFactory::getApplication('site');
	$db = JFactory::getDBO();
	if (!$db) {
		throw new Exception( "Database query failed.");
	}	
	
	$query = "SELECT j16_content.title FROM  j16_content WHERE j16_content.id = ". $db->quote($id) ."; ";
	$db->setQuery($query);
	if ( !$db->query() ) {
		throw new Exception( "Database query failed: " . $db->getErrorMsg() );	
	}
	$res = $db->loadRow();
	return $res[0];
}

if ($ARMED == 0) {
	echo "0";	
	return;
}
try {
	$pwd = getArg("mgk");
	if ($pwd != '$$dieter$$') {
		throw new Exception("auth error.");
	} 
	$db = getDB();
	if ( getArg("cmd", false) == "get_title" ) {
		print getTitle($db, getArg("id"));
		return;	
	} 
	update($db, getArg("id"), getArg("introtext"), getArg("fulltext") );
} catch (Exception $e) {
	if ($VERBOSE) {
		echo $e->getMessage();
		return;	
	}
	echo "0";
	return;
}
echo "1";
?>


