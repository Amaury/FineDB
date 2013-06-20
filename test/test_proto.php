#!/usr/bin/php
<?php

print("Test PUT\n");
$sock = fsockopen('localhost', 11138);
fwrite($sock, chr(1), 1);
$data = fread($sock, 4096);
print("=> '$data'\n");

print("Test GET\n");
//$sock = fsockopen('localhost', 11138);
fwrite($sock, chr(2), 1);
$data = fread($sock, 4096);
print("=> '$data'\n");

print("Test DEL\n");
//$sock = fsockopen('localhost', 11138);
fwrite($sock, chr(3), 1);
$data = fread($sock, 4096);
print("=> '$data'\n");

