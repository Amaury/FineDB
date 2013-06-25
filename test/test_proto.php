#!/usr/bin/php
<?php

/**
 * PHP program used to test FineDB server.
 *
 * @author	Amaury Bouchard <amaury@amaury.net>
 * @copyright	© 2013, Amaury Bouchard
 */

require_once('finebase/Ansi.php');
require_once('finebase/FineTimer.php');
require_once('finebase/FineDatasource.php');
require_once('finebase/FineNDB.php');
require_once('finebase/FineCache.php');

const FINEDB_HOST = 'localhost';
const FINEDB_PORT = 11138;
const REDIS_DSN = 'redis://localhost:6379/0';
const MEMCACHE_DSN = 'memcache://localhost:11211';

$nbrLoops = 15;
$dataList = array(
	'aaa'			=> 'bbb',
	'trululu pouet pouet'	=> 'tagada tsouin tsouin',
	'foobar'		=> 'fucked up beyond any recovery',
	'Pere Noel'		=> 'Santa Claus'
);
$timer = new FineTimer();
$ndb = FineNDB::factory(REDIS_DSN);
$cache = FineCache::factory(MEMCACHE_DSN);

// FineDB
print(Ansi::bold("FineDB\n"));
$sock = fsockopen(FINEDB_HOST, FINEDB_PORT);
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		commandPut($sock, "$key-$i", $data);
}
$timer->stop();
print("PUT : " . $timer->getTime() . "\n");
fclose($sock);
$sock = fsockopen(FINEDB_HOST, FINEDB_PORT);
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		commandGet($sock, "$key-$i");
}
$timer->stop();
print("GET : " . $timer->getTime() . "\n");
fclose($sock);

// Redis
print(Ansi::bold("Redis\n"));
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		$ndb->set("$key-$i", $data);
}
$timer->stop();
print("PUT : " . $timer->getTime() . "\n");
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		$ndb->get("$key-$i");
}
$timer->stop();
print("GET : " . $timer->getTime() . "\n");

// Memcache
print(Ansi::bold("Memcache\n"));
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		$cache->set("$key-$i", $data);
}
$timer->stop();
print("PUT : " . $timer->getTime() . "\n");
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		$result = $cache->get("$key-$i");
}
$timer->stop();
print("GET : " . $timer->getTime() . "\n");

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
	//showResponse($response);
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
	//showResponse($response);
	//print("'$key' => '" . mb_substr($response, 5, 4096, 'ascii') . "'\n");
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
