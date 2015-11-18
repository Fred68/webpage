<?php

// COMPLETARE PRIMA LE PROVE SU f.inc
// POI SCRIVERE LA CLASSE

class EncSession
	{
		
	$sep = "*";												// Separatore
	$keystrval   = "11111111111111111111111111111111";		// Password (fittizia) 32 caratteri 
		
	function __construct()
		{
		print "Session constructor\n";
		}
	
	
	
	
	
	
	function addpadding($string, $blocksize = 32)			// Aggiunge padding PKCS7
		{													// Thanks to http://blog.djekldevelopments.co.uk/?p=334)
	    $pad = $blocksize - (strlen($string) % $blocksize);
	    $string .= str_repeat(chr($pad), $pad);
	    return $string;
		}
	function strippadding($string)							// Elimina padding
		{													// Thanks to http://blog.djekldevelopments.co.uk/?p=334)
		$slast = ord(substr($string, -1));
		$slastc = chr($slast);
		$pcheck = substr($string, -$slast);
		if(preg_match("/$slastc{".$slast."}/", $string))
			{
			$string = substr($string, 0, strlen($string)-$slast);
			return $string;
			}
		else
			{
			return false;
			}
		}
	function encrypt($string="", $keystr)							// Criptatura messaggio
		{
		$msg = "";
		$keystrval = $_SESSION['keystr'];
		$enc = "";											// Messaggio criptato (inizializzato a zero)
		$size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_CBC);	// Dimensione dell'iv
		$ivstrauto = mcrypt_create_iv ($size);				// Creazione di un iv casuale [in quale formato ?]
		$key = base64_decode($keystrval);
		$enc = mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $key, addpadding($string), MCRYPT_MODE_CBC, $ivstrauto);	// Codifica
		$msg = base64_encode($enc).$sep.base64_encode($ivstrauto);				// Unisce messaggio criptato, sepratore e iv
		return $msg;
		}
	}

	

?>