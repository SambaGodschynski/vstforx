<?php

defined('_JEXEC') or die;


function getDB() {
	$db = JFactory::getDBO();
	if ($db == null) {
		throw new Exception("Database access failed.");	
	}
	return $db;
}

function processQuery($db, $query) {
	$db->setQuery($query);
	if ( !$db->query() ) {
		throw new Exception( "Database query failed."); //  : " . $db->getErrorMsg() );	
	}	
	$res = $db->loadRowList();
	if (!$res) {
		return null;
	}
	return $res;
}

function getPaymentStates($user) {
	$db = getDB();
	$query = "SELECT * 
    	FROM ".$db->nameQuote('frx_selled')."  
    	WHERE ".$db->nameQuote('juser')." = ".$db->quote($user->id).";
  	";
	return processQuery($db, $query);
}

function getPaymentState($user, $item) {
	$db = getDB();
	$query = "SELECT * 
    	FROM ".$db->nameQuote('frx_selled')."  
    	WHERE ".$db->nameQuote('juser')." = ".$db->quote($user->id)."  
		AND ".$db->nameQuote('item')." = ".$db->quote($item).";
  	";
	return processQuery($db, $query);
}

function hasUserPaid($user) {
	$paysts = getPaymentState($user, "VSTForx");
	if (!$paysts) {
		return 0;	
	} 
	return 1;
}


?>
