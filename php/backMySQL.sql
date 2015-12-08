-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Versione server:              5.6.26 - MySQL Community Server (GPL)
-- S.O. server:                  Win32
-- HeidiSQL Versione:            9.3.0.4997
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- Dump della struttura del database pert01
CREATE DATABASE IF NOT EXISTS `pert01` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `pert01`;


-- Dump della struttura di tabella pert01.att
CREATE TABLE IF NOT EXISTS `att` (
  `ID_att` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'pk indice',
  `Nome` varchar(50) DEFAULT NULL,
  `Pos` varchar(20) DEFAULT NULL,
  `Inizio` datetime DEFAULT NULL COMMENT 'Inizio (data + orario)\n',
  `Durata` datetime DEFAULT '0000-00-00 01:00:00' COMMENT 'durata. Vedere se memorizzata correttamente. Deve essere >=0. Se zero: attività cardine.\\n\\n',
  `Fissa` tinyint(1) DEFAULT NULL COMMENT 'Se inizio non anticipabile',
  `Annullata` tinyint(1) DEFAULT NULL COMMENT 'Flag, di lavoro. La segna barrata o cancellata.\\n',
  `Eliminare` tinyint(1) DEFAULT NULL COMMENT 'Flag, attività da cancellare (anche se non cancellata). Il numero dell ID può esser riciclato.',
  PRIMARY KEY (`ID_att`),
  UNIQUE KEY `ID_att_UNIQUE` (`ID_att`)
) ENGINE=InnoDB AUTO_INCREMENT=101 DEFAULT CHARSET=latin1;

-- Dump dei dati della tabella pert01.att: ~5 rows (circa)
/*!40000 ALTER TABLE `att` DISABLE KEYS */;
INSERT INTO `att` (`ID_att`, `Nome`, `Pos`, `Inizio`, `Durata`, `Fissa`, `Annullata`, `Eliminare`) VALUES
	(1, 'Inizio', '0.0', '2015-10-19 23:11:47', '0000-00-01 00:00:00', 0, NULL, NULL),
	(2, 'aaa', '1.1', NULL, '0000-00-02 00:00:00', 1, NULL, NULL),
	(3, 'bbb', '1.2', NULL, '0000-00-00 10:00:00', 0, NULL, NULL),
	(4, 'a+b', '2.0', NULL, '0000-00-00 00:00:00', 0, NULL, 1),
	(5, 'FINE', 'F', NULL, '0000-00-00 01:00:00', NULL, NULL, NULL),
	(6, 'extra', '3.0.1', NULL, '0000-00-00 00:00:00', NULL, NULL, NULL),
	(100, 'extra', '3.0.1', NULL, '0000-00-00 00:00:00', NULL, NULL, NULL);
/*!40000 ALTER TABLE `att` ENABLE KEYS */;


-- Dump della struttura di tabella pert01.pred
CREATE TABLE IF NOT EXISTS `pred` (
  `id_att` bigint(20) unsigned NOT NULL,
  `id_attp` bigint(20) unsigned NOT NULL,
  UNIQUE KEY `att_attp_key` (`id_att`,`id_attp`),
  KEY `attp_fk_idx` (`id_attp`),
  CONSTRAINT `att_fk` FOREIGN KEY (`id_att`) REFERENCES `att` (`ID_att`),
  CONSTRAINT `attp_fk` FOREIGN KEY (`id_attp`) REFERENCES `att` (`ID_att`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Predecessori.';

-- Dump dei dati della tabella pert01.pred: ~4 rows (circa)
/*!40000 ALTER TABLE `pred` DISABLE KEYS */;
INSERT INTO `pred` (`id_att`, `id_attp`) VALUES
	(2, 1),
	(3, 1),
	(4, 2),
	(4, 3),
	(5, 4);
/*!40000 ALTER TABLE `pred` ENABLE KEYS */;


-- Dump della struttura del database my_fred68
CREATE DATABASE IF NOT EXISTS `my_fred68` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `my_fred68`;


-- Dump della struttura di tabella my_fred68.logged
CREATE TABLE IF NOT EXISTS `logged` (
  `sid` char(32) NOT NULL DEFAULT '1' COMMENT 'session id (pk)',
  `usrname` varchar(65) NOT NULL COMMENT 'utente',
  `lgtm` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'login time',
  `lstlg` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'last log time',
  PRIMARY KEY (`sid`,`usrname`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Dump dei dati della tabella my_fred68.logged: ~0 rows (circa)
/*!40000 ALTER TABLE `logged` DISABLE KEYS */;
/*!40000 ALTER TABLE `logged` ENABLE KEYS */;


-- Dump della struttura di tabella my_fred68.members
CREATE TABLE IF NOT EXISTS `members` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `usrname` varchar(65) NOT NULL,
  `passwd` varchar(40) NOT NULL,
  `keystr` char(32) DEFAULT NULL,
  `pwddb` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `usrname` (`usrname`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

-- Dump dei dati della tabella my_fred68.members: ~2 rows (circa)
/*!40000 ALTER TABLE `members` DISABLE KEYS */;
INSERT INTO `members` (`id`, `usrname`, `passwd`, `keystr`, `pwddb`) VALUES
	(1, 'pippo', '45fc0c0fe91bfe9804c9c8906980523bf8a44d58', '12345678901234567890123456789012', 'apQ127'),
	(2, 'pluto', '8b101cf4faf7992456c7f11064973d3aad2603b9', '21098765432109876543210987654321', '98zpR15');
/*!40000 ALTER TABLE `members` ENABLE KEYS */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
