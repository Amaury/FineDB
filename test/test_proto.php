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
require_once('finebase/FineDatabase.php');
require_once('finebase/FineNDB.php');
require_once('finebase/FineCache.php');

const FINEDB_HOST = 'localhost';
const FINEDB_PORT = 11138;
const REDIS_DSN = 'redis://localhost:6379/0';
const MEMCACHE_DSN = 'memcache://localhost:11211';
const MYSQL_DSN = 'mysqli://finedb:finedb@localhost/finedb';

$nbrLoops = 45;
$dataList = array(
	'aaa'			=> "{\"type\":\"question\",\"ip\":\"\",\"text\":\"<p>Bonjour,\\n<br \\/>Je voudrais savoir qu'elle est la diff\\u00e9rence entre un vitrage de 4:16:4 et un de 4:20:4 (gaz argon en tr\\u00e8s les deux vitres, et l'int\\u00e9r\\u00eat de mettre un vitrage de 6:16:4) ?<\\/p>\\n\\n<p>Merci de vos r\\u00e9ponses<\\/p>\\n\\n<p>Mick<\\/p>\",\"tags\":\"fen\\u00eatre argon double vitrage gaz argon dimension \\u00e9paisseur\"}",
	'trululu pouet pouet'	=> "{\"type\":\"question\",\"ip\":\"2.8.152.24\",\"text\":\"<p>Bonjour,<br \\/>Nous venons d'acheter une maison sur laquelle il reste encore quelques travaux \\u00e0 faire.<br \\/>Nous nous sommes rendu compte qu'il n'y avait pas de grille ou baguette d'a\\u00e9ration aux fen\\u00eatres.D'apr\\u00e8s certaines personnes, ce n'est pas obligatoire, mais \\u00e7a d\\u00e9pend de la maison.<br \\/>Voici un bref r\\u00e9sum\\u00e9 des lieux :<br \\/>Chambre 1 : 1 fen\\u00eatre, pas de grille d'a\\u00e9rationChambre 2 : 1 fen\\u00eatre, 1 salle d'eau ouverte sur la chambre avec VMCSalle de bain : VMCWC : VMCSalon : 1 fen\\u00eatre et une baie vitr\\u00e9e coulissante sans grille d'a\\u00e9ration donnant sur une cuisine am\\u00e9ricaine avec une fen\\u00eatre et une porte fen\\u00eatre sans grille d'a\\u00e9ration non plus, mais VMC et hotte dans la cuisine.<br \\/>Faut-il que l'on rajoute des grilles d'a\\u00e9ration \\u00e0 nos fen\\u00eatres ?<br \\/>Merci pour vos r\\u00e9ponse.<\\/p>\",\"tags\":\"fenetre grille aeration obligatoire\"}",
	'foobar'		=> "{\"type\":\"question\",\"ip\":\"82.225.215.131\",\"text\":\"<p>Bonjour,<\\/p>\\n\\n<p><br \\/>Je souhaite changer les fen\\u00eatres de mon appartement et obtenir ainsi une bonne isolation thermique et phonique et dans le m\\u00eame temps il faut mettre des grilles d'a\\u00e9ration, puisque j'ai une VMC simple flux.Est-ce qu'il existe des fen\\u00eatres avec une isolation phonique et des grilles de ventilations?Merci d'avance pour vos r\\u00e9ponses.<\\/p>\"}",
	'Pere Noel'		=> "{\"type\":\"question\",\"ip\":\"86.193.36.153\",\"text\":\"<p>Bonjour,<\\/p>\\n\\n<p><br \\/>je dois changer des cales anti d\\u00e9gondage sur deux vitrages coulissants en alu datant des ann\\u00e9es 1970. Ceux que je dois changer sont en plastique blanc sans marque mentionn\\u00e9e hormis le chiffre 077.Je n'en connais pas la marque. O\\u00f9 peut on se procurer ces cales svp merci ?<\\/p>\",\"tags\":\"fen\\u00eatre baie vitr\\u00e9e coulissante\"}"
);
$timer = new FineTimer();
$ndb = FineNDB::factory(REDIS_DSN);
$cache = FineCache::factory(MEMCACHE_DSN);
$couch = new Couchbase('localhost:8091', '', '', 'default');
$db = FineDatabase::factory(MYSQL_DSN);
$mongo = new MongoClient();
$mongodb = $mongo->selectDB('finedb');

// ---------- FineDB
print(Ansi::bold("FineDB\n"));
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data) {
		$sock = fsockopen(FINEDB_HOST, FINEDB_PORT);
		commandPut($sock, false, false, "$key-x$i", $data);
		fclose($sock);
	}
}
$timer->stop();
print("PUT ASYNC : " . $timer->getTime() . "\n");
exit();
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data) {
		$sock = fsockopen(FINEDB_HOST, FINEDB_PORT);
		commandPut($sock, true, false, "$key-$i", $data);
		fclose($sock);
	}
}
$timer->stop();
print("PUT SYNC  : " . $timer->getTime() . "\n");
//sleep(5);
$sock = fsockopen(FINEDB_HOST, FINEDB_PORT);
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		commandGet($sock, false, "$key-$i", $data);
}
$timer->stop();
fclose($sock);
print("GET       : " . $timer->getTime() . "\n");
exit();


