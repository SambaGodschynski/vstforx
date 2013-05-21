<?php
/**
 * @package		Joomla.Site
 * @subpackage	mod_login
 * @copyright	Copyright (C) 2005 - 2012 Open Source Matters, Inc. All rights reserved.
 * @license		GNU General Public License version 2 or later; see LICENSE.txt
 */

// no direct access
defined('_JEXEC') or die;

if (!function_exists('processQuery')) {
	function processQuery($db, $query, $params) {
		$db->setQuery($query);
		if ( !$db->query() ) {
			throw new Exception( "Database query failed. : " . $db->getErrorMsg() );	
		}	
		$res = $db->loadAssocList();

		$keys = array();

		foreach( $res as $entry ) {
			foreach( $entry as $k=>$v ) {
				$keys[$k] = "";
			}
		}

		$tblClass =  $params->get('tbl_class', '');
		print "<table class='". $tblClass ."'>";
		print "<thead>";
		print "<tr>";
		foreach( $keys as $k=>$dummy ) {
			print "<th>";
			print $k;
			print "</th>";
		}
		print "</tr>";
		print "</thead>";
		foreach( $res as $entry ) {
			print "<tr>";
			foreach( $keys as $k=>$dummy ) {
				print "<td>";
				print $entry[$k];
				print "</td>";
			}
			print "</tr>";
		}
		print "</table>";
	}


	$query =  $params->get('query');
	$db = JFactory::getDBO();

	if ($db == null) {
		throw new Exception("Database access failed.");	
	}
}
$querys = split( " *;", $query );
foreach( $querys as $q ) {
	if (trim($q)!="") {
		processQuery($db, $q, $params);
	}
}
?>



