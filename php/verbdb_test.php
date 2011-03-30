<?php

//$_VAE['vaedbd_port'] = 9092;
require_once(dirname(__FILE__). "/client.php");
require_once("/www/vae_remote/current/lib/constants.php");
require_once("/www/vae_remote/current/lib/general.php");
require_once("/www/vae_remote/current/lib/store.php");
require_once("/www/vae_remote/current/lib/phpapi.php");
require_once("/www/vae_remote/current/lib/vaedata.php");
require_once("/www/vae_remote/current/lib/vae_exception.php");
$_VAE['settings']['subdomain'] = "btg";
$_VAE['staging'] = true;
$_VAE['config']['secret_key'] = "9dee6740e4ddbe2c9488242318b764beeef7a17f";
//$_VAE['settings']['subdomain'] = "peeptoe";
//$_VAE['config']['secret_key'] = "9dee6740e4ddbe2c9488242318b764beeef7a17f";
//$_VAE['settings']['subdomain'] = "hintmagazine";
//$_VAE['config']['secret_key'] = "8021e63777c6fe00a7a0ab660b29780d0e9fee25";
//$_VAE['settings']['subdomain'] = "impose";
//$_VAE['config']['secret_key'] = "4dd583d746bcaaac042a0a132e6971b245fd174f";
$k = microtime(true);

$items = vae("artists");
foreach ($items as $r) {
  echo $r->name . "\n";
}

echo "\n" . (microtime(true)-$k) . " seconds elapsed\n";

?>
