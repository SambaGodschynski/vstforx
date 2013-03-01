<?php
 
// No direct access
 
defined('_JEXEC') or die('Restricted access'); ?>
<?php JHTML::stylesheet('j2mantis.css', 'components/com_j2mantis/assets/'); ?>

<style type="text/css">
	.issue-resolved {
		text-decoration: line-through;	
	}
</style>

<?php
	echo "<!-- Show bugs for: " . $this->version . "-->";
?>
<h1><?php echo $this->caption; ?></h1>
<table  id="mt_overview" class="table table-striped">
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
	  <?php echo JText::_('last update');?>
	</th>
</tr>
</thead>
<?php foreach($this->bugs as $bug){ ?>
<tr <?php if ( $bug->status->name == "resolved" ) echo 'class="issue-resolved"'; ?> >
	<td>
		<?php echo $bug->status->name ?>
	</td>
	<td>
		<a href="<?php echo $bug->target_url; ?>"> <?php echo $bug->summary ?> </a>
	</td>
	<td>
		<?php echo $bug->category ?>
	</td>
	<td>
		<?php 
		$timezone_offset = -date("H",strtotime("Y-m-d",time()));
		echo date("d.m.Y H:i" ,strtotime($bug->last_updated. " +" . $timezone_offset . " hours"));?>
	</td>
</tr>
<?php } ?>
</table>
