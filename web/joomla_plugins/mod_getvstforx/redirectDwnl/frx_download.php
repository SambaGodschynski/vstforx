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


//require_once 'database.php';

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
		throw new Exception( "Database query failed.  : " . $db->getErrorMsg() );	
	}	
	$res = $db->loadRow();
	if (!$res) {
		return null;
	}
	return $res;
}


$path = '../binary/';

$downloads_id = $_POST['dst'];
try {
	$rq=getFilename($downloads_id);
	$path.=$rq[0];
	$file= $rq[1];
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

//$name = "dateiname.zip"; // Name, unter welchem die Datei an den Browser geschickt wird

header("Content-type: application/octet-stream");

// Anstatt "attachment" kann auch "inline" gewählt werden, damit der Browser
// die Datei normal darstellen kann und nicht immer der "Speichern unter"-Dialog erscheint
header("Content-disposition: inline; filename=".$file);
header("Content-Length: ".filesize( $path.$file ) );
readfile( $path.$file );
?>


