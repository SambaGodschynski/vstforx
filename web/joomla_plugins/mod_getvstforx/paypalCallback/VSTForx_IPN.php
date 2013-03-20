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
//$p->paypal_url = 'https://www.paypal.com/cgi-bin/webscr';


////////////////////////////////////////////////////////////////////////////////
// END CONFIG
////////////////////////////////////////////////////////////////////////////////

function getDB() {
	$mainframe =& JFactory::getApplication('site');
	$db = JFactory::getDBO();
	if (!$db) {
		error ("Database query failed.");
		die;
	}	
	return $db;
}

function query($db, $q) {
	$db->setQuery($q);
	if ( !$db->query() ) {
		error( "Database query failed: " . $db->getErrorMsg() );	
	}
	return $db->loadAssocList();
}

function getDetails($details) {
	$name = $details->ipn_data['item_name'];
	$am = $details->ipn_data['mc_gross'];
	$tx = $details->ipn_data['txn_id'];
	$em = $details->ipn_data['payer_email'];
	$dt = $details->ipn_data['payment_date'];
	return "product: $name 
Price: $am 
Payment Date: $dt
Payer EMail: $em
Paypal Transaction Id: $tx
";
}

function sendEmail($juser, $prodid, $details) {
	$db = getDB();
	$q = "SELECT * FROM frx_selled_response_mail 
		  WHERE frx_selled_response_mail.productid = " . $db->quote($prodid) . "
	;";
	$res = query($db, $q);
	if (sizeof($res)==0) {
		//echo "no content here.";
		return;	
	}
	$res = $res[0];
	$mainframe =& JFactory::getApplication('site');
	$mailer =& JFactory::getMailer();

	$config =& JFactory::getConfig();
	$sender = array( 
	    $config->getValue( 'config.mailfrom' ),
	    $config->getValue( 'config.fromname' ) 
	);
	$mailer->setSender($sender);
	$user =& JFactory::getUser($juser);

	$body = str_replace('$USER', $user->name, $res["text"]);
	$body = str_replace('$DETAILS', getDetails($details), $body);
	

	$recipient = array($user->email, 'selled@vstforx.de');
	if ($user->email == "") {
		error('Error sending email to juser_id: ' . $juser);
		return;
	}
 	$mailer->addRecipient($recipient);
	$mailer->setSubject( $res["subject"] );
	$mailer->setBody($body);
	$send =& $mailer->Send();
	if ( $send !== true ) {
		error('Error sending email: ' . $recipient);
	}
}

if ($p->validate_ipn()) {

	if($p->ipn_data['payment_status']!='Completed') {
		error("payment failed.");
		return;
	}
	
	$juser = (int)$p->ipn_data['custom'];
	if ($juser==0) {
		error("tried handle user '0'.");
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
	query($db, $query);
	sendEmail((int)$p->ipn_data['custom'], $p->ipn_data['item_number'], $p);
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


