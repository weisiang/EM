#!/usr/bin/php5 -q
<?php
	//*************************************
	$corpus="";
	$database="";
	$lang = "en";
	$length = 12;
	$options = getopt("i:l:d:n:h");
	if (count($options))
	{
		reset($options);
		while(list($key, $value) = each($options)) {
			switch ($key) {
				case 'i':
					$corpus = $value;
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
	if (!$corpus)
	{
		//echo "null";
		help();
	}
	function help()
	{
		echo "\t-i input corpus data.\r\n\t-d phrase database file.\r\n\t-l this corpus language. eq. fr or en\r\n\t-h help message.\r\n";
		exit;
	}
	//*************************************
	$dbhost = 'localhost';
	$dbuser = 'root';
	$dbpass = 'nlpgroup';
	$dbname = 'phrase';
	$conn = mysql_connect($dbhost, $dbuser, $dbpass) or die('Error with MySQL connection');
	mysql_query("SET NAMES 'utf8'");
	mysql_select_db($dbname);
	//*************************************
	//*************************************
	//$line = explode(" ", "mr president , mr president-in-office of the council , ladies and gentlemen , thank you for your kind accolade and thank you also to those who have gently reproached me for not making this speech earlier .");
	//$db->exec('CREATE TABLE IF NOT EXISTS phrase(id INTEGER PRIMARY KEY, phrase text, num INTEGER)');
	$i=0;
	//$file = fopen("Test.lowercase.en", "r");
	$file = file($corpus);
	for ($i=0; $i<count($file); $i++)
	{
        $line = explode(" ", $file[$i]);
        if (count($line))
        {
        	echo getPhrase($line, $length, $lang)."\n";
        	//print_r(getPhrase($db, $line, 4, $delimited));
        }
	}
	
	//echo getPhrase($db, $line, 4);
	function getPhrase($line, $max, $lang)
	{
		global $conn;
		$initial = log10(1.0);
		$len = count($line)-1; //字串長度
		//$i = 1; //預設第一個字
		$tmp = array();
		$op = "";
		//echo $len."<br />";
		for ($i=0; $i<=$len; $i++)
		{
			$term='';
			$l =1;
			$tmp[$i][0][0]="";
			$tmp[$i][0][1]="";
			$tmp[$i][0][2]=log10(0);
			for($j=$i; $j>$i-$max; $j--)
			{
				
				if ($j>=0)
				{
					$term = $line[$j]." ".$term;
					//$term_sql = $db->escapeString(trim($term));
					$term_sql = addslashes(trim($term));
					$sql = "SELECT probability  FROM `phrase_thr_1_max_".$max."_".$lang."_probability` where phrase='$term_sql'";
					$result = mysql_query($sql, $conn) or die('MySQL query error '.$sql);
					$tmp[$i][$l][0] = trim($term);
					$tmp[$i][$l][1] = count(explode(" ", $tmp[$i][$l][0]));
					//$tmp[$i][$l][2] = $tmp[$i][$l][2];
					if (!mysql_num_rows($result))
					{
						$row = mysql_fetch_array($result);
						if ($i)
						{
							$pt = $i-$l;
							//if ($pt<0) echo $i."\r\n".$l."\r\n".$line."\r\n"; exit;
							if ($pt>=0)
							{
								$tmp[$i][$l][2]=$row['probability'] + $tmp[$pt][0][2]; //$tmp[$i-1][0][2] 每個欄位最大值
							}
							else
							{
								$tmp[$i][$l][2]=$row['probability'] + $initial;
							}
						}
						else
						{
							$tmp[$i][$l][2]=$row['probability'] + $initial; 
						}
					}
					else
					{
						$tmp[$i][$l][2]=log10(0);
					}
					//echo "tmp[".$i."][".$l."][0] = ".$tmp[$i][$l][2]."\r\n";
				}
				else
				{
					break;
					//$tmp[$i][$l][0] = "";
					//$tmp[$i][$l][1] = 0;
					//$tmp[$i][$l][2] = log10(0);
					//echo "tmp[".$i."][".$l."][0] = ".$tmp[$i][$l][2]."\r\n";
				}
				//開始計算最大值
				
				if ($tmp[$i][0][2]<$tmp[$i][$l][2])
				{
					$tmp[$i][0][0] = $tmp[$i][$l][0];
					$tmp[$i][0][1] = $tmp[$i][$l][1];
					$tmp[$i][0][2] = $tmp[$i][$l][2];
				}
				$l++;
			}
		}
		//*************************************
		//$op = "最佳段法：\r\n";
		//$op="";
		//執行輸出
		//return $tmp;
		$output = array();
		for ($i=$len; $i>=0; $i--)
		{
			$output[] = $tmp[$i][0][0]; //先放入緩衝區
			$i=$i-$tmp[$i][0][1]+1;
		}
		
		//unlink($tmp);
		for ($i=count($output)-1; $i>=0; $i--)
		{
			$phrase = addslashes(trim($output[$i]));
			$sql = "select * from `phrase-thr-1-max-$max-$lang` where phrase = '$phrase'";
			$result = mysql_query($sql, $conn) or die('MySQL query error '.$sql);
			if (!mysql_num_rows($result))
			{
				$lens = count(explode(" ", $phrase));
				$sql = "insert into `phrase-thr-1-max-$max-$lang` (phrase,len)values('$phrase',$lens)";
				$result = mysql_query($sql, $conn) or die('MySQL query error '.$sql);
				if ($lang=="fr")
				{
					$op.= "$#".mysql_insert_id($conn). "# ";
				}
				else
				{
					$op.= "$".mysql_insert_id($conn). "# ";
				}
			}
			else
			{
				$info = mysql_fetch_array($result);
				if ($lang=="fr")
				{
					$op.= "$#".$info['id']. "# ";
				}
				else
				{
					$op.= "$".$info['id']. "# ";
				}
			}
		}
		//unlink($output);
		//$op.= "\n";
		return $op;
	}
?>
