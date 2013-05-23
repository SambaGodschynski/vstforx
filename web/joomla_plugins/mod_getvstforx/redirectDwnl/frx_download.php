<?php 

/**
 * get joomla access.
 * altenative: impl. as extra joomla-module. 
 */
// Definiere J!
define( '_JEXEC', 1 );
define( 'NOT_SETTED', '##variable_not_setted++' );
// J! Pfad setzen - BITTE ANPASSEN !!!
define( 'JPATH_BASE', '.' );
define( 'DS', DIRECTORY_SEPARATOR );
// Dateien des J! Frameworks einbinden
require_once ( JPATH_BASE.DS.'includes'.DS.'defines.php' );
require_once ( JPATH_BASE.DS.'includes'.DS.'framework.php' );  	
defined('_JEXEC') OR defined('_VALID_MOS') OR die( "Direct Access Is Not Allowed" );
// piwik
require_once "../FrxPiwikTracker.php";
PiwikTracker::$URL = 'http://www.4divisions.com/piwik';


function trackDownload($file) {
	$piwikTracker = new PiwikTracker( $idSite = 1, PiwikTracker::$URL );
	$piwikTracker->doTrackAction(urlencode($file), 'download');
}

/**
 * @param 2=old beta
 *        3=VSTForx
 */
function getFilename($downloads_id) {
	$mainframe =& JFactory::getApplication('site');
	$db = JFactory::getDBO();
	if (!$db) {
		throw new Exception( "Datbase access failed." );	
	}	
	$user = JFactory::getUser();
	$userid = 0;
	$default_user = 0;
	if (!$user->guest) {
		$userid = $user->id;	
	}
	$query = "SELECT frx_downloads.path, frx_downloads.filename
		FROM frx_downloads
		WHERE productid IN (
			SELECT productid FROM frx_selled WHERE frx_selled.juser=". $db->quote($userid) ."		
			OR frx_selled.juser=". $db->quote($default_user) ."
		) AND id=". $db->quote($downloads_id) ."
	";
	$db->setQuery($query);
	if ( !$db->query() ) {
		throw new Exception( "Database query failed."); //  : " . $db->getErrorMsg() );	
	}	
	$res = $db->loadRow();
	if (!$res) {
		return null;
	}
	return $res;
}

function getIP() {
	if ( !isset($_SERVER['HTTP_X_FORWARDED_FOR']) ) {
		if ( isset($_SERVER['REMOTE_ADDR']) ) {
			return $_SERVER['REMOTE_ADDR'];
		}
	}
	else {
		return $_SERVER['HTTP_X_FORWARDED_FOR'];
	}
	return "?";
}

function save_user($downloads_id) {
	$mainframe =& JFactory::getApplication('site');
	$db = JFactory::getDBO();
	if (!$db) {
		throw new Exception( "Datbase access failed." );	
	}	
	$user = JFactory::getUser();
	$userid = 0;
	$default_user = 0;
	if (!$user->guest) {
		$userid = $user->id;	
	}
	$ip = getIP();
	$query = "INSERT INTO frx_juser_download (`juser`, `download`, `date`, `ip`) 
		VALUES (" . $db->quote($userid) . ", 
				" . $db->quote($downloads_id) . ", 
				CURRENT_TIMESTAMP,
				" . $db->quote($ip) . "
	);";
	$db->setQuery($query);
	if ( !$db->query() ) {
		throw new Exception( "Database query failed."); //  : " . $db->getErrorMsg() );	
	}	
}


$path = '../binary/';

$downloads_id = $_POST['dst'];
try {
	save_user($downloads_id);
	$rq=getFilename($downloads_id);
	$path.=$rq[0];
	$file= $rq[1];
	trackDownload("http://vstforx.de/downloads/".$file);
} catch (Exception $e) {
	echo "Exception: ". $e->getMessage();
	die();
}
if (!$file) {
	die();
}

if ( !file_exists($path.$file) ) {
    echo $file. " not found!";
    die();
}


header("Content-type: application/octet-stream");

// Anstatt "attachment" kann auch "inline" gewählt werden, damit der Browser
// die Datei normal darstellen kann und nicht immer der "Speichern unter"-Dialog erscheint
header("Content-disposition: inline; filename=".$file);
header("Content-Length: ".filesize( $path.$file ) );
readfile( $path.$file );
?>


