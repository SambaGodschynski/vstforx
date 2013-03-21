<?php

defined('_JEXEC') or die;
require_once 'database.php';
require_once 'paypalbtn.php';


/**
 * maps name=>[  version=>[(name, version, os, date, id, notes), ...], ...]
 */
function mapDownloads($_x) {
	$res = array();	
	foreach($_x as $x) {
		$prodKey = $x[0];
		$versKey = $x[1];		
		if ( !array_key_exists($prodKey, $res) ) {
			$res[$prodKey] = array();
		}
		$prodVal = &$res[$prodKey];
		if ( !array_key_exists($versKey, $prodVal) ) {
			$prodVal[$versKey] = array();
		}
		$val = &$prodVal[$versKey];
		array_push($val, array(
			'version' => $x[1],
			'os' => $x[2],
			'date' => $x[3],
			'id' => $x[4],
			'notes' => $x[5],
			'name' => $x[6],
		) );
	}
	return $res;
}

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
	$query = "SELECT frx_products.id AS pr_id,
              frx_downloads.name AS version, 
			  frx_product_os.name AS os, 
			  frx_downloads.date,
			  frx_downloads.id,
			  frx_download_notes.text AS notes,
			  frx_products.name AS pr_name
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

/**
 * 
 */
function getProductInfo($prodId) {
	$db = getDB();
	$query = "SELECT frx_products.name, 
              frx_products.description,
			  frx_products.id
		FROM frx_products
		WHERE id = " . $db->quote($prodId) . ";";
	return processQuery($db, $query);
}

function showDwnlBtn($download_id) {
	$dstUrl = JURI::base() . "frx_download.php";
?>
	<form name="input" action="<?php echo $dstUrl; ?>" method="post">
		<input type="hidden" name="dst" value="<?php echo $download_id; ?>">
		<input type="submit" value="Download" class="btn btn-success">
	</form>
<?php
}

function showProducts(&$map) {
	$c = 0;
	foreach ( $map as $key => $value ) {
		$info = getProductInfo($key);
		if (count($info) == 0) {
			continue;		
		}
		$info = $info[0];
?>		
		<h4><?php echo $info[0] ?></h4>
		<?php showProductDetails($info); ?>
		<div class="well frx-<?php echo $c % 2==0 ? 'even' : 'odd' ?>" >
<?php
		showProduct($info, $value);
?>
		</div>
<?php
		$c++;
	}
}

function __beginVersionTable() {
?>
	<table class="table table-striped">
		<thead>
			<tr>
				<th>Version</th>
				<th>Plattform</th>
				<th>Date</th>
				<th>Notes</th>
			</tr>
		</thead>
<?php
}

function __endVersionTable() {
?>
	</table>
<?php
}

function __beginCollapse($name) {
?>
	<div class="accordion" id="<?php echo $name ?>">
<?php
}

function __beginCollapseEntry($name, $toogle_txt, $id, $showDefault=true, $addClass="") {
?>
  	<div class="accordion-group <?php echo $addClass ?>" >
    	<div class="accordion-heading">
    		<a class="accordion-toggle" data-toggle="collapse" data-parent="#<?php echo $name ?>" href="#<?php echo $id ?>" id="A-<?php echo $id ?>">
			<?php echo $toogle_txt ?>
    		</a>
    	</div>
		<div id="<?php echo $id ?>" class="accordion-body collapse <?php echo $showDefault ? 'in' : '' ?>">
    		<div class="accordion-inner">
<?php
}

function __endCollapse() {
?>
	</div>
<?php
}

function __endCollapseEntry() {
?>
			</div>
		</div>
    </div>
<?php
}

function showProductDetails($info) {
?>
	<div class="frx-productDetails">
		<?php echo $info[1]; ?>	
	</div>
<?php
}

function showProduct(&$info, &$map) {
	if ( count($map) == 0 ) {
	 return;
	}
	if ( count($info)==0 ) {
		return;	
	}
	$prId = $info[2];
	$collapseid = "collapse".$prId;
	__beginCollapse($collapseid);
		__beginCollapseEntry($collapseid, "Latest Version", $collapseid."-latest-version");
			__beginVersionTable();
				showAndPopLatestVersion($map);
			__endVersionTable();
		__endCollapseEntry();
		if ( count($map) > 0 ) {
			__beginCollapseEntry($collapseid, "Older Versions", $collapseid."-older-versions", false);
				__beginVersionTable();
					while ( showAndPopLatestVersion($map) );
				__endVersionTable();
			__endCollapseEntry();
		}
	__endCollapse();
}

function __orderbyos($arr) {
	$res = array();	
	foreach( $arr as $x ) {
		$res[ $x['os'] ] = $x;
	}
	arsort($res); 
	return $res;
}

/**
 * shows latest version and pops entry from download map.
 */
function showAndPopLatestVersion(&$map) {
	$keys = array_keys($map);
	if ( count($keys) == 0 ) {
		return;	
	}
	$entry = __orderbyos ( $map[ $keys[0] ] );
	foreach( $entry as $x ) {
?>
		<tr>			
			<td><?php echo($x['version'])?></td> 
			<td><?php echo($x['os'])?></td>
			<td><?php echo($x['date'])?></td>
			<td><?php echo($x['notes'])?></td>
			<td>
				<?php echo showDwnlBtn($x['id']); ?>
			</td>
		</tr>
<?php
	}
	unset($map[ $keys[0] ]);
	return true;
}


function showDownloadsImpl($user) {
		$res = getDownloads($user);
		$res = mapDownloads($res);
		showProducts($res);
}


function showDownloads($user) {
	showDownloadsImpl($user);
}
?>
