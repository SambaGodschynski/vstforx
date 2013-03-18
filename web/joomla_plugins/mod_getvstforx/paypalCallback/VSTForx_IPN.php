<?php

require "paypal.class/paypal.class.php";
////////////////////////////////////////////////////////////////////////////////
// BEGIN CONFIG
////////////////////////////////////////////////////////////////////////////////
/**
 * get joomla access.
 * altenative: impl. as extra joomla-module. 
 */
// Definiere J!
define( '_JEXEC', 1 );
define( 'NOT_SETTED', '##variable_not_setted++' );
// J! Pfad setzen - BITTE ANPASSEN !!!
define( 'JPATH_BASE', '../i' );
define( 'DS', DIRECTORY_SEPARATOR );
// Dateien des J! Frameworks einbinden
require_once ( JPATH_BASE.DS.'includes'.DS.'defines.php' );
require_once ( JPATH_BASE.DS.'includes'.DS.'framework.php' );  	
defined('_JEXEC') OR defined('_VALID_MOS') OR die( "Direct Access Is Not Allowed" );

$p = new paypal_class;
/**
 * !!!!!!!!!!
 * consider line 186 in paypal.class.php before switching into realmode
 * !!!!!!!!!!
 */
$p->paypal_url = 'https://www.sandbox.paypal.com/cgi-bin/webscr';

////////////////////////////////////////////////////////////////////////////////
// END CONFIG
////////////////////////////////////////////////////////////////////////////////

function getDB() {
	$mainframe =& JFactory::getApplication('site');
	$db = JFactory::getDBO();
	if (!$db) {
		error ("Database query failed.");
	}	
	return $db;
}

if ($p->validate_ipn()) {

	if($p->ipn_data['payment_status']!='Completed') {
		error("payment failed.");
		return;
	}
	
	$db = getDB();

	$amount = $p->ipn_data['mc_gross'] - $p->ipn_data['mc_fee'];
	$query = "INSERT INTO `usr_web22_1`.`frx_selled` (
				`juser` ,
				`transaction_id` ,
				`email` ,
				`amount` ,
				`productid`,
				`rquest`
				) VALUES (
					" . (int)$p->ipn_data['custom'] . ",
					" . $db->quote($p->ipn_data['txn_id'], $link). ",
					" . $db->quote($p->ipn_data['payer_email'], $link). ",
					" . (float)$amount . ",
					" . $db->quote($p->ipn_data['item_number'], $link). ",
					" . $db->quote(http_build_query($_POST), $link). "
				);";
	$db->setQuery($query);
	if ( !$db->query() ) {
		error( "Database query failed: " . $db->getErrorMsg() );	
	}	
}


function error($msg) {
/*
0	message wird an das Log-Subsystem von PHP gesendet
1	message wird via E-Mail an die Adresse gesendet,
2	Diese Option ist nicht mehr verfügbar.
3	message wird an die Datei destination angefügt. 
    Ein Zeilenumbruch wird nicht automatisch an das Ende des message-Strings angehängt.
4	message wird direkt zum SAPI-Logging-Handler gesendet.
*/
	$type = 3;
	$d = date("d.m.Y");
	$t = date("H:i");	
	error_log($d.', '.$t.':: '.$msg."\n", $type, "transactionResponseErr.log");
}
?>


