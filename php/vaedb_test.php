<?php

//$_VAE['vaedbd_port'] = 9092;
require_once(dirname(__FILE__). "/client.php");
require_once("/www/vae_remote/current/lib/constants.php");
require_once("/www/vae_remote/current/lib/general.php");
require_once("/www/vae_remote/current/lib/store.php");
require_once("/www/vae_remote/current/lib/phpapi.php");
require_once("/www/vae_remote/current/lib/vaedata.php");
require_once("/www/vae_remote/current/lib/vae_exception.php");
$_VAE['settings']['subdomain'] = "rxart";
$_VAE['staging'] = true;
$_VAE['config']['secret_key'] = "9dee6740e4ddbe2c9488242318b764beeef7a17f";
$k = microtime(true);

foreach (vae("items") as $id => $item) {
  echo "Got $id\n";
  var_dump($item->structure());
}

echo "\n" . (microtime(true)-$k) . " seconds elapsed\n";

?>
