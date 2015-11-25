<?php
// Variabili globali
$timeout = 60; // timeout in secondi
$pausaWrong = 1; // Pausa se utente errato
$sep = "*"; // Separatore
$keystrval = "11111111111111111111111111111111"; // Password (fittizia) 32 caratteri

$host = "127.0.0.1"; // Server name
$username = "root"; // Mysql username DA CAMBIARE
$password = ""; // Mysql password DA AGGIORNARE

$db_name = "my_fred68"; // Nome database per il login
$pertdb = "pert01"; // Nome database del pert (eventualmente identico)

$tbl_name = "members"; // Tabella utenti
$tbl_log = "logged"; // Tabella log

$pwdColumn = "passwd"; // Nome della colonna con la password criptata
$keystr = "keystr"; // Nome colonna con chiave di criptatura
$pwddb = "pwddb"; // Nome colonna con password database
$usrColumn = "usrname"; // Nome della colonna utente (anche per la tabella $tbl_log)
$sidColumn = "sid"; // Nome della colonna con session_id
$lgtmColumn = "lgtm"; // Nome della colonna con log time
$lstlgColumn = "lstlg";
 // Nome della colonna con last log
function serverPost($t1, $t2, $sec) // FUNZIONE DI PROVA
	{
	sleep($sec);
	$txt = $t1 . "\n" . $t2 . "\n" . microtime();
	return $txt;
	}

function addpadding($string, $blocksize = 32) // Aggiunge padding PKCS7
	{ // Thanks to http://blog.djekldevelopments.co.uk/?p=334)
	$pad = $blocksize - (strlen($string) % $blocksize);
	$string .= str_repeat(chr($pad), $pad);
	return $string;
	}

function strippadding($string) // Elimina padding
	{ // Thanks to http://blog.djekldevelopments.co.uk/?p=334)
	$slast = ord(substr($string, - 1));
	$slastc = chr($slast);
	$pcheck = substr($string, - $slast);
	if(preg_match("/$slastc{" . $slast . "}/", $string))
		{
		$string = substr($string, 0, strlen($string) - $slast);
		return $string;
		}
	else
		{
		return false;
		}
	}

function encrypt($string = "") // Criptatura messaggio
	{
	global $sep;
	$msg = "";
	if(isset($_SESSION['keystr']))
		{
		$keystrval = $_SESSION['keystr'];
		$enc = ""; // Messaggio criptato (inizializzato a zero)
		$size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_CBC); // Dimensione dell'iv
		$ivstrauto = mcrypt_create_iv($size); // Creazione di un iv casuale [in quale formato ?]
		$key = base64_decode($keystrval);
		$enc = mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $key, addpadding($string), MCRYPT_MODE_CBC, $ivstrauto); // Codifica
		$msg = base64_encode($enc) . $sep . base64_encode($ivstrauto); // Unisce messaggio criptato, sepratore e iv
		}
	return $msg;
	}

function decrypt($msg = "") // Decriptatura messaggio
	{
	global $sep;
	$ret = "";
	if(isset($_SESSION['keystr']))
		{
		$keystrval = $_SESSION['keystr'];
		$key = base64_decode($keystrval);
		$iv = substr($msg, strrpos($msg, $sep) + 1); // Estrae l'iv
		$msg = str_replace($sep . $iv, "", $msg); // Elimina iv e separatore
		$string = base64_decode($msg);
		$ret = strippadding(mcrypt_decrypt(MCRYPT_RIJNDAEL_256, $key, $string, MCRYPT_MODE_CBC, base64_decode($iv)));
		}
	return $ret;
	}

function connectIntDB(&$err) // Connette al database interno
	{
	global $host, $db_name, $username, $password;
	$conn = null;
	$ok = false;
	try
		{ // Apre connessione a database
		$conn = new PDO("mysql:host=$host;dbname=$db_name", $username, $password);
		$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
		$ok = true;
		}
	catch (PDOException $e) // Se errore, chiude la connessione
		{
		$err .= $e->getMessage();
		$conn = null;
		}
	return $conn;
	}

