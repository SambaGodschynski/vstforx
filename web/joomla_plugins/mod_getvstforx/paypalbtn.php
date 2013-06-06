<?php

function showPaypalBtn03($usr) {
/*
<form action="https://www.sandbox.paypal.com/cgi-bin/webscr" method="post">
<input type="hidden" name="cmd" value="_s-xclick">
<input type="hidden" name="custom" value="<?php echo($usr->id) ?>">
<input type="hidden" name="hosted_button_id" value="8UXX9HRF8SBMY">
<input type="image" src="https://www.sandbox.paypal.com/en_US/i/btn/btn_buynow_SM.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">
<img alt="" border="0" src="https://www.sandbox.paypal.com/en_US/i/scr/pixel.gif" width="1" height="1">
</form>
*/
?>

<form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
<input type="hidden" name="cmd" value="_s-xclick">
<input type="hidden" name="hosted_button_id" value="DK49P73UQZT8L">
<input type="hidden" name="custom" value="<?php echo($usr->id) ?>">
<input type="image" src="https://www.paypalobjects.com/en_GB/i/btn/btn_buynow_SM.gif" border="0" name="submit" alt="PayPal – The safer, easier way to pay online.">
<img alt="" border="0" src="https://www.paypalobjects.com/de_DE/i/scr/pixel.gif" width="1" height="1">
</form>

<?php
}

function showPaypalBtn07($usr) {
?>
<form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
<input type="hidden" name="cmd" value="_s-xclick">
<input type="hidden" name="hosted_button_id" value="FM2G4UGSNB5LW">
<input type="hidden" name="custom" value="<?php echo($usr->id) ?>">
<input type="image" src="https://www.paypalobjects.com/en_GB/i/btn/btn_buynow_SM.gif" border="0" name="submit" alt="PayPal – The safer, easier way to pay online.">
<img alt="" border="0" src="https://www.paypalobjects.com/de_DE/i/scr/pixel.gif" width="1" height="1">
</form>
<?php
}


function showPaypalBtn($user, $productid) {
	switch($productid) {
		/*case 3 : showPaypalBtn03($user); return;
		case 7 : showPaypalBtn07($user); return;*/
	}
?>
	<strong>Payment</strong> is currently not available, please try it again later!
<?php
}


?>

