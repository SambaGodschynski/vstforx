<?php



function error($msg) {
/*
0	message wird an das Log-Subsystem von PHP gesendet
1	message wird via E-Mail an die Adresse gesendet,
2	Diese Option ist nicht mehr verfügbar.
3	message wird an die Datei destination angefügt. 
    Ein Zeilenumbruch wird nicht automatisch an das Ende des message-Strings angehängt.
4	message wird direkt zum SAPI-Logging-Handler gesendet.
*/
	$type = 3;
	$d = date("d.m.Y");
	$t = date("H:i");	
	error_log($d.', '.$t.':: '.$msg."\n", $type, "transactionResponseErr.log");
}




// STEP 1: Read POST data

// reading posted data from directly from $_POST causes serialization 
// issues with array data in POST
// reading raw POST data from input stream instead. 
$raw_post_data = file_get_contents('php://input');
$raw_post_array = explode('&', $raw_post_data);
$myPost = array();
foreach ($raw_post_array as $keyval) {
  $keyval = explode ('=', $keyval);
  if (count($keyval) == 2)
     $myPost[$keyval[0]] = urldecode($keyval[1]);
}
// read the post from PayPal system and add 'cmd'
$req = 'cmd=_notify-validate';
if(function_exists('get_magic_quotes_gpc')) {
   $get_magic_quotes_exists = true;
} 
foreach ($myPost as $key => $value) {        
   if($get_magic_quotes_exists == true && get_magic_quotes_gpc() == 1) { 
        $value = urlencode(stripslashes($value)); 
   } else {
        $value = urlencode($value);
   }
   $req .= "&$key=$value";
}

 
// STEP 2: Post IPN data back to paypal to validate

$ch = curl_init('https://sandbox.paypal.com/cgi-bin/webscr');
curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
curl_setopt($ch, CURLOPT_POST, 1);
curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
curl_setopt($ch, CURLOPT_POSTFIELDS, $req);
curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
curl_setopt($ch, CURLOPT_FORBID_REUSE, 1);
curl_setopt($ch, CURLOPT_HTTPHEADER, array('Connection: Close'));

// In wamp like environments that do not come bundled with root authority certificates,
// please download 'cacert.pem' from "http://curl.haxx.se/docs/caextract.html" and set the directory path 
// of the certificate as shown below.
// curl_setopt($ch, CURLOPT_CAINFO, dirname(__FILE__) . '/cacert.pem');
if( !($res = curl_exec($ch)) ) {
    error("Got " . curl_error($ch) . " when processing IPN data");
    curl_close($ch);
    exit;
}
curl_close($ch);
 
// STEP 3: Inspect IPN validation result and act accordingly

if (strcmp ($res, "VERIFIED") == 0) {
    // check whether the payment_status is Completed
    // check that txn_id has not been previously processed
    // check that receiver_email is your Primary PayPal email
    // check that payment_amount/payment_currency are correct
    // process payment

    // assign posted variables to local variables
    $item_name = $_POST['item_name'];
    $item_number = $_POST['item_number'];
    $payment_status = $_POST['payment_status'];
    $payment_amount = $_POST['mc_gross'];
    $payment_currency = $_POST['mc_currency'];
    $txn_id = $_POST['txn_id'];
    $receiver_email = $_POST['receiver_email'];
    $payer_email = $_POST['payer_email'];
	$custom = $_POST['custom'];
	$usrid = (int)$custom;
	
	if($usrid==0) {
		error("no userid.");
	}
	
	$link = mysql_connect('localhost', 'web22', '$$1334$$');
	if (!$link) {
		error("connection to database failed.");
	    exit;
	}

	$result = mysql_query('INSERT INTO  `usr_web22_1`.`frx_donations` (`usr`) VALUES ('.$userid.');');
	
	if (!$result) {
		error( "database query failed: ". mysql_error() );
		exit;
	}

	mysql_close($link);


} else if (strcmp ($res, "INVALID") == 0) {
	error("transaction was'nt verified.");
}
?>


