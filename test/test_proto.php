#!/usr/bin/php
<?php

print("Test PUT\n");
$sock = fsockopen('localhost', 11138);
fwrite($sock, chr(1), 1);
$data = fread($sock, 4096);
//fclose($sock);
//file_put_contents('result_put', $data);
//print("=> '$data'\n");
show_response($data);

print("Test GET\n");
//$sock = fsockopen('localhost', 11138);
fwrite($sock, chr(2), 1);
$data = fread($sock, 4096);
//fclose($sock);
//file_put_contents('result_get', $data);
//print("=> '$data'\n");
show_response($data);

print("Test DEL\n");
//$sock = fsockopen('localhost', 11138);
fwrite($sock, chr(3), 1);
$data = fread($sock, 4096);
//fclose($sock);
//file_put_contents('result_del', $data);
//print("=> '$data'\n");
show_response($data);

function show_response($data) {
	$primeCode = ord($data[0]);
	$hasData = $primeCode & 0x40;
	$code = $primeCode & 0x3f;
	if ($code == 1)
		print("[OK]");
	else if ($code == 2)
		print("[BAD CMD]");
	else if ($code == 3)
		print("[PROTOCOL ERR]");
	else
		print("(unkown response)");
	print(" ($primeCode / $code)\n");
}
