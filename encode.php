<?php
session_start();
include 'php/f.inc';
$msg = $_POST["msg"];
echo encrypt($msg);			// Solo per verifica, non restituire dati in chiaro o codificati in base64
?>
