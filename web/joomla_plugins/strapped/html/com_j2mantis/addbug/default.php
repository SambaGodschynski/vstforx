<?php
 
// No direct access
 
defined('_JEXEC') or die('Restricted access'); ?>

<?php JHTML::_('behavior.formvalidation') ?>
<?php JHTML::stylesheet('j2mantis.css', 'components/com_j2mantis/assets/'); ?>
<script type="text/javascript">
//<![CDATA[
function myValidate(f) {
        if (document.formvalidator.isValid(f)) {
                //f.check.value='<?php echo JUtility::getToken(); ?>';//send token
                return true; 
        }
        else {
                alert('<?php echo JText::_('please check the form');?>');
        }
        return false;
}
//]]>
</script>

<div id="j2Mantis" class="item-page">
<h2>Report Bug</h2>

<?php if(!empty($_POST['errors'])): ?>
<div class="error"><h3><?php echo JText::_('Error');?></h3><ul>
<?php foreach($_POST['errors'] as $error){
	echo "<li>" . $error . "</li>";
}?>
</ul></div>
<?php endif; ?>


<?php
	$user	= JFactory::getUser();
	if (!$user->guest) {
		$uname = $user->username;
		$uemail = $user->email;	
	}
?>

<form method="post" 
	class="form-horizontal"
	action="?option=com_j2mantis&amp;task=addBug&amp;Itemid=<?php echo JRequest::getInt('Itemid',0);?>"  
	onsubmit="return myValidate(this);">
<input type="hidden" name="view" value="addbug" />
<input type="hidden" name="check" value="post" />
<?php if( sizeof($this->project) > 1 ):?>
	<div class="control-group">
		<label for="project" class="control-label"><?php echo JText::_('Project');?></label>
		<div class="controls">		
			<select name="project" id="project" onchange="changeProject(this)">
				<?php foreach($this->project as $pid => $name){ ?>
					
					<option <?php if(!empty($_POST['project']) && $_POST['project'] ==$pid ) echo 'selected="selected"'; ?> value="<?php echo $pid; ?>"><?php echo $name ?></option>
					
				<?php } ?>
			</select >
		</div>
	</div>
<?php else: ?>
	<?php foreach($this->project as $pid => $name){ ?>
		<input type="hidden" name="project" value="<?php echo $pid; ?>" />
	<?php } ?>
<?php endif; ?>
<div class="control-group">
	<label for="category" class="control-label"><?php echo JText::_('Category');?></label>
	<div class="controls">
		<select name="category" id="category">
			<?php foreach($this->cat as $id => $cArray){ ?>
				<?php foreach($cArray as $c){ ?>
					<option <?php if(!empty($_POST['category']) && $_POST['category'] ==$c ) echo 'selected="selected"'; ?> value="<?php echo $c; ?>" class="project-<?php echo $id; ?>"><?php echo  $c ?></option>
				<?php } ?>
			<?php } ?>
		</select >
	</div>
</div>

<div class="control-group">
	<label for="summary" class="control-label">Summary*</label>
	<div class="controls">
		<input type="text" name="summary" id="summary" class="required" <?php if(!empty($_POST['summary']))echo 'value="'.$_POST['summary'].'"' ?> />
	</div>
</div>
<div class="control-group">
	<label for="name" class="control-label"><?php echo JText::_('Name');?>*</label>
	<div class="controls">
		<input type="text" name="name" id="name" class="required"  <?php if(!empty($uname))echo 'value="'.$uname.'"' ?>  />
	</div>
</div>
<div class="control-group">
	<label for="email" class="control-label"><?php echo JText::_('E-Mail');?>*</label>
	<div class="controls">
		<input type="text" name="email" id="email" class="required validate-email"  <?php if(!empty($uemail))echo 'value="'.$uemail.'"' ?> />
	</div>
</div>
<div class="control-group">
	<label for="priority" class="control-label"><?php echo JText::_('Priority');?></label>
	<div class="controls">
		<select name="priority" id="priority">
		<option value="20" <?php if(!empty($_POST['priority']) && $_POST['priority'] ==20 ) echo 'selected="selected"'; ?>><?php echo JText::_('low');?></option>
		<option value="30" <?php if(empty($_POST['priority']) or (!empty($_POST['priority']) && $_POST['priority'] ==30) ) echo 'selected="selected"'; ?>><?php echo JText::_('normal');?></option>
		<option value="40" <?php if(!empty($_POST['priority']) && $_POST['priority'] ==40 ) echo 'selected="selected"'; ?>><?php echo JText::_('high');?></option>
		</select>
	</div>
</div>
<div class="control-group">
	<label for="description" class="control-label"><?php echo JText::_('Description');?></label>
	<div class="controls">
		<textarea name="description" id="description"  cols="50" rows="8"><?php if(!empty($_POST['description']))echo $_POST['description'] ?></textarea>
	</div>
</div>
<div class="control-group">
	<?php $params = &JComponentHelper::getParams( 'com_j2mantis' ); ?>
	<?php if($params->get('captcha')){ plgSystemJCCReCaptcha::display(); } ?>
	<div class="controls">
		<input type="submit" class="btn" value="<?php echo JText::_('Submit');?>"/>
	</div>
</div>
</form>
<?php if((boolean)$params->get('overview')): ?>
	<br />
	
<?php endif; ?>
<br style="clear: both;" />
</div>

<script type="text/javascript">
//<![CDATA[
var catogoryLists = new Array(<?php echo sizeof($this->project) ?>);
<?php foreach($this->project as $id => $name ): ?>
catogoryLists[<?php echo $id ?>] = ["<?php echo implode('","',$this->cat[$id]) ?>"];
<?php endforeach; ?>

function addNewOptions(id,projectId){
    var cList = catogoryLists[projectId];
    for( var i = 0; i < cList.length; i++ ){
        newOption = document.createElement("option");
         newOption.value = cList[i]; // assumes option string and value are the same
         newOption.text=cList[i];
        try{
            newOption.inject($('category'));
        }
        catch(ex){
            id.add(newOption);
        }
    }
}

function changeProject(el){
    var projectId = el.value;
    $('category').options.length = 0;
    addNewOptions($('category'),projectId);
}
changeProject( $('project') );
//]]>
</script>
