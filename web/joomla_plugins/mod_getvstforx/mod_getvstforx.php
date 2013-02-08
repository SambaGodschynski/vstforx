<?php
/**
 * @package		Joomla.Site
 * @subpackage	mod_login
 * @copyright	Copyright (C) 2005 - 2012 Open Source Matters, Inc. All rights reserved.
 * @license		GNU General Public License version 2 or later; see LICENSE.txt
 */

// no direct access
defined('_JEXEC') or die;
require_once'download.php';
require_once'payment.php';

function handleTransactionSucceed() {
?>
	<div class="alert alert-success">
		<strong>Thank you</strong> for purchase.
	</div>

<?php
}


function checkPageSource() {
	$val = $_GET["src"];
	switch ($val) {
		case "ppsc": /*paypal succeed*/
			handleTransactionSucceed();	
			break;
	}
}

checkPageSource();

try {
	$user	= JFactory::getUser();
	if (!showShop($user)) {
?>
		<div class="alert alert-warning">
		<strong>Login needed!</strong>
		<p>you need to login in order to purchase VSTForx.</p>		
		</div>
		<br/>
		<p><strong>- OR -</strong></p>
		<br/>
		<p>Download the outdated beta version for free.</p>
<?php	
	} else {
?>
		<h4>Your Downloads</h4>
<?php
	}
	showDownloads($user);
} catch (Exception $e) {
	?>
	<div class="alert alert-error">
	<h4>Critical Error:</h4> <?php echo($e->getMessage()); ?>		
	</div>
	<?php
}
?>



