<?php

function showPaypalBtn03($usr) {
?>

<form action="https://www.sandbox.paypal.com/cgi-bin/webscr" method="post">
<input type="hidden" name="cmd" value="_s-xclick">
<input type="hidden" name="hosted_button_id" value="8UXX9HRF8SBMY">
<input type="hidden" name="custom" value="<?php echo($usr->id) ?>">
<input type="image" src="https://www.sandbox.paypal.com/en_US/i/btn/btn_buynow_SM.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">
<img alt="" border="0" src="https://www.sandbox.paypal.com/en_US/i/scr/pixel.gif" width="1" height="1">
</form>


<?php
}


function showPaypalBtn($user, $productid) {
	switch($productid) {
		case 3 : showPaypalBtn03($user); return;
	}
?>
	unavailable
<?php
}


?>
