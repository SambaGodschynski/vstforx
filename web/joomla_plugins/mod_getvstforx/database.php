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
		throw new Exception( "Database query failed. : " . $db->getErrorMsg() );	
	}	
	$res = $db->loadRowList();
	if (!$res) {
		return null;
	}
	return $res;
}

function processQueryAssoc($db, $query) {
	$db->setQuery($query);
	if ( !$db->query() ) {
		throw new Exception( "Database query failed. : " . $db->getErrorMsg() );	
	}	
	$res = $db->loadAssocList();
	if (!$res) {
		return null;
	}
	return $res;
}

function getDefaultUser() { 
	/*!hardcoded in frx_redirect_download*/
	return 0;
}

?>
