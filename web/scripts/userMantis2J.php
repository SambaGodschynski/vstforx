<?php

/**
 * get joomla access.
 * altenative: impl. as extra joomla-module. 
 */
// Definiere J!
define( '_JEXEC', 1 );
define( 'NOT_SETTED', '##variable_not_setted++' );
// J! Pfad setzen - BITTE ANPASSEN !!!
define( 'JPATH_BASE', '../.' );
define( 'DS', DIRECTORY_SEPARATOR );
// Dateien des J! Frameworks einbinden
require_once ( JPATH_BASE.DS.'includes'.DS.'defines.php' );
require_once ( JPATH_BASE.DS.'includes'.DS.'framework.php' );  	
defined('_JEXEC') OR defined('_VALID_MOS') OR die( "Direct Access Is Not Allowed" );

jimport( 'joomla.environment.request' );
$mainframe =& JFactory::getApplication('site');
$mainframe->initialise();
jimport('joomla.user.helper');


$sbj = "New VSTForx Version Available CORRECTION";
$txt = 'I did a stupid mistake with the account information, I am sorry!
Below you will find a working version: 

Hello $NAME,

VSTForx has a new GUI and is almost done. It will be released on June 1, 2013 
with a final price of 15,00 EUR. Until then, for all people which are 
interested in supporting VSTForx, a special offer is available:

  - Save 20%!
  - Get all future versions for free!
  - Get all future extensions for free!

Find more Information under the following link:
http://www.vstforx.de

In order to accept this offer you can use an account which was created for you:

username: $USR
password: $PWD

please visit:
http://www.vstforx.de/index.php/my-account to login and to update your profile.

Best Regards.

Johannes Unger
Petersburger Str. 95
10247 Berlin
+49176 76825783
www.vstforx.de
johannes.unger@vstforx.de
';

function getDB() {
	$mainframe =& JFactory::getApplication('site');
	$db = JFactory::getDBO();
	if (!$db) {
		error ("Database query failed.");
		die;
	}	
	return $db;
}

function _sendEmailImpl($mail, $sbj, $body) {
	$mailer =& JFactory::getMailer();
	$config =& JFactory::getConfig();
	$sender = array( 
	    $config->getValue( 'config.mailfrom' ),
	    $config->getValue( 'config.fromname' ) 
	);
	$mailer->setSender($sender);
 	$mailer->addRecipient($mail);
	$mailer->setSubject( $sbj );
	$mailer->setBody($body);
	$send =& $mailer->Send();
	if ( $send !== true ) {
		throw new Exception('Error sending email: ' . $mail);
	}
	echo "MAIL TO $mail sent. <br />";
}

function addJoomlaUser($name, $username, $email) {
	$password = JUserHelper::genRandomPassword(8);
    $data = array(
        "name"=>$name,
        "username"=>$username,
        "password"=>$password,
        "password2"=>$password,
        "email"=>$email,
        "block"=>0,
        "groups"=>array("2")
    );

    $user = new JUser;
    //Write to database
    if(!$user->bind($data)) {
        throw new Exception("Could not bind data. Error: " . $user->getError());
    }
    if (!$user->save()) {
        throw new Exception("Could not save user. Error: " . $user->getError());
    }
	global $txt, $sbj;
	$_txt = str_replace('$NAME', $name, $txt);
	$_txt = str_replace('$USR', $username, $_txt);
	$_txt = str_replace('$PWD', $password, $_txt);
	_sendEmailImpl($email, $sbj, $_txt);	

	return $user->id;
}
$db = getDB();
$db->setQuery("SELECT * FROM mantis_user_table WHERE mantis_user_table.login_count > 0
			   AND mantis_user_table.email NOT IN (
  			   SELECT j16_users.email FROM j16_users);");
if ( !$db->query() ) {
	error( "Database query failed: " . $db->getErrorMsg() );	
}
$res = $db->loadAssocList();
echo sizeof($res);
foreach( $res as $x ) {
	try {
		if ($x['realname']=="") {
			$x['realname'] = $x['username'];		
		}
		addJoomlaUser($x['realname'], $x['username'], $x['email']);
	} catch (Exception $e) {
		echo $x['email'] . " failed. <br />";
		continue;	
	}
}
?>
