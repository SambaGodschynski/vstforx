<?php
/**
 * @package		Joomla.Site
 * @subpackage	mod_login
 * @copyright	Copyright (C) 2005 - 2012 Open Source Matters, Inc. All rights reserved.
 * @license		GNU General Public License version 2 or later; see LICENSE.txt
 */

// no direct access
defined('_JEXEC') or die;
JHtml::_('behavior.keepalive');
?>
<?php if ($type == 'logout') : ?>
<form action="<?php echo JRoute::_('index.php', true, $params->get('usesecure')); ?>" method="post" 
	id="frx-main-logout-form" class="navbar-form pull-right">
	<?php if ($params->get('greeting')) : ?>
	<span class="navbar-text">
		<?php if($params->get('name') == 0) : {
		echo JText::sprintf('MOD_LOGIN_HINAME', $user->get('name'));
	} else : {
		echo JText::sprintf('MOD_LOGIN_HINAME', $user->get('username'));
	} endif; ?>
	</span>
	<?php endif; ?>
	
		<input type="submit" name="Submit" class="button btn btn-primary" value="<?php echo JText::_('JLOGOUT'); ?>" />
		<input type="hidden" name="option" value="com_users" />
		<input type="hidden" name="task" value="user.logout" />
		<input type="hidden" name="return" value="<?php echo $return; ?>" />
		<?php echo JHtml::_('form.token'); ?> 
</form>
<?php else : ?>
<form action="<?php echo JRoute::_('index.php', true, $params->get('usesecure')); ?>" method="post" id="frx-main-login-form" class="navbar-form pull-right">
	<fieldset class="userdata">
		<input id="modlgn-username" type="text" name="username" class="span2" placeHolder="Username"/>
		<input id="modlgn-passwd" type="password" name="password" class="span2" placeHolder="Password"/>
		<input type="submit" name="Submit" class="button btn btn-primary" value="<?php echo JText::_('JLOGIN') ?>" />
		<input type="hidden" name="option" value="com_users" />
		<input type="hidden" name="task" value="user.login" />
		<input type="hidden" name="return" value="<?php echo $return; ?>" />
		<?php echo JHtml::_('form.token'); ?>
	</fieldset>
	<a href="<?php echo JRoute::_('index.php?option=com_users&view=registration'); ?>" > <?php echo JText::_('MOD_LOGIN_REGISTER'); ?></a>
</form>
<?php endif; ?>
