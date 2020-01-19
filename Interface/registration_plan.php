<?php
$domain = GetHostByName($REMOTE_ADDR);
$link = mysql_connect('mysql103.mysite4now.com', 'easyvoip', 'SIMJA');
mysql_select_db('easyvoip',$link) ;

$result = mysql_db_query("easyvoip", "select * from version");
$thisrow=mysql_fetch_row($result);
echo $thisrow[0].','.$thisrow[1];
mysql_close($link);
?>


