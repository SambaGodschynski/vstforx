<?php

require_once 'mantis_user.php';

$c=0;

function processMail($str) {
  echo $str . "<br/>";
  //sendMail($str, "johannes.unger@vstforx.de", "How much is VSTForx worth for you?", "poll.txt");
  //poll_out($str);
  global $c;
  $c = $c + 1;
}

function filter($entry) {
  if ($entry["login_count"] >= 1) {
    return 1;
  }
  return 0;
}

processMails("filter", "processMail");
echo $c . " mails processed."
?>