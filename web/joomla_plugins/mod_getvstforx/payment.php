<?php

defined('_JEXEC') or die;
require_once 'database.php';
require_once 'paypalbtn.php';

function getPurchaseOptions($user) {
	$db = getDB();
	$userid = 0;
	$default_user = getDefaultUser();
	if (!$user->guest) {
		$userid = $user->id;	
	}
	$date = date( 'Y-m-d', time() );

	$query = "SELECT  frx_selling_event.productid,
		              frx_products.name AS productname,
				      frx_selling_event.amount,
					  frx_selling_event.text
		  FROM frx_selling_event
		  JOIN frx_products       ON frx_products.id = frx_selling_event.productid
		  WHERE ('" . $date . "' >= frx_selling_event.begin AND '" . $date . "' <= frx_selling_event.end
          OR frx_selling_event.begin is NULL AND frx_selling_event.end is NULL)
		  AND frx_selling_event.productid NOT IN (
		    SELECT frx_selled.productid FROM frx_selled WHERE frx_selled.juser = " . $db->quote($userid) . "
          )
	;";
	return processQueryAssoc($db, $query);
}


function showShop($user, $res) {
		if (!$res) {
			return true;		
		}
?>
		<div class="well frx-odd">
			<table class="table table-striped">
				<thead>
					<tr>
						<th>Product</th>
						<th>Notes</th>
						<th>Amount</th>
						<th>Action</th>
					</tr>
				</thead>
<?php foreach($res as $x) { ?>
				<tr>			
		 			<td><?php echo($x['productname'])?></td> 
					<td><?php echo($x['text'])?></td> 
					<td><?php echo( number_format($x['amount'], 2) )?> EUR</td> 
					<td>
						<?php showPaypalBtn($user, $x['productid']); ?>
					</td>
				</tr>
<?php } ?>		
			</table>
		</div>
<?php
	return true;
}
?>