function countUsers($uname, $upwd, $con, &$err) // Conta gli utenti del database interno, con nome $uname e password $upwd
	{
	global $tbl_name, $usrColumn, $pwdColumn;
	$cnt = 0;
	if($con != null)
		{
		$sql = "SELECT COUNT(*) FROM $tbl_name WHERE " . $usrColumn . "='$uname' AND " . $pwdColumn . "='$upwd'";
		try
			{
			$stmt = $con->prepare($sql);
			$stmt->execute();
			$cnt = $stmt->fetchColumn(); // Ottiene il numero di record con utente e pwd (criptata) -> $count
			}
		catch (PDOException $e)
			{
			$err .= $e->getMessage();
			$cnt = 0;
			}
		}
	return $cnt;
	}

function getUserKeyPwd($uname, $upwd, $con, &$key, &$dbpw, &$err) // Legge keystring e dbPassword dell'utente $uname con password $upwd, da database interno
	{
	global $usrColumn, $pwdColumn, $tbl_name, $keystr, $pwddb;
	$ok = false;
	if($con != null)
		{
		$sql = "SELECT " . $keystr . ", " . $pwddb . " FROM $tbl_name WHERE " . $usrColumn . "='$uname' AND " . $pwdColumn . "='$upwd'";
		try
			{
			$stmt = $con->prepare($sql);
			$stmt->execute();
			$rows = $stmt->fetchAll(); // Estrae tutte le righe della query
			$row = $rows[0]; // Sceglie la prima (ed unica)
			$key = $row[0]; // Legge la chiave
			$dbpw = $row[1]; // Legge la password del db
			$ok = true;
			}
		catch (PDOException $e)
			{
			$err .= $e->getMessage();
			$ok = false;
			}
		}
	return $ok;
	}

function countLoggedUsers($uname, $con, &$err, $sID = null) // Conta gli connessi con $uname (e $sID eventuale)
	{
	global $usrColumn, $tbl_log, $sidColumn;
	$cnt = 0;
	if($con != null)
		{
		$sql = "SELECT COUNT(*) FROM $tbl_log WHERE " . $usrColumn . "='$uname'";
		
		if($sID != null)
			{
			$sql .= " AND " . $sidColumn . " = '" . $sID . "'";
			}
		
		try
			{
			$stmt = $con->prepare($sql);
			$stmt->execute();
			$cnt = $stmt->fetchColumn(); // Ottiene il numero di record con utente e pwd (criptata) -> $count
			}
		catch (PDOException $e)
			{
			$err .= $e->getMessage();
			$cnt = 0;
			}
		}
	return $cnt;
	}

function insertLoggedUser($uname, $id, $con, &$err) // Inserisce i dati dell'utente connesso
	{
	global $usrColumn, $tbl_log, $sidColumn;
	
	$sql = "INSERT INTO " . $tbl_log . "(" . $sidColumn . ", " . $usrColumn . ") VALUES('" . $id . "', '" . $uname . "')";
	$ok = false;
	try
		{
		$stmt = $con->prepare($sql);
		$stmt->execute();
		$ok = true;
		}
	catch (PDOException $e)
		{
		$err .= "Error: " . $e->getMessage();
		$ok = false;
		}
	return $ok;
	}

function updateLoggedUser($uname, $id, $con, &$err) // Aggiorna il tempo dell'ultimo update dell'utente connesso
	{
	global $usrColumn, $tbl_log, $sidColumn, $lstlgColumn;
	
	$sql = "UPDATE " . $tbl_log . " SET " . $lstlgColumn . "=null WHERE " . $usrColumn . "='" . $uname . "' AND " . $sidColumn . "='" . $id . "'";
	$ok = false;
	try
		{
		$stmt = $con->prepare($sql);
		$stmt->execute();
		$ok = true;
		}
	catch (PDOException $e)
		{
		$err .= "Error: " . $e->getMessage();
		$ok = false;
		}
	return $ok;
	}
	
function removeLoggedUser($uname, $con, &$err, &$msg, $sID = null) // Elimina tutti gli utenti collegati con $uname (e $sID eventuale) 
	{
	global $usrColumn, $tbl_log, $sidColumn;
	
	$sql = "DELETE FROM " . $tbl_log . " WHERE " . $usrColumn . " = '" . $uname . "'";
	if($sID != null)
		{
		$sql .= " AND " . $sidColumn . " = '" . $sID . "'";
		}
		
	
	$ok = false;
	try
		{
		$stmt = $con->prepare($sql);
		$stmt->execute();
		$ok = true;
		}
	catch (PDOException $e)
		{
		$err .= "Error: " . $e->getMessage();
		$ok = false;
		}
	
	return $ok;
	}

