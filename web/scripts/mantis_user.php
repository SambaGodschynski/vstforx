<?php

$db_link = mysql_connect ( "localhost", 
                           "web22", 
                           "$$1334$$" );
$db_sel = mysql_select_db("usr_web22_1")
     or die("Auswahl der Datenbank fehlgeschlagen");

function poll_out($mail) { 
  $sql = "INSERT INTO `usr_web22_1`.`man_pollquery_sent` (`email`) VALUES ('$mail');";
 
  $db_erg = mysql_query( $sql );
  if ( ! $db_erg )
  {
    die('Ungültige Abfrage: ' . mysql_error());
  }
}

function sendMail($to, $from, $subject, $msgFile)  {
  $message = implode(" ", @file($msgFile));
  $headers = "From:" . $from;
  mail($to,$subject,$message,$headers)
    or die("sending mail failed");
  echo "Mail Sent to:" . $to . "<br/>";
}

function processMails($filter, $processor) {
  $sql = "SELECT * FROM `mantis_user_table`";
 
  $db_erg = mysql_query( $sql );
  if ( ! $db_erg )
  {
    die('Ungültige Abfrage: ' . mysql_error());
  }
 
  while ($zeile = mysql_fetch_array( $db_erg, MYSQL_ASSOC))
  {
    if ( $filter($zeile) == 1 ) {
      $processor($zeile["email"]);
    }
  }
  
  mysql_free_result( $db_erg );
}
?>