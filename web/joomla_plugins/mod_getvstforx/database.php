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
		throw new Exception( "Database query failed.  : " . $db->getErrorMsg() );	
	}	
	$res = $db->loadRowList();
	if (!$res) {
		return null;
	}
	return $res;
}

function getDefaultUser() {
	return 0;
}

?>
