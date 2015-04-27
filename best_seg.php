#!/usr/bin/php5 -q
<?php
	$data = "";
	$lang = "en";
	$length = 12;
	$options = getopt("i:l:n:h");

	if (count($options))
	{
		reset($options);
		while(list($key, $value) = each($options))
		{
			switch ($key)
			{
				case 'i':
					$data = $value;
					break;
				case 'l':
					$lang = $value;
					breaK;
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
		help();
	}

	if(!$data)
	{
		help();
	}

	function help()
	{
		echo "\t-i input file.\r\n\t-l corpus language. ex: en, fr.\r\n\t-n max phrase length.\r\n\t-h help message.\r\n";
		exit;
	}

	$file = file($data);
	$dbhost = 'localhost';
	$dbuser = 'root';
	$dbpass = 'nlpgroup';
	$dbname = 'phrase';
	$conn = mysql_connect($dbhost, $dbuser, $dbpass) or die('Error with MySQL connection');
	mysql_query("SET NAMES 'utf8'");
	mysql_select_db($dbname);

	for ($i=0; $i<count($file); $i++)
	{
		$sum=-1;
		$sid = explode(" ", $file[$i]);
		for ($j=0; $j<count($sid); $j++)
		{
			$str = str_replace('$',"",$sid[$j]);
			$str = str_replace('#','',$str);
			$str = intval($str);
			$sql = "select len from `phrase-thr-1-max-$length-$lang` where id=$str";
			$result = mysql_query($sql) or die('MySQL query error '.$sql);
			if (mysql_num_rows($result))
			{
				$info = mysql_fetch_array($result);
				//echo $info['len']. " ";
				$sum=$sum+$info['len'];
				echo $sum." ";
			}
		}
		echo "\n";
	}
	
	// clean MySQL en and fr phrase table's data
	//$sql1 = "TRUNCATE TABLE `phrase_thr_1_max_$length"."_en_probability`";
	//$result1 = mysql_query($sql1) or die('MySQL query error '.$sql1);
	//$sql2 = "TRUNCATE TABLE `phrase_thr_1_max_$length"."_fr_probability`";
        //$result2 = mysql_query($sql2) or die('MySQL query error '.$sql2);
	//$sql3 = "TRUNCATE TABLE `phrase-thr-1-max-$length-en`";
        //$result3 = mysql_query($sql3) or die('MySQL query error '.$sql3);
	//$sql4 = "TRUNCATE TABLE `phrase-thr-1-max-$length-fr`";
	//$result4 = mysql_query($sql4) or die('MySQL query error '.$sql4);
?>