function login($usr, $pwd) // Connette l'utente
	{
	global $username, $password, $db_name, $host, $usrColumn, $pwdColumn, $tbl_name, $keystr, $keystrval, $pwddb, $tbl_log, $sidColumn, $txtColumn;
	$msg = "";
	$err = "";
	
	$conn = connectIntDB($err); // Connessione al database interno
	$myusername = preg_replace("/[^a-zA-Z0-9@.]+/", "", $usr); // Filtra...
	$mypassword = sha1(preg_replace("/[^a-zA-Z0-9@.]+/", "", $pwd)); // ...solo alfanumerico, punto e chiocciola
	$count = 0; // Numero utenti trovati
	$logcount = 0; // Numero utenti loggati trovati
	
	$count = countUsers($myusername, $mypassword, $conn, $err); // Numero utenti registrati nel database
	if($count > 0) // Se trova almeno un record con utente e password, legge chiave e password della prima riga trovata e...
		{ // ...lascia aperta la connessione
		$keystrval = "";
		$pwddbval = "";
		$ret = getUserKeyPwd($myusername, $mypassword, $conn, $keystrval, $pwddbval, $err);
		if(! $ret)
			$conn = null;
		}
	else // Se non ci sono utenti ($count == 0)
		{
		$conn = null; // Chiude la connessione
		$err .= "Utente o password errati";
		}

	if($conn != null) // Se la connessione è aperta, prosegue. Imposta $count = -1 se c'è un errore
		{
		$logcount = countLoggedUsers($myusername, $conn, $err); // Conta gli utenti connessi con stesso username
		if($logcount > 0) // Se già connesso, azzera il conteggio (a -1, per distiguerlo)
			{
			$err .= $logcount . " utenti " . $myusername . " gia` connessi";
			$count = - 1;	// Imposta come se non avesse trovato uetenti registrati
			}
		if($count == 1) // Login corretto, solo un utente registrato
			{
			if(! isset($_SESSION['usr'])) // Se $_SESSION['usr'] non esiste ancora, allora la imposta
				{
				$_SESSION['usr'] = $myusername;
				$_SESSION['loginTime'] = time();
				$_SESSION['lastTime'] = time();
				$_SESSION['keystr'] = $keystrval;
				$_SESSION['pwddb'] = $pwddbval;
				
				$ok = insertLoggedUser($myusername, session_id(), $conn, $err); // Inserisce l'utente nella lista degli utenti 'loggati'
				if($ok == true)
					{
					$msg .= $_SESSION['usr'] . "\n" . session_id(); // Se tutto ok, invia usr e sID separati da un return
					}
				else
					{
					$count = -1;
					}
				}
			else // Se $_SESSION['usr'] esiste, un login è già stato eseguito.
				{
				$err .= "Login gia` eseguito";
				$count = - 1;
				}
			}
		}
	
	if($count < 1)
		{
		logout();
		}
	
	$conn = null; // Disconnette dal database
	#warning IMPOSTA LA VARIABILE DI SESSIONE Waiting a now()
	// ....
	// Invia risposta criptata (deve contenere $sID e utente, o altro)
	
	return "Errori: ".$err."\n".encrypt($msg);

	// Poi usare un'altra funzione: unlock, che deve ricevere i dati ricevuti prima e confrontarli (criptati).
	// confrontarli con le variabili si sessione, confronrtare now() con waiting e, se inferiore a qualche secondo,
	// sbloccare waiting.
	
	}
	
	#warning AGGIUNGERE CLAUSOLA SU NOW()-LASTTIME > 2*TIMEOUT in removeLoggedUser
	#warning RISCRIVERE MEGLIO TUTTA LA FUNZIONE ED ESAMINARE BENE I CASI (PROBABILE ERRORE). POI SEMPLIFICARE ED USARE FUNZIONI SEPARATE
	#warning AGGIUNGERE, SE POSSIBILE, UNA RISPOSTA DI CONFERMA O DI DINIEGO (CRIPTATA) AL LOGIN, con necessità di risposta
	#warning Nel primo login viene inviata la password su connessione non protetta.
	#warning Prevedere handshaking dopo il primo login: se login ok -> invia risposta criptata e di mette in attesa -> poi riceve risposta criptata -> se errore: disconnette
	#warning Inviare la password criptata non con sha1() ma con una funzione più complessa, e confrontarla con il valore criptato una seconda volta
	#warning Risposta codificata al refresh per sapere se è ancora connesso o no (il programma in C# mostra se è connesso o no) 
	
