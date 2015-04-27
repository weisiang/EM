#!/usr/bin/php5 -q
<?php
	//*************************************
	$file="";
	$lang = "en";
	$length = 12;
	$options = getopt("i:l:d:n:h");
	if (count($options))
	{
		reset($options);
		while(list($key, $value) = each($options)) {
			switch ($key) {
				case 'i':
					$file = $value;
					break;
				case 'l':
					$lang = $value;
					break;
				case 'n':
					$length = $value;
					break;
				default:
					help();
					break;
			}
		}
	}
	else
	{
		//echo "else";
		help();
	}
	if (!$file)
	{
		//echo "null";
		help();
	}
	function help()
	{
		echo "\t-i input probability data.\r\n\t-l this file used language.\r\n\t-n this file max length.\r\n\t-h help message.\r\n";
		exit;
	}
	$dbhost = 'localhost';
	$dbuser = 'root';
	$dbpass = 'nlpgroup';
	$dbname = 'phrase';
	$conn = mysql_connect($dbhost, $dbuser, $dbpass) or die('Error with MySQL connection');
	mysql_query("SET NAMES 'utf8'");
	mysql_select_db($dbname);
	$read = file($file);
	for($i=0; $i<count($read); $i++)
	{
		$line = explode("\t\t", trim($read[$i]));
		$sql = "insert into `phrase_thr_1_max_$length"."_$lang"."_probability` (phrase, probability) values ('$line[1]', '$line[0]')";
		$result = mysql_query($sql) or die('MySQL query error '.$sql);
	}
	echo "OK";
?>
