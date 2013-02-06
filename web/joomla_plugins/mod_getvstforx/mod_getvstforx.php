<?php
/**
 * @package		Joomla.Site
 * @subpackage	mod_login
 * @copyright	Copyright (C) 2005 - 2012 Open Source Matters, Inc. All rights reserved.
 * @license		GNU General Public License version 2 or later; see LICENSE.txt
 */

// no direct access
defined('_JEXEC') or die;
require'paymentDetails.php';
require'paypalbtn.php';

$user	= JFactory::getUser();
if ($user.guest)

?>

<?php if ($user->guest == true) : /*user isn't logged in*/ ?>
	<div class="alert alert-error">	
		<p>You need to login first!</p>
	</div>
	<?php return; ?>
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

<?php 					 						/* user has'nt paid    */ 
	if ($userpaid == 0) {
		showPaypalBtn($user);
		return;	
	}
?>		
<?php if ($userpaid==1) : /* user has paid*/ ?>		
	<p>download</p>
<?php endif; ?>


