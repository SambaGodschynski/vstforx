<?php
/**
 * @package		Joomla.Site
 * @subpackage	mod_login
 * @copyright	Copyright (C) 2005 - 2012 Open Source Matters, Inc. All rights reserved.
 * @license		GNU General Public License version 2 or later; see LICENSE.txt
 */

// no direct access
defined('_JEXEC') or die;
require_once'paymentDetails.php';
require_once'paypalbtn.php';
require_once'download.php';

$user	= JFactory::getUser();
if ($user.guest)

?>

<?php if ($user->guest == true) : /*user isn't logged in*/ ?>
	<div class="alert alert-error">	
		<p>You need to login first!</p>
	</div>
<?php endif; ?>

<?php								
	try {
		$userpaid = hasUserPaid($user); 
	} catch (Exception $e) {
		?>
		<div class="alert alert-error">
		<h4>Critical Error:</h4> <?php echo($e->getMessage()); ?>		
		</div>
		<?php
		$userpaid = -1;
	}
?>

<?php
	if ($userpaid == 0) {
		showPaypalBtn($user);
	} elseif($userpaid == 1) {
		showDownloads($user);
	}
?>
<br/>
<div>
  <p><strong>-OR-</strong></p>
  <p>
  Download the old Beta version to get the idea.
  </p>
<?php
	showBetaDownloads();
?>
</div>



