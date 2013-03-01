<?php
 
// No direct access
 
defined('_JEXEC') or die('Restricted access'); ?>
<?php JHTML::stylesheet('j2mantis.css', 'components/com_j2mantis/assets/'); ?>

<style type="text/css">
	.issue-resolved {
		text-decoration: line-through;	
	}
</style>


<h1><?php echo $this->caption; ?></h1>
<table  id="mt_overview">
<thead>
<tr>
	<th>
	  <?php echo JText::_('Status');?>
	</th>
	<th>
	  <?php echo JText::_('Summary');?>
	</th>
	<th>
	  <?php echo JText::_('Category');?>
	</th>
	<th>
	  <?php echo JText::_('project'); ?>
	</th>
	<th>
	  <?php echo JText::_('last update');?>
	</th>
</tr>
</thead>
<?php foreach($this->bugs as $bug){ ?>
<tr <?php if ( strcmp( $bug->status->name, "resolved" ) == 0 ) echo 'class="issue-resolved"'; ?> >
	<td>
		<?php echo $bug->status->name ?>
	</td>
	<td>
		<a href="?option=com_j2mantis&amp;view=showbug&amp;bugid=<?php echo base64_encode($this->mantis->encode((string)$bug->id)); ?>&amp;Itemid=<?php echo JRequest::getInt('Itemid',0);?>"><?php echo $bug->summary ?></a>
	</td>
	<td>
		<?php echo $bug->category ?>
	</td>
	<td>
		<?php echo $bug->project->name ?>
	</td>
	<td>
		<?php 
		$timezone_offset = -date("H",strtotime("Y-m-d",time()));
		echo date("d.m.Y H:i" ,strtotime($bug->last_updated. " +" . $timezone_offset . " hours"));?>
	</td>
</tr>
<?php } ?>
</table>
