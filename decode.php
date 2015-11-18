<?php
session_start();
include 'php/f.inc';
$msg = $_POST["msg"];
echo decrypt($msg);
?>
