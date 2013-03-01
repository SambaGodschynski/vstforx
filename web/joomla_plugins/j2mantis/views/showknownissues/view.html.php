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


	function array_sort($array, $on, $order='SORT_DESC') 
    { 
      $new_array = array(); 
      $sortable_array = array(); 
  
      if (count($array) > 0) { 
          foreach ($array as $k => $v) { 
              if (is_array($v)) { 
                  foreach ($v as $k2 => $v2) { 
                      if ($k2 == $on) { 
                          $sortable_array[$k] = $v2; 
                      } 
                  } 
              } else { 
                  $sortable_array[$k] = $v; 
              } 
          } 
  
          switch($order) 
          { 
              case 'SORT_ASC':    
                  //echo "ASC"; 
                  asort($sortable_array); 
              break; 
              case 'SORT_DESC': 
                  //echo "DESC"; 
                  arsort($sortable_array); 
              break; 
          } 
  
          foreach($sortable_array as $k => $v) { 
              $new_array[] = $array[$k]; 
          } 
      } 
      return $new_array; 
   } 

	function filterArray($bugs, $version) {
		if (count($bugs) == 0) {
			return $bugs;		
		}
		$new_array = array();
		foreach ($bugs as $k => $v) {
			if ( strcmp($v->target_version, $version)!=0 ) 
			{
				continue;			
			}
			$new_array[$k] = $v;		
		}
		return $new_array;
	}

    function display($tpl = null)
    {
		$app = JFactory::getApplication('site');
		$componentParams = $app->getParams('com_j2mantis');
		$version = $componentParams->get('target_version');
		if (empty($version)) {
			echo "no version set.";
			return;		
		}
    	
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
		$bugs = $this->filterArray($bugs, $version);
		$bugs = $this->array_sort($bugs, 'last_updated');
    	$caption = JText::_('Known issues');
        $this->assignRef( 'caption', $caption );
        $this->assignRef( 'bugs', $bugs);
        $this->assignRef( 'mantis', $Mantis);
        
        parent::display($tpl);
    }
    

}

?>
