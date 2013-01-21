<?php
/**
 * @version		$Id: loader.php 882 2013-01-07 11:53:44Z dhorsfall $
 * @package		Joomla.Framework
 * @subpackage	Form
 * @copyright	Copyright (C) 2005 - 2010 Open Source Matters, Inc. All rights reserved.
 * @license		GNU General Public License version 2 or later; see LICENSE.txt
 */

defined('JPATH_BASE') or die;

jimport('joomla.form.formfield');

/**
 * Form Field class for the Joomla Framework.
 *
 * @package		Joomla.Framework
 * @subpackage	Form
 * @since		1.6
 */
class JFormFieldLoader extends JFormField
{
	/**
	 * The form field type.
	 *
	 * @var		string
	 * @since	1.6
	 */
	protected $type = 'Loader';

	/**
	 * Method to get the field input markup.
	 *
	 * @return	string	The field input markup.
	 * @since 	1.6
	 */
	function getInput(){
		$document = JFactory::getDocument();
		$header_media = $document->addStyleSheet(JURI::root(1) . '/templates/jp_audiopraise_j1.7/admin/admin.css');
		$header_media .= $document->addScript(JURI::root(1) . '/templates/jp_audiopraise_j1.7/admin/mooRainbow.js');
		$header_media .= $document->addScript(JURI::root(1) . '/templates/jp_audiopraise_j1.7/admin/admin.js');
		return $header_media;
	}
}
