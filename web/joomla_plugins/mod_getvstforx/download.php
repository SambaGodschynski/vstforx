<?php

defined('_JEXEC') or die;
require_once 'database.php';

/**
 * @param 2=old beta
 *        3=VSTForx
 */
function getDownloads($productid) {
	$db = getDB();
	$query = "
		SELECT frx_downloads.name, 
			   frx_product_os.name, 
			   frx_downloads.date,
			   frx_downloads.filename,
			   frx_download_notes.text
		FROM frx_downloads
		JOIN frx_product_os      ON frx_product_os.id = frx_downloads.osid
		JOIN frx_download_notes  ON frx_downloads.noteid = frx_download_notes.id
		WHERE productid = ". $db->quote($productid) ."
		ORDER BY frx_downloads.date DESC;
	";
	return processQuery($db, $query);

	/*
		SELECT frx_downloads.name, 
			   frx_product_os.name, 
			   frx_downloads.date,
			   frx_downloads.filename,
			   frx_download_notes.text
		FROM frx_downloads
		JOIN frx_product_os      ON frx_product_os.id = frx_downloads.osid
		JOIN frx_download_notes ON frx_downloads.noteid = frx_download_notes.id
		WHERE productid = 2
		ORDER BY frx_downloads.date DESC;
	*/
}

function showDownloadsImpl($prodid) {
		$res = getDownloads($prodid);
?>
	<form class="form-horizontal">
		<fieldset>
			<legend>Select your version:</legend>
			<select>
<?php foreach($res as $x) { ?>
				<option><?php echo($x[0] . "  | " . $x[1])?></option>
<?php } ?>
			</select>
			<button type="submit" class="btn btn-success">Download</button>
		</fieldset>
	</form>
<?php
}


function showDownloads($user) {
	showDownloadsImpl(3);
}

function showBetaDownloads() {
	showDownloadsImpl(2);
}
?>