function logout() // Disconnette l'utente
	{
	global $tbl_name, $keystr, $keystrval, $pwddb, $tbl_log, $sidColumn, $txtColumn;
	$msg = "";
	$conn = null;
	$err = "";
	if(isset($_SESSION['usr']))
		{
		$conn = connectIntDB($err);
		if($conn != null)
			{
			removeLoggedUser($_SESSION['usr'], $conn, $err, $msg, session_id()); // Elimina, senza controllare il risultato (usa $err)
			}
		$conn = null;
		session_unset();
		session_destroy();
		$msg .= "Utente disconnesso.";
		if($err != "")
			{
			$msg .= "\nERRORE: " . $err;
			}
		}
	else
		{
		$msg .= "Utente non ancora connesso";
		}
	return $msg;
	}

function clearlogged($usr, $pwd)
	{
	global $usrColumn, $pwdColumn, $tbl_name, $keystr, $keystrval, $pwddb, $tbl_log, $sidColumn, $txtColumn;
	$msg = "";
	$err = "";
	$conn = null;
	
	$conn = connectIntDB($err); // Connessione al database interno
	$msg .= "Clearing logged " . $usr . " users\n";
	$myusername = preg_replace("/[^a-zA-Z0-9@.]+/", "", $usr); // Filtra...
	$mypassword = sha1(preg_replace("/[^a-zA-Z0-9@.]+/", "", $pwd)); // ...solo alfanumerico, punto e chiocciola
	if(isset($_SESSION['usr'])) // Se utente gia` connesso, lo aggiunge al messaggio
		{
		$msg .= "Utente: " . $_SESSION['usr'] . " gia` connesso\n";
		}
	else // Se non è connesso, lo ricerca nel database,
		{ // come se aprisse un nuovo login
		$count = 0; // Numero utenti trovati
		$count = countUsers($myusername, $mypassword, $conn, $err); // Numero utenti nel database
		if($count < 1) // Se ci sono utenti
			{
			$conn = null; // Disconnette dal database
			}
		}
	if($conn != null) // Se è connesso o se utente e password sono corretti
		{
		$logcount = countLoggedUsers($myusername, $conn, $err); // Conta gli utenti connessi con stesso username
		$msg .= "\n" . $logcount . " utenti gia` connessi.";
		removeLoggedUser($myusername, $conn, $err, $msg); // Elimina, senza controllare il risultato (usa $err)
		$msg .= " Rimossi.";
		}
	return $msg;
	}

function refreshStat($stat = false) // Stato della connessione (se $stat==true: restituisce dettagli)
	{
	global $timeout;
	$msg = "";
	$err = "";
	if(isset($_SESSION['usr'])) // Se è connesso...
		{
		$tt = time() - $_SESSION['lastTime']; // ...verifica il tempo dall'ultimo refresh
		$_SESSION['lastTime'] = time();
		if($tt > $timeout) // Se il tempo è scaduto, chiude la sessione
			{
			if($stat)
				$msg = "Trascorsi " . $tt . " secondi. Timeout dopo " . $timeout . "s.";
			logout();
			}
		else	// Se il tempo non è scaduto, controlla se utente e sessione sono ancora nel database
			{
			$conn = connectIntDB($err);
			$cnt = countLoggedUsers($_SESSION['usr'], $conn, $err, session_id());
			if($cnt > 0)	// Se è nel database (non verifica la differenza di tempo nel database, perchè ha cercato utente e session id)...
				{
				updateLoggedUser($_SESSION['usr'], session_id(), $conn, $err);	// ...aggiorna il log
				if($stat)
					{
					$msg = "Utente: " . $_SESSION['usr'] . "\n";
					$msg .= "Session: " . session_id() . "\n";
					$msg .= "Tempo trascorso dal login: " . (time() - $_SESSION['loginTime']) . "\n";
					$msg .= "Tempo trascorso da ultima operazione: " . $tt;
					$msg .= "\nERRORE MYSQL:\n" . $err;
					}
				}
			else	// Se non è più nel database
				{
				logout();
				}
			}
		}
	else
		{
		$msg = "Utente non connesso";
		session_unset();
		session_destroy();
		}
	return $msg;
	}

