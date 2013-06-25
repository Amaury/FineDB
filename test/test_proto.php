#!/usr/bin/php
<?php

/**
 * PHP program used to test FineDB server.
 *
 * @author	Amaury Bouchard <amaury@amaury.net>
 * @copyright	© 2013, Amaury Bouchard
 */

require_once('finebase/FineTimer.php');

const HOST = 'localhost';
const PORT = 11138;

$timer = new FineTimer();

print("PUT test\n");
$sock = fsockopen(HOST, PORT);
$timer->start();
commandPut($sock, 'aaa', 'xyz');
commandPut($sock, 'ccc', 'ddd');
commandPut($sock, 'eee', 'fff');
$timer->stop();
print("...OK (" . $timer->getTime() . ")\n");
fclose($sock);

print("GET test\n");
$sock = fsockopen(HOST, PORT);
$timer->start();
commandGet($sock, 'aaa');
commandGet($sock, 'ccc');
commandGet($sock, 'eee');
$timer->stop();
print("...OK (" . $timer->getTime() . ")\n");
fclose($sock);

// compare with REDIS
require_once('finebase/FineDatasource.php');
require_once('finebase/FineNDB.php');

const REDIS_DSN = 'redis://serv1.finemedia.fr:6379/0';

$ndb = FineNDB::factory(REDIS_DSN);

print("REDIS PUT\n");
$timer->start();
$ndb->set('aaa', 'xyz');
$ndb->set('ccc', 'ddd');
$ndb->set('eee', 'fff');
$timer->stop();
print("...OK (" . $timer->getTime() . ")\n");

print("REDIS GET\n");
$result = $ndb->get('aaa');
print("'aaa' => '$result'\n");
$result = $ndb->get('ccc');
print("'ccc' => '$result'\n");
$result = $ndb->get('eee');
print("'eee' => '$result'\n");
$timer->stop();
print("...OK (" . $timer->getTime() . ")\n");

require_once('finebase/FineNDB.php');


/**
 * Send data to FineDB server.
 * @param	stream	$sock	Socket descriptor.
 * @param	string	$key	Key of the data.
 * @param	string	$data	Data to send.
 */
function commandPut($sock, $key, $data) {
	$buffer = chr(1) . pack('n', mb_strlen($key, 'ascii')) . $key .
		  pack('N', mb_strlen($data, 'ascii')) . $data;
	fwrite($sock, $buffer);
	$response = fread($sock, 4096);
	showResponse($response);
}
/**
 * Fetch data from FineDB server.
 * @param	stream	$sock	Socket descriptor.
 * @param	string	$key	Key of the data.
 */
function commandGet($sock, $key) {
	$buffer = chr(2) . pack('n', mb_strlen($key, 'ascii')) . $key;
	fwrite($sock, $buffer);
	$response = fread($sock, 4096);
	showResponse($response);
	print("'$key' => '" . mb_substr($response, 5, 4096, 'ascii') . "'\n");
}
/**
 * Show the status of a response from FineDB server.
 * @param	string	$data	The data received from the server.
 */
function showResponse($data) {
	$primeCode = ord($data[0]);
	$hasData = $primeCode & 0x40;
	$code = $primeCode & 0x3f;
	if ($code == 0)
		print("[OK]");
	else if ($code == 1)
		print("[BAD CMD]");
	else if ($code == 2)
		print("[PROTOCOL ERR]");
	else if ($code == 3)
		print("[SERVER ERR]");
	else if ($code == 4)
		print("[UNKOWN]");
	else
		print("(unkown response)");
	print(" ($primeCode / $code)\n");
}
