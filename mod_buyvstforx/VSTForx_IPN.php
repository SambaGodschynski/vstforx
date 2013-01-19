<?php

require "paypal.class/paypal.class.php";

$p = new paypal_class;
/**
 * !!!!!!!!!!
 * consider line 186 in paypal.class.php before switching into realmode
 * !!!!!!!!!!
 */
$p->paypal_url = 'https://www.sandbox.paypal.com/cgi-bin/webscr';

if ($p->validate_ipn()) {

	if($p->ipn_data['payment_status']!='Completed') {
		error("payment failed.");
		return;
	}
	
	$link = mysql_connect('localhost', 'web22', '$$1334$$');
	if (!$link) {
		error("connection to database failed.");
		exit;
	}

	$amount = $p->ipn_data['mc_gross'] - $p->ipn_data['mc_fee'];
	$result = mysql_query("INSERT INTO `usr_web22_1`.`frx_selled` (
				`juser` ,
				`transaction_id` ,
				`email` ,
				`amount` ,
				`item`,
				`rquest`
				) VALUES (
					".(int)$p->ipn_data['custom'].",
					'".esc($p->ipn_data['txn_id'], $link)."',
					'".esc($p->ipn_data['payer_email'], $link)."',
					".(float)$amount.",
					'".esc($p->ipn_data['item_name'], $link)."',
					'".esc(http_build_query($_POST), $link)."'
				)");

	if (!$result) {
		error( "database query failed: ". mysql_error() );
		mysql_close($link);
		exit;
	}
	mysql_close($link);
}


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

function esc($str, $dblink)
{
	return mysql_real_escape_string($str,$dblink);
}
?>