function execute($cmd = "") // Esecuzione di un comando
	{
	$msg = "Messaggio\n" . decrypt($cmd) . "\nricevuto...";
	echo encrypt($msg);
	}

function query($cmd = "", $crypt = true) // Esecuzione di una query mysql
	{
	global $pertdb, $host;
	if($crypt)
		$sql = decrypt($cmd); // Legge il comando
	else
		$sql = $cmd;
	$c = null; // Connessione
	if(isset($_SESSION['usr']) && isset($_SESSION['pwddb']))
		{
		try
			{
			$c = new PDO("mysql:host=$host;dbname=$pertdb", $_SESSION['usr'], $_SESSION['pwddb']);
			$c->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
			try
				{
				$stmt = $c->prepare($sql);
				$stmt->execute();
				$result = $stmt->fetchAll(PDO::FETCH_ASSOC);
				if($result !== false)
					{
					$msg = "Query eseguita.\n";
					foreach ($result as $r)
						{
						foreach ($r as $x)
							$msg .= $x . "\t";
						$msg .= "\n";
						}
					}
				else
					{
					$msg = "Nessun risultato";
					}
				}
			catch (PDOException $e)
				{
				$msg = "Query error: " . $e->getMessage();
				}
			}
		catch (PDOException $e)
			{
			$msg = "WARNING: Connection error:\n";
			$msg .= $e->getMessage();
			}
		$c = null; // Disconnette dal database
		}
	if($crypt)
		$msg = encrypt($msg);
	return $msg;
	}

function sqlcom($cmd = "", $crypt = true) // Esecuzione di un comando mysql
	{
	global $pertdb, $host;
	$msg = "";
	if($crypt)
		$sql = decrypt($cmd); // Legge il comando
	else
		$sql = $cmd;
	$c = null; // Connessione
	if(isset($_SESSION['usr']) && isset($_SESSION['pwddb']))
		{
		try
			{
			$c = new PDO("mysql:host=$host;dbname=$pertdb", $_SESSION['usr'], $_SESSION['pwddb']);
			$c->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
			try
				{
				$count = $c->exec($sql);
				if($count !== false)
					$msg = "Affected " . $count . " rows.";
				else
					$msg = "No rows affected";
				}
			catch (PDOException $e)
				{
				$msg .= "Exec error: " . $e->getMessage();
				}
			}
		catch (PDOException $e)
			{
			$msg = "Connection error:\n";
			$msg .= $e->getMessage();
			}
		$c = null; // Disconnette dal database
		}
	if($crypt)
		$msg = encrypt($msg);
	return $msg;
	}

function command() // Lettura ed esecuzione di un comando (Post)
	{
	$a = $_POST["a"];
	$b = $_POST["b"];
	$c = $_POST["c"];
	switch ($a)
		{
		case "login":
			{
			$rsp = login($b, $c);
			echo $rsp;
			}
			break;
		case "logout":
			echo logout();
			break;
		case "clearlogged":
			echo clearlogged($b, $c);
			break;
		case "status":
			echo refreshStat(true);
			break;
		case "encode":
			echo encrypt($b);
			refreshStat();
			break;
		case "decode":
			echo decrypt($b);
			refreshStat();
			break;
		case "exe":
			echo execute($b);
			refreshStat();
			break;
		case "query":
			if($c == "nocrypt")
				echo query($b, false);
			else
				echo query($b);
			refreshStat();
			break;
		case "exec":
			if($c == "nocrypt")
				echo sqlcom($b, false);
			else
				echo sqlcom($b);
			refreshStat();
			break;
		case "refresh":
			echo refreshStat(); // echo refresh solo qui, per verificare connessione
			break;
		default:
			echo "command unknown";
			break;
		}
	}
	
?>
