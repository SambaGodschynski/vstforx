<?php
/**
 * @package    Joomla.J2Mantis
 * @subpackage Components
 * components/com_J2Mantis/view/view.html.php
 * @license    GNU/GPL
*/
 
// no direct access
defined( '_JEXEC' ) or die( 'Restricted access' );
 
jimport( 'joomla.application.component.view');
jimport('joomla.application.component.helper');
 
/**
 * HTML View class for the J2Mantis Component
 *
 * @package    J2Mantis
 */
 

class J2MantisViewshowknownissues extends JView
{

	function filterArray($bugs, $version, $url) {
		if (count($bugs) == 0) {
			return $bugs;		
		}
		$new_array = array();
		foreach ($bugs as $k => $v) {
			if ( $v->target_version != $version ) 
			{
				continue;			
			}
			if ( $v->status->name != "confirmed" &&
				 $v->status->name != "assigned"  &&
				 $v->status->name != "resolved" ) 
			{
				continue;			
			}
			$v->target_url = $url . "/view.php?id=" . $v->id;		
			$new_array[$k] = $v;		
		}
		return $new_array;
	}

	function getTargetUrlBase() {
		$url = &JComponentHelper::getParams( 'com_j2mantis' )->get("url");
		$url = parse_url($url);
		if (!$url) {
			return "";		
		}
		return $url["scheme"] . "://" . $url["host"];
	}

    function display($tpl = null)
    {
		$input = JFactory::getApplication()->input;
		$version = $input->get('target_version');
		if (empty($version)) {
			echo "no version set.";
			return;		
		}

		$url = $this->getTargetUrlBase();
    	
    	require_once( JPATH_COMPONENT.DS.'JoomlaMantisParameter.class.php');
		$settings = new JoomlaMantisParameter();
		require_once( JPATH_COMPONENT.DS.'MantisConnector.class.php');
		$Mantis = new MantisConnector($settings);

		$findIds = $settings->getMantisProjectIds();
		if( empty( $findIds ) || (sizeof($findIds)==1 && $findIds[0] == 0) ){		
			foreach($Mantis->getAllProjects(false, true) as $id => $p){			
				$settings->addMantisProjectId($id); 
			}
		}
		$bugs = $Mantis->getAllBugsOfAllProjects();
		$bugs = $this->filterArray($bugs, $version, $url);
    	$caption = JText::_('Known issues');
        $this->assignRef( 'caption', $caption );
        $this->assignRef( 'bugs', $bugs);
        $this->assignRef( 'mantis', $Mantis);
		$this->assignRef( 'version', $version);
		$this->assignRef( 'target_url', $url);
        
        parent::display($tpl);
    }
    

}

?>
