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
		<strong>Thank you</strong> for purchasing.
	</div>

<?php
}

function checkState($user) {
	if (!$user->guest) {
		return true;	
	}
	return false;
}

function checkPageSource() {
	$val = $_GET["src"];
	switch ($val) {
		case "ppsc": /*paypal succeed*/
			handleTransactionSucceed();	
			break;
	}
}

function showUserAlert() {
?>
		<div class="alert alert-warning">
		<strong>Login needed!</strong>
		<p>you need to login in order to purchase VSTForx.</p>
		<a href="<?php echo JRoute::_('index.php?option=com_users&view=registration'); ?>">don't have an account?</a>	
		</div>
		<br />
		<p><strong>- OR -</strong></p>
		<br />
		<p>enjoy the <b>free</b> stuff:</p>
		<br />
<?php
}

function beginShop() {
?>

		<h3>Your purchase options</h3>
		<br />
<?php
}

function beginDownl($showtitle) {
	if ($showtitle) {
?>

		<h3>Your Downloads</h3>
		<br />
<?php
	}
}

checkPageSource();

try {
	$user	= JFactory::getUser();
	$showdwntitle = false;
	if (!checkState($user)) {
		showUserAlert();
	} else {
		$opt = getPurchaseOptions($user);
		if ( sizeof($opt) > 0 ) {
			beginShop();
			showShop($opt);		
		}
		$showdwntitle = true;
	}
	beginDownl($showdwntitle);
	showDownloads($user);
} catch (Exception $e) {
	?>
	<div class="alert alert-error">
	<h4>Critical Error:</h4> <?php echo($e->getMessage()); ?>		
	</div>
	<?php
}
?>



