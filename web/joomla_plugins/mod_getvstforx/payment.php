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
	$query = "SELECT  frx_products.id,
		          frx_products.name,
				  frx_products.amount
		  FROM frx_products
		  WHERE id NOT IN (
			SELECT productid FROM frx_selled 
			WHERE frx_selled.juser=".$db->quote($userid)." 
			OR frx_selled.juser=".$db->quote($default_user)."
		);		
	";
	return processQuery($db, $query);

	/*
		SELECT  frx_products.id,
				frx_products.name,
				frx_products.amount
		FROM frx_products
		WHERE id NOT IN (
			SELECT productid FROM frx_selled 
			WHERE frx_selled.juser = 65 
			OR frx_selled.juser = 0 << products bought by default user will not appear in puchase list 
		);
	*/
}


function checkState($user) {
	if (!$user->guest) {
		return true;	
	}
	return false;
}

function showShop($user) {
		if (!checkState($user)) {
			return false;		
		}
		$res = getPurchaseOptions($user);	
		if (!$res) {
			return true;		
		}
?>
	<h4>Your purchase options</h4>
	<table class="table table-striped">
		<thead>
			<tr>
				<th>Product</th>
				<th>Amount</th>
				<th>Action</th>
			</tr>
		</thead>
<?php foreach($res as $x) { ?>
		<tr>			
 			<td><?php echo($x[1])?></td> 
			<td><?php echo($x[2])?> EUR</td> 
			<td>
				<?php showPaypalBtn($user, $x[0]); ?>
			</td>
		</tr>
<?php } ?>		
	</table>
<?php
	return true;
}
?>