// ---------- MongoDB
print(Ansi::bold("MongoDB\n"));
$mongodb->finedb->drop();
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data) {
		$mongodb->finedb->insert(array(
			'_id'	=> "$key-$i",
			'data'	=> $data
		));
	}
}
$timer->stop();
print("PUT ASYNC : " . $timer->getTime() . "\n");
$mongodb->finedb->drop();
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data) {
		$mongodb->finedb->insert(array(
			'_id'	=> "$key-$i",
			'data'	=> $data
		), array(
			'fsync'	=> true,
			'w'	=> true
		));
	}
}
$timer->stop();
print("PUT SYNC  : " . $timer->getTime() . "\n");
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data) {
		$value = $mongodb->finedb->findOne(array(
			'_id'	=> "$key-$i"
		));
	}
}
$timer->stop();
print("GET       : " . $timer->getTime() . "\n");
exit();


// ---------- MySQL
print(Ansi::bold("MySQL\n"));
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data) {
		$sql = "INSERT INTO Data
			(name, data)
			VALUES ('$key-$i', '" . $db->quote($data) . "')
			ON DUPLICATE KEY UPDATE data = '" . $db->quote($data) . "'";
		$db->exec($sql);
	}
}
$timer->stop();
print("PUT       : " . $timer->getTime() . "\n");
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data) {
		$sql = "SELECT data
			FROM Data
			WHERE name = '$key-$i'";
		$result = $db->queryOne($sql);
	}
}
$timer->stop();
print("GET       : " . $timer->getTime() . "\n");
exit();


// ---------- Couchbase
print(Ansi::bold("Couchbase\n"));
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		$couch->set("$key-$i", $data);
}
$timer->stop();
print("PUT       : " . $timer->getTime() . "\n");
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		$couch->get("$key-$i");
}
$timer->stop();
print("GET       : " . $timer->getTime() . "\n");

exit();


// ---------- Redis
print(Ansi::bold("Redis\n"));
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		$ndb->set("$key-$i", $data);
}
$timer->stop();
print("PUT       : " . $timer->getTime() . "\n");
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		$ndb->get("$key-$i");
}
$timer->stop();
print("GET       : " . $timer->getTime() . "\n");


// ---------- Memcache
print(Ansi::bold("Memcache\n"));
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		$cache->set("$key-$i", $data);
}
$timer->stop();
print("PUT       : " . $timer->getTime() . "\n");
$timer->start();
for ($i = 0; $i < $nbrLoops; $i++) {
	foreach ($dataList as $key => $data)
		$result = $cache->get("$key-$i");
}
$timer->stop();
print("GET       : " . $timer->getTime() . "\n");

/**
 * Send data to FineDB server.
 * @param	stream	$sock		Socket descriptor.
 * @param	bool	$sync		True if synchronized.
 * @param	bool	$compress	True if the data is compressed.
 * @param	string	$key		Key of the data.
 * @param	string	$data		Data to send.
 */
function commandPut($sock, $sync, $compress, $key, $data) {
	if (!$sync && !$compress)
		$command = chr(0x21);
	else if (!$sync && $compress)
		$command = chr(0x61);
	else if ($sync && !$compress)
		$command = chr(0x29);
	else if ($sync && $compress)
		$command = chr(0x69);
	$buffer = $command . pack('n', mb_strlen($key, 'ascii')) . $key .
		  pack('N', mb_strlen($data, 'ascii')) . $data;
	fwrite($sock, $buffer);
	$response = fread($sock, 4096);
	//showResponse($response);
}
/**
 * Fetch data from FineDB server.
 * @param	stream	$sock		Socket descriptor.
 * @param	bool	$compress	True if the data is compressed.
 * @param	string	$key		Key of the data.
 * @param	string	$check		(optional) Data that should have been retreived.
 */
function commandGet($sock, $compress, $key, $check=null) {
	$command = $compress ? chr(0x40) : chr(0);
	$buffer = chr(0) . pack('n', mb_strlen($key, 'ascii')) . $key;
	fwrite($sock, $buffer);
	$response = fread($sock, 4096);
	//showResponse($response);
	/*
	$response = mb_substr($response, 5, 4096, 'ascii');
	if (isset($check) && $response != $check)
		print("ERR GET '$key' => '$response' (instead of '$check')\n");
	print("'$key' => '$response'\n");
	*/
}
/**
 * Show the status of a response from FineDB server.
 * @param	string	$data	The data received from the server.
 */
function showResponse($data) {
	$primeCode = ord($data[0]);
	$hasData = $primeCode & 0x40;
	$code = $primeCode & 0x3;
	if ($code == 0)
		print("[OK]");
	else if ($code == 1)
		print("[PROTOCOL ERR]");
	else if ($code == 2)
		print("[SERVER ERR]");
	else if ($code == 3)
		print("[NO DATA]");
	else if ($code == 4)
		print("[UNKOWN ERR]");
	else
		print("(unkown response)");
	print(" ($primeCode / $code)\n");
}
