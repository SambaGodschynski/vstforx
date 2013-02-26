<?php

defined('_JEXEC') or die;
require_once 'database.php';
require_once 'paypalbtn.php';

/**
 * @param 2=old beta
 *        3=VSTForx
 */
function getDownloads($user) {
	$db = getDB();
	$userid = 0;
	$default_user = getDefaultUser();
	if (!$user->guest) {
		$userid = $user->id;	
	}
	$query = "SELECT frx_products.name AS pr_name,
              frx_downloads.name AS version, 
			  frx_product_os.name AS os, 
			  frx_downloads.date,
			  frx_downloads.id,
			  frx_download_notes.text AS notes
		FROM frx_downloads
		JOIN frx_products       ON frx_products.id = frx_downloads.productid
		JOIN frx_product_os     ON frx_product_os.id = frx_downloads.osid
		JOIN frx_download_notes ON frx_downloads.noteid = frx_download_notes.id
		WHERE productid IN (
			SELECT productid FROM frx_selled WHERE frx_selled.juser=". $db->quote($userid) ."		
			OR frx_selled.juser=". $db->quote($default_user) ."
		)
		ORDER BY frx_downloads.date DESC;
	";
	return processQuery($db, $query);
}

function showDwnlBtn($download_id) {
?>
	<a href="frx_download.php?dst=<?php echo($download_id); ?>" class="btn btn-success">Download</a> 
<?php
}

function showDownloadsImpl($user) {
		$res = getDownloads($user);
?>
	<table class="table table-striped">
		<thead>
			<tr>
				<th>Product</th>
				<th>Version</th>
				<th>Os</th>
			</tr>
		</thead>
<?php foreach($res as $x) { ?>
		<tr>			
 			<td><?php echo($x[0])?></td>
			<td><?php echo($x[1])?></td> 
			<td><?php echo($x[2])?></td>
			<td>
				<?php echo showDwnlBtn($x[4]); ?>
			</td>
		</tr>
<?php } ?>
			
	</table>
<?php
}


function showDownloads($user) {
	showDownloadsImpl($user);
}
?>
