<?php

defined('_JEXEC') or die;
require_once 'database.php';

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
	if ($user->guest == true) {
		return -1;	
	}
	$paysts = getPaymentState($user, "VSTForx");
	if (!$paysts) {
		return 0;	
	} 
	return 1;
}


?>
