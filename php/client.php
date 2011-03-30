<?php

function _verb_thrift($port = 9090) {
  global $_VERB;
  if ($_VERB['verbrubyd']) return $_VERB['verbrubyd'];
  $_VERB['verbrubyd'] = _verb_thrift_open("VerbRubydClient", $port);
  return $_VERB['verbrubyd'];
};

function _verb_dbd($port = 9091) {
  global $_VERB;
  if ($_VERB['verbdbd_port']) $port = $_VERB['verbdbd_port'];
  if ($_VERB['verbdbd']) return $_VERB['verbdbd'];
  $_VERB['verbdbd'] = _verb_thrift_open("VerbDbClient", $port);
  return $_VERB['verbdbd'];
};

function _verb_thrift_open($client_class, $port) {
  $GLOBALS['THRIFT_ROOT'] = dirname(__FILE__) . '/vendor/thrift';
  require_once $GLOBALS['THRIFT_ROOT'].'/Thrift.php';
  require_once $GLOBALS['THRIFT_ROOT'].'/protocol/TBinaryProtocol.php';
  require_once $GLOBALS['THRIFT_ROOT'].'/transport/TSocket.php';
  require_once $GLOBALS['THRIFT_ROOT'].'/transport/TBufferedTransport.php';
  require_once $GLOBALS['THRIFT_ROOT'].'/../../../gen-php/verb/VerbDb.php';
  require_once $GLOBALS['THRIFT_ROOT'].'/../../../gen-php/verb/VerbRubyd.php';
  require_once $GLOBALS['THRIFT_ROOT'].'/../../../gen-php/verb/verb_types.php';
  try {
    $socket = new TSocket('localhost', $port);
    $socket->setRecvTimeout(30000);
    $transport = new TBufferedTransport($socket, 1024, 1024);
    $protocol = new TBinaryProtocol($transport);
    $client = new $client_class($protocol);
    $transport->open();
    return $client;
  } catch (TException $tx) {
    throw new VerbException("", $tx->getMessage());
  }
}

?>
