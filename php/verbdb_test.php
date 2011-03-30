<?php

//$_VERB['verbdbd_port'] = 9092;
require_once(dirname(__FILE__). "/client.php");
require_once("/www/verb_remote/current/lib/constants.php");
require_once("/www/verb_remote/current/lib/general.php");
require_once("/www/verb_remote/current/lib/store.php");
require_once("/www/verb_remote/current/lib/phpapi.php");
require_once("/www/verb_remote/current/lib/verbdata.php");
require_once("/www/verb_remote/current/lib/verb_exception.php");
$_VERB['settings']['subdomain'] = "btg";
$_VERB['staging'] = true;
$_VERB['config']['secret_key'] = "9dee6740e4ddbe2c9488242318b764beeef7a17f";
//$_VERB['settings']['subdomain'] = "peeptoe";
//$_VERB['config']['secret_key'] = "9dee6740e4ddbe2c9488242318b764beeef7a17f";
//$_VERB['settings']['subdomain'] = "hintmagazine";
//$_VERB['config']['secret_key'] = "8021e63777c6fe00a7a0ab660b29780d0e9fee25";
//$_VERB['settings']['subdomain'] = "impose";
//$_VERB['config']['secret_key'] = "4dd583d746bcaaac042a0a132e6971b245fd174f";
$k = microtime(true);

$items = verb("artists");
foreach ($items as $r) {
  echo $r->name . "\n";
}

echo "\n" . (microtime(true)-$k) . " seconds elapsed\n";

?>
