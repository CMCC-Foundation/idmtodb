-- MariaDB dump 10.19  Distrib 10.5.16-MariaDB, for Linux (x86_64)
--
-- Host: localhost    Database: idmdb_wip
-- ------------------------------------------------------
-- Server version	10.5.16-MariaDB

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `group`
--

USE idmdb_wip;

DROP TABLE IF EXISTS `group`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `group` (
  `idgroup` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `desc` varchar(512) DEFAULT NULL,
  `status` tinyint(3) unsigned DEFAULT 1,
  `gid` int(11) NOT NULL,
  PRIMARY KEY (`idgroup`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `group`
--

LOCK TABLES `group` WRITE;
/*!40000 ALTER TABLE `group` DISABLE KEYS */;
INSERT INTO `group` VALUES (1,'opa',NULL,1,12000),(2,'oda',NULL,1,4000),(3,'remhi',NULL,1,6000),(4,'csp',NULL,1,5000),(5,'asc',NULL,1,7000),(6,'sysm',NULL,1,3000),(8,'iscd',NULL,1,13000),(9,'cp',NULL,1,5400),(10,'opa-dev',NULL,1,12500),(11,'cmip',NULL,1,5500),(12,'ada',NULL,1,5300),(13,'opa-op',NULL,1,12600),(14,'sim',NULL,1,5100),(17,'cpo',NULL,1,5200);
/*!40000 ALTER TABLE `group` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary table structure for view `tmp_user_group`
--

DROP TABLE IF EXISTS `tmp_user_group`;
/*!50001 DROP VIEW IF EXISTS `tmp_user_group`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `tmp_user_group` (
  `iduser_idm` tinyint NOT NULL,
  `username` tinyint NOT NULL,
  `name` tinyint NOT NULL,
  `surname` tinyint NOT NULL,
  `uid` tinyint NOT NULL,
  `gid` tinyint NOT NULL,
  `group_name` tinyint NOT NULL,
  `division` tinyint NOT NULL,
  `creation_date` tinyint NOT NULL,
  `expiration_date` tinyint NOT NULL,
  `vpn_expiration_date` tinyint NOT NULL,
  `email` tinyint NOT NULL,
  `notes` tinyint NOT NULL,
  `closing_date` tinyint NOT NULL,
  `no_cmcc` tinyint NOT NULL,
  `status` tinyint NOT NULL,
  `real_group_name` tinyint NOT NULL,
  `type` tinyint NOT NULL
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `user_gd_idm`
--

DROP TABLE IF EXISTS `user_gd_idm`;
/*!50001 DROP VIEW IF EXISTS `user_gd_idm`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `user_gd_idm` (
  `iduser_idm` tinyint NOT NULL,
  `name` tinyint NOT NULL,
  `surname` tinyint NOT NULL,
  `uid` tinyint NOT NULL,
  `gid` tinyint NOT NULL,
  `group_name` tinyint NOT NULL,
  `division` tinyint NOT NULL,
  `creation_date` tinyint NOT NULL,
  `expiration_date` tinyint NOT NULL,
  `vpn_expiration_date` tinyint NOT NULL,
  `email` tinyint NOT NULL,
  `notes` tinyint NOT NULL,
  `closing_date` tinyint NOT NULL
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `user_group_idm`
--

DROP TABLE IF EXISTS `user_group_idm`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user_group_idm` (
  `iduser_group_idm` int(11) NOT NULL AUTO_INCREMENT,
  `iduserExt` int(11) NOT NULL,
  `idgroupExt` int(11) NOT NULL,
  `dateFrom` date DEFAULT NULL,
  `dateTo` date DEFAULT NULL,
  `type` tinyint(4) NOT NULL DEFAULT 2,
  PRIMARY KEY (`iduser_group_idm`),
  KEY `fk_user_group_idm_1_idx` (`iduserExt`),
  KEY `fk_user_group_idm_2_idx` (`idgroupExt`),
  CONSTRAINT `fk_user_group_idm_1` FOREIGN KEY (`iduserExt`) REFERENCES `user_idm` (`iduser_idm`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_user_group_idm_2` FOREIGN KEY (`idgroupExt`) REFERENCES `group` (`idgroup`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=2041 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user_group_idm`
--

LOCK TABLES `user_group_idm` WRITE;
/*!40000 ALTER TABLE `user_group_idm` DISABLE KEYS */;
INSERT INTO `user_group_idm` VALUES (1531,196,1,'2023-04-28',NULL,0),(1532,197,2,'2023-05-15',NULL,0),(1533,198,1,'2023-04-19',NULL,0),(1534,199,3,'2023-02-22',NULL,0),(1535,200,2,'2023-04-14',NULL,0),(1536,201,1,'2023-04-18',NULL,0),(1537,202,1,'2023-02-24',NULL,0),(1538,203,2,'2023-05-15',NULL,0),(1539,204,1,'2023-02-24',NULL,0),(1540,205,1,'2023-02-24',NULL,0),(1541,206,2,'2023-05-15',NULL,0),(1542,207,1,'2023-04-19',NULL,0),(1543,208,1,'2023-04-19',NULL,0),(1544,209,4,'2023-01-10',NULL,0),(1545,210,4,'2023-04-14',NULL,0),(1546,211,1,'2023-02-24',NULL,0),(1547,212,1,'2023-02-24',NULL,0),(1548,213,1,'2023-02-17',NULL,0),(1549,214,1,'2023-02-20',NULL,0),(1550,215,1,'2023-02-17',NULL,0),(1551,216,1,'2023-02-20',NULL,0),(1552,217,1,'2023-04-26',NULL,0),(1553,218,2,'2023-03-22',NULL,0),(1554,219,4,'2023-05-19',NULL,0),(1555,220,1,'2023-02-24',NULL,0),(1556,221,1,'2023-02-17',NULL,0),(1557,222,5,'2023-02-27',NULL,0),(1558,223,2,'2023-03-16',NULL,0),(1559,224,4,'2023-05-19',NULL,0),(1560,225,4,'2023-01-10',NULL,0),(1561,226,2,'2023-05-15',NULL,0),(1562,227,1,'2023-02-24',NULL,0),(1563,228,1,'2023-04-19',NULL,0),(1564,229,2,'2023-03-22',NULL,0),(1565,230,5,'2023-05-08',NULL,0),(1566,231,5,'2023-02-27',NULL,0),(1567,232,5,'2023-02-14',NULL,0),(1568,233,1,'2023-02-24',NULL,0),(1569,234,1,'2023-04-17',NULL,0),(1570,235,1,'2023-04-17',NULL,0),(1571,236,1,'2023-02-24',NULL,0),(1572,237,5,'2023-02-27',NULL,0),(1573,238,2,'2023-05-15',NULL,0),(1574,239,4,'2023-01-10',NULL,0),(1575,240,3,'2023-03-29',NULL,0),(1576,241,1,'2023-02-15',NULL,0),(1577,242,1,'2023-04-28',NULL,0),(1578,243,1,'2023-04-18',NULL,0),(1579,244,5,'2023-02-14',NULL,0),(1580,245,1,'2023-02-17',NULL,0),(1581,246,1,'2023-02-20',NULL,0),(1582,247,1,'2023-04-17',NULL,0),(1583,248,1,'2023-04-17',NULL,0),(1584,249,1,'2023-02-15',NULL,0),(1585,250,1,'2023-02-15',NULL,0),(1586,251,5,'2023-02-14',NULL,0),(1587,252,1,'2023-02-15',NULL,0),(1588,253,1,'2023-04-27',NULL,0),(1589,254,2,'2023-05-15',NULL,0),(1590,255,4,'2023-01-10',NULL,0),(1591,256,1,'2023-04-19',NULL,0),(1592,257,4,'2023-04-26',NULL,0),(1593,258,3,'2023-02-22',NULL,0),(1594,259,4,'2023-04-06',NULL,0),(1595,260,2,'2023-05-15',NULL,0),(1596,261,2,'2023-05-15',NULL,0),(1597,262,1,'2023-02-21',NULL,0),(1598,263,1,'2023-02-24',NULL,0),(1599,264,1,'2023-02-24',NULL,0),(1600,265,1,'2023-02-24',NULL,0),(1601,266,1,'2023-02-24',NULL,0),(1602,267,1,'2023-02-17',NULL,0),(1603,268,1,'2023-02-20',NULL,0),(1604,269,1,'2023-02-17',NULL,0),(1605,270,1,'2023-02-20',NULL,0),(1606,271,4,'2023-03-14',NULL,0),(1607,272,1,'2023-04-27',NULL,0),(1608,273,1,'2023-02-24',NULL,0),(1609,274,3,'2023-03-15',NULL,0),(1610,275,1,'2023-04-19',NULL,0),(1611,276,3,'2023-02-21',NULL,0),(1612,277,1,'2023-04-26',NULL,0),(1613,278,1,'2023-04-19',NULL,0),(1614,279,4,'2023-05-29',NULL,0),(1615,280,1,'2023-04-27',NULL,0),(1616,281,1,'2023-02-24',NULL,0),(1617,282,1,'2023-04-17',NULL,0),(1618,283,1,'2023-04-17',NULL,0),(1619,284,1,'2023-04-17',NULL,0),(1620,285,1,'2023-02-24',NULL,0),(1621,286,1,'2023-02-24',NULL,0),(1622,287,2,'2023-02-21',NULL,0),(1623,288,3,'2023-02-22',NULL,0),(1624,289,2,'2023-05-15',NULL,0),(1625,290,1,'2023-04-18',NULL,0),(1626,291,1,'2023-02-15',NULL,0),(1627,292,1,'2023-04-19',NULL,0),(1628,293,1,'2023-02-24',NULL,0),(1629,294,1,'2023-04-26',NULL,0),(1630,295,1,'2023-04-19',NULL,0),(1631,296,1,'2023-04-19',NULL,0),(1632,297,4,'2023-01-10',NULL,0),(1633,298,4,'2022-12-22',NULL,0),(1634,299,4,'2022-12-22',NULL,0),(1635,300,4,'2023-02-08',NULL,0),(1636,301,6,'2022-11-23',NULL,0),(1637,302,6,'2022-11-23',NULL,0),(1638,303,6,'2022-11-17',NULL,0),(1639,304,6,'2022-11-23',NULL,0),(1640,305,6,'2022-11-16',NULL,0),(1641,306,6,'2022-11-23',NULL,0),(1642,307,6,'2022-12-22',NULL,0),(1643,308,2,'2023-05-15',NULL,0),(1644,309,1,'2023-02-24',NULL,0),(1645,310,1,'2023-02-24',NULL,0),(1646,311,1,'2023-04-28',NULL,0),(1647,312,1,'2023-04-19',NULL,0),(1648,313,1,'2023-04-17',NULL,0),(1649,314,1,'2023-04-17',NULL,0),(1650,315,1,'2023-02-17',NULL,0),(1651,316,1,'2023-02-20',NULL,0),(1652,317,1,'2023-04-17',NULL,0),(1653,318,1,'2023-04-17',NULL,0),(1654,319,4,'2023-06-09',NULL,0),(1655,320,2,'2023-06-08',NULL,0),(1656,321,3,'2023-06-08',NULL,0),(1657,322,2,'2023-06-08',NULL,0),(1658,323,3,'2023-06-08',NULL,0),(1659,324,3,'2023-06-08',NULL,0),(1660,325,3,'2023-06-08',NULL,0),(1661,326,1,'2023-06-09',NULL,0),(1662,327,4,'2023-06-09',NULL,0),(1786,196,1,'2023-04-28',NULL,1),(1787,197,2,'2023-05-15',NULL,1),(1788,198,1,'2023-04-19',NULL,1),(1789,199,3,'2023-02-22',NULL,1),(1790,200,2,'2023-04-14',NULL,1),(1791,201,1,'2023-04-18',NULL,1),(1792,202,1,'2023-02-24',NULL,1),(1793,203,2,'2023-05-15',NULL,1),(1794,204,1,'2023-02-24',NULL,1),(1795,205,1,'2023-02-24',NULL,1),(1796,206,2,'2023-05-15',NULL,1),(1797,207,1,'2023-04-19',NULL,1),(1798,208,1,'2023-04-19',NULL,1),(1799,209,9,'2023-01-10',NULL,1),(1800,210,9,'2023-04-14',NULL,1),(1801,211,10,'2023-02-24',NULL,1),(1802,212,10,'2023-02-24',NULL,1),(1803,213,10,'2023-02-17',NULL,1),(1804,214,10,'2023-02-20',NULL,1),(1805,215,10,'2023-02-17',NULL,1),(1806,216,10,'2023-02-20',NULL,1),(1807,217,1,'2023-04-26',NULL,1),(1808,218,2,'2023-03-22',NULL,1),(1809,219,11,'2023-05-19',NULL,1),(1810,220,1,'2023-02-24',NULL,1),(1811,221,10,'2023-02-17',NULL,1),(1812,222,5,'2023-02-27',NULL,1),(1813,223,2,'2023-03-16',NULL,1),(1814,224,12,'2023-05-19',NULL,1),(1815,225,11,'2023-01-10',NULL,1),(1816,226,2,'2023-05-15',NULL,1),(1817,227,1,'2023-02-24',NULL,1),(1818,228,1,'2023-04-19',NULL,1),(1819,229,2,'2023-03-22',NULL,1),(1820,230,5,'2023-05-08',NULL,1),(1821,231,5,'2023-02-27',NULL,1),(1822,232,5,'2023-02-14',NULL,1),(1823,233,1,'2023-02-24',NULL,1),(1824,234,13,'2023-04-17',NULL,1),(1825,235,10,'2023-04-17',NULL,1),(1826,236,1,'2023-02-24',NULL,1),(1827,237,5,'2023-02-27',NULL,1),(1828,238,2,'2023-05-15',NULL,1),(1829,239,12,'2023-01-10',NULL,1),(1830,240,3,'2023-03-29',NULL,1),(1831,241,1,'2023-02-15',NULL,1),(1832,242,1,'2023-04-28',NULL,1),(1833,243,1,'2023-04-18',NULL,1),(1834,244,5,'2023-02-14',NULL,1),(1835,245,10,'2023-02-17',NULL,1),(1836,246,10,'2023-02-20',NULL,1),(1837,247,13,'2023-04-17',NULL,1),(1838,248,10,'2023-04-17',NULL,1),(1839,249,1,'2023-02-15',NULL,1),(1840,250,1,'2023-02-15',NULL,1),(1841,251,5,'2023-02-14',NULL,1),(1842,252,1,'2023-02-15',NULL,1),(1843,253,1,'2023-04-27',NULL,1),(1844,254,2,'2023-05-15',NULL,1),(1845,255,12,'2023-01-10',NULL,1),(1846,256,1,'2023-04-19',NULL,1),(1847,257,14,'2023-04-26',NULL,1),(1848,258,3,'2023-02-22',NULL,1),(1849,259,9,'2023-04-06',NULL,1),(1850,260,2,'2023-05-15',NULL,1),(1851,261,2,'2023-05-15',NULL,1),(1852,262,1,'2023-02-21',NULL,1),(1853,263,10,'2023-02-24',NULL,1),(1854,264,10,'2023-02-24',NULL,1),(1855,265,10,'2023-02-24',NULL,1),(1856,266,10,'2023-02-24',NULL,1),(1857,267,10,'2023-02-17',NULL,1),(1858,268,10,'2023-02-20',NULL,1),(1859,269,10,'2023-02-17',NULL,1),(1860,270,10,'2023-02-20',NULL,1),(1861,271,12,'2023-03-14',NULL,1),(1862,272,1,'2023-04-27',NULL,1),(1863,273,1,'2023-02-24',NULL,1),(1864,274,3,'2023-03-15',NULL,1),(1865,275,1,'2023-04-19',NULL,1),(1866,276,3,'2023-02-21',NULL,1),(1867,277,1,'2023-04-26',NULL,1),(1868,278,1,'2023-04-19',NULL,1),(1869,279,11,'2023-05-29',NULL,1),(1870,280,1,'2023-04-27',NULL,1),(1871,281,1,'2023-02-24',NULL,1),(1872,282,10,'2023-04-17',NULL,1),(1873,283,13,'2023-04-17',NULL,1),(1874,284,10,'2023-04-17',NULL,1),(1875,285,10,'2023-02-24',NULL,1),(1876,286,10,'2023-02-24',NULL,1),(1877,287,2,'2023-02-21',NULL,1),(1878,288,3,'2023-02-22',NULL,1),(1879,289,2,'2023-05-15',NULL,1),(1880,290,1,'2023-04-18',NULL,1),(1881,291,1,'2023-02-15',NULL,1),(1882,292,1,'2023-04-19',NULL,1),(1883,293,1,'2023-02-24',NULL,1),(1884,294,1,'2023-04-26',NULL,1),(1885,295,1,'2023-04-19',NULL,1),(1886,296,1,'2023-04-19',NULL,1),(1887,297,12,'2023-01-10',NULL,1),(1888,298,17,'2022-12-22',NULL,1),(1889,299,17,'2022-12-22',NULL,1),(1890,300,12,'2023-02-08',NULL,1),(1891,301,6,'2022-11-23',NULL,1),(1892,302,6,'2022-11-23',NULL,1),(1893,303,6,'2022-11-17',NULL,1),(1894,304,6,'2022-11-23',NULL,1),(1895,305,6,'2022-11-16',NULL,1),(1896,306,6,'2022-11-23',NULL,1),(1897,307,6,'2022-12-22',NULL,1),(1898,308,2,'2023-05-15',NULL,1),(1899,309,10,'2023-02-24',NULL,1),(1900,310,10,'2023-02-24',NULL,1),(1901,311,1,'2023-04-28',NULL,1),(1902,312,1,'2023-04-19',NULL,1),(1903,313,13,'2023-04-17',NULL,1),(1904,314,10,'2023-04-17',NULL,1),(1905,315,10,'2023-02-17',NULL,1),(1906,316,10,'2023-02-20',NULL,1),(1907,317,13,'2023-04-17',NULL,1),(1908,318,10,'2023-04-17',NULL,1),(1909,319,9,'2023-06-09',NULL,1),(1910,320,2,'2023-06-08',NULL,1),(1911,321,3,'2023-06-08',NULL,1),(1912,322,2,'2023-06-08',NULL,1),(1913,323,3,'2023-06-08',NULL,1),(1914,324,3,'2023-06-08',NULL,1),(1915,325,3,'2023-06-08',NULL,1),(1916,326,1,'2023-06-09',NULL,1),(1917,327,14,'2023-06-09',NULL,1);
/*!40000 ALTER TABLE `user_group_idm` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user_idm`
--

DROP TABLE IF EXISTS `user_idm`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user_idm` (
  `iduser_idm` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(16) NOT NULL,
  `name` varchar(32) NOT NULL,
  `surname` varchar(32) NOT NULL,
  `uid` int(11) NOT NULL,
  `gid` int(11) NOT NULL,
  `group_name` varchar(32) NOT NULL,
  `division` varchar(32) NOT NULL,
  `creation_date` date NOT NULL,
  `expiration_date` date DEFAULT NULL,
  `vpn_expiration_date` date DEFAULT NULL,
  `email` varchar(64) DEFAULT NULL,
  `notes` varchar(256) DEFAULT NULL,
  `closing_date` date DEFAULT NULL,
  `no_cmcc` tinyint(1) NOT NULL,
  `status` tinyint(1) NOT NULL,
  PRIMARY KEY (`iduser_idm`)
) ENGINE=InnoDB AUTO_INCREMENT=328 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user_idm`
--

LOCK TABLES `user_idm` WRITE;
/*!40000 ALTER TABLE `user_idm` DISABLE KEYS */;
INSERT INTO `user_idm` VALUES (196,'aa11823','Abdul','Azeez',12099,12000,'opa','opa','2023-04-28','2023-07-31','2023-07-31','sabdulazeez44@gmail.com',NULL,NULL,0,0),(197,'aa32919','Ali','Aydogdu',4008,4000,'oda','oda','2023-05-15',NULL,'2023-11-17','ali.aydogdu@cmcc.it',NULL,NULL,0,0),(198,'ab10923','Arno','Behrens',12096,12000,'opa','opa','2023-04-19','2024-12-31',NULL,'arno.behrens@hereon.de',NULL,NULL,0,0),(199,'ac06322','Angelo','Campanale',6028,6000,'remhi','remhi','2023-02-22',NULL,'2024-02-28','angelo.campanale@cmcc.it',NULL,NULL,0,0),(200,'ac28919','Andrea','Cipollone',4007,4000,'oda','oda','2023-04-14',NULL,'2024-01-03','andrea.cipollone@cmcc.it',NULL,NULL,0,0),(201,'ad07521','Alessandro','De Lorenzis',12072,12000,'opa','opa','2023-04-18','2024-02-28','2024-02-28','alessandro.delorenzis@cmcc.it',NULL,NULL,0,0),(202,'ad12621','Alessandro','De Donno',12074,12000,'opa','opa','2023-02-24',NULL,'2023-04-30','alessandro.dedonno@cmcc.it',NULL,NULL,0,0),(203,'ag15419','Anna Chiara','Goglio',4013,4000,'oda','oda','2023-05-15',NULL,'2025-05-08','annachiara.goglio@cmcc.it',NULL,NULL,0,0),(204,'ag22216','Alessandro','Grandi',12040,12000,'opa','opa','2023-02-24',NULL,'2023-09-30','alessandro.grandi@cmcc.it',NULL,NULL,0,0),(205,'am09320','Antonio','Mariani',12011,12000,'opa','opa','2023-02-24',NULL,'2024-03-31','antonio.mariani@cmcc.it',NULL,NULL,0,0),(206,'am13420','Aimie','Moulin',4213,4000,'oda','oda','2023-05-15','2024-04-30','2024-05-10','aimie.moulin@cmcc.it',NULL,NULL,0,0),(207,'am24522','Aljaz','Maslo',12087,12000,'opa','opa','2023-04-19','2023-08-31','2023-08-31','aljaz.maslo@cmcc.it',NULL,NULL,0,0),(208,'an23420','Augusto','Sepp Neves',12064,12000,'opa','opa','2023-04-19','2023-08-31','2023-08-31','augusto.sepp@cmcc.it',NULL,NULL,0,0),(209,'as34319','Antonella','Sanna',5401,5400,'cp','csp','2023-01-10',NULL,'2024-06-14','antonella.sanna@cmcc.it',NULL,NULL,0,0),(210,'aspect','aspect','project account',5414,5400,'cp','csp','2023-04-14',NULL,NULL,'antonella.sanna@cmcc.it',NULL,NULL,0,0),(211,'bs_inter','bs_inter','project account',12543,12500,'opa-dev','opa','2023-02-24',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(212,'bs_inter-dev','bs_inter-dev','project account',12608,12600,'opa-dev','opa','2023-02-24',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(213,'bsfs','bsfs','project account',12517,12500,'opa-dev','opa','2023-02-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(214,'bsfs-dev','bsfs-dev','project account',12604,12600,'opa-dev','opa','2023-02-20',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(215,'bswav','bswav','project account',12512,12500,'opa-dev','opa','2023-02-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(216,'bswav-dev','bswav-dev','project account',12601,12600,'opa-dev','opa','2023-02-20',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(217,'cg05222','Caterina','Gianolla',12078,12000,'opa','opa','2023-04-26','2024-12-31','2024-02-21','caterina.gianolla@cmcc.it',NULL,NULL,0,0),(218,'cg30921','Chiara','Galeotti',4041,4000,'oda','oda','2023-03-22','2024-03-31','2023-11-05','chiara.galeotti@cmcc.it',NULL,NULL,0,0),(219,'cmip02','cmip02','project account',5502,5500,'cmip','csp','2023-05-19',NULL,NULL,'dario.nicoli@cmcc.it',NULL,NULL,0,0),(220,'da01720','Diana','Azevedo',12038,12000,'opa','opa','2023-02-24',NULL,'2023-12-31','diana.azevedo@cmcc.it',NULL,NULL,0,0),(221,'das','das','project account',12502,12500,'opa-dev','opa','2023-02-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(222,'dd26322','Davide','Donno',7039,7000,'asc','asc','2023-02-27',NULL,'2023-09-30','davide.donno@cmcc.it',NULL,NULL,0,0),(223,'di28319','Dorotea','Iovino',4001,4000,'oda','oda','2023-03-16',NULL,'2024-01-03','dorotea.iovino@cmcc.it',NULL,NULL,0,0),(224,'dn10017','Dario','Nicoli',5304,5300,'ada','csp','2023-05-19',NULL,'2024-12-31','dario.nicoli@cmcc.it',NULL,NULL,0,0),(225,'dp16116','Daniele','Peano',5505,5500,'cmip','csp','2023-01-10',NULL,NULL,'daniele.peano@cmcc.it',NULL,NULL,0,0),(226,'ec04916','Emanuela','Clementi',4002,4000,'oda','oda','2023-05-15',NULL,'2023-10-01','emanuela.clementi@cmcc.it',NULL,NULL,0,0),(227,'ej03515','Eric','Jansen',12028,12000,'opa','opa','2023-02-24',NULL,'2023-09-30','eric.jansen@cmcc.it',NULL,NULL,0,0),(228,'fb06023','Francesco','Benfenati',12093,12000,'opa','opa','2023-04-19','2024-12-31','2025-12-31','francesco.benfenati@cmcc.it',NULL,NULL,0,0),(229,'fc09621','Francesco','Cocetta',4030,4000,'oda','oda','2023-03-22','2024-03-31','2023-01-18','francesco.cocetta@cmcc.it',NULL,NULL,0,0),(230,'fc12823','Francesco','Carere',7043,7000,'asc','asc','2023-05-08','2024-03-31',NULL,'francesco.carere@cmcc.it',NULL,NULL,0,0),(231,'fi18022','Francesco','Immorlano',7037,7000,'asc','asc','2023-02-27',NULL,'2023-06-30','francesco.immorlano@cmcc.it',NULL,NULL,0,0),(232,'fm27215','Francesca','Mele',7018,7000,'asc','asc','2023-02-14',NULL,'2023-06-01','francesca.mele@cmcc.it',NULL,NULL,0,0),(233,'fp17315','Francesco','Palermo',12030,12000,'opa','opa','2023-02-24',NULL,'2023-09-23','francesco.palermo@cmcc.it',NULL,NULL,0,0),(234,'framevisir','framevisir','project account',12567,12500,'opa-op','opa','2023-04-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(235,'framevisir-dev','framevisir-dev','project account',12566,12500,'opa-dev','opa','2023-04-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(236,'fv08620','Fabio','Viola',12042,12000,'opa','opa','2023-02-24',NULL,'2024-03-31','fabio.viola@cmcc.it',NULL,NULL,0,0),(237,'ga18618','Gabriele','Accarino',7026,7000,'asc','asc','2023-02-27',NULL,'2023-03-31','gabriele.accarino@cmcc.it',NULL,NULL,0,0),(238,'gb20216','Giulia','Bonino',4402,4000,'oda','oda','2023-05-15','2025-03-31','2024-03-31','giulia.bonino@cmcc.it',NULL,NULL,0,0),(239,'gc02720','Giovanni','Conti',5303,5300,'ada','csp','2023-01-10','2024-12-31','2024-12-31','giovanni.conti@cmcc.it',NULL,NULL,0,0),(240,'gc08823','Gabriella','Ceci',6032,6000,'remhi','remhi','2023-03-29','2023-06-30',NULL,'gabriella.ceci@cmcc.it',NULL,NULL,0,0),(241,'gc15521','Giovanni','De Cillis',12077,12000,'opa','opa','2023-02-15',NULL,'2025-05-31','giovanni.decillis@cmcc.it',NULL,NULL,0,0),(242,'gk11823','Gerasimos','Korres',12100,12000,'opa','opa','2023-04-28','2024-12-31',NULL,'gkorres@hcmr.gr',NULL,NULL,0,0),(243,'gm29119','Gianandrea','Mannarini',12003,12000,'opa','opa','2023-04-18','2099-12-31','2023-09-01','gianandrea.mannarini@cmcc.it',NULL,NULL,0,0),(244,'gm30419','Giorgio','Micaletto',7008,7000,'asc','asc','2023-02-14',NULL,'2023-10-27','giorgio.micaletto@cmcc.it',NULL,NULL,0,0),(245,'gofs','gofs','project account',12520,12500,'opa-dev','opa','2023-02-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(246,'gofs-dev','gofs-dev','project account',12605,12600,'opa-dev','opa','2023-02-20',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(247,'guttavisir','guttavisir','project account',12569,12500,'opa-op','opa','2023-04-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(248,'guttavisir-dev','guttavisir-dev','project account',12568,12500,'opa-dev','opa','2023-04-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(249,'gv29119','Giorgia','Verri',12008,12000,'opa','opa','2023-02-15',NULL,'2023-10-11','giorgia.verri@cmcc.it',NULL,NULL,0,0),(250,'ib31217','Ivano','Barletta',12050,12000,'opa','opa','2023-02-15',NULL,'2023-10-04','ivano.barletta@cmcc.it',NULL,NULL,0,0),(251,'ie30419','Italo','Epicoco',7009,7000,'asc','asc','2023-02-14',NULL,'2023-10-29','italo.epicoco@cmcc.it',NULL,NULL,0,0),(252,'if29119','Ivan','Federico',12004,12000,'opa','opa','2023-02-15',NULL,'2023-10-11','ivan.federico@cmcc.it',NULL,NULL,0,0),(253,'jc11022','Jonas','Carvalho',12080,12000,'opa','opa','2023-04-27','2024-03-30','2024-03-31','jonastakeo.carvalho@cmcc.it',NULL,NULL,0,0),(254,'jp04916','Jenny','Pistoia',4011,4000,'oda','oda','2023-05-15',NULL,'2023-11-18','jenny.pistoia@cmcc.it',NULL,NULL,0,0),(255,'lg07622','Luis','Goncalves',5315,5300,'ada','csp','2023-01-10','2024-02-28','2024-02-28','luis.goncalves@cmcc.it',NULL,NULL,0,0),(256,'lm09621','Lorenzo','Mentaschi',12073,12000,'opa','opa','2023-04-19','2024-12-31','2025-04-13','lorenzo.mentaschi@yahoo.it',NULL,NULL,0,0),(257,'ls21622','Lorenzo','Sangelantoni',5115,5100,'sim','csp','2023-04-26','2024-07-31','2024-07-31','lorenzo.sangelantoni@cmcc.it',NULL,NULL,0,0),(258,'ma24620','Marianna','Adinolfi',6021,6000,'remhi','remhi','2023-02-22',NULL,'2024-09-06','maria.adinolfi@cmcc.it',NULL,NULL,0,0),(259,'mb16318','Marianna','Benassi',5407,5400,'cp','csp','2023-04-06',NULL,'2024-09-29','marianna.benassi@cmcc.it',NULL,NULL,0,0),(260,'mb24817','Momme','Butenschön',4010,4000,'oda','oda','2023-05-15',NULL,'2023-09-21','momme.butenschon@cmcc.it',NULL,NULL,0,0),(261,'mb31322','Matteo','Broccoli',4044,4000,'oda','oda','2023-05-15','2024-10-31','2024-10-31','matteo.broccoli@cmcc.it',NULL,NULL,0,0),(262,'md04916','Massimiliano','Drudi',12037,12000,'opa','opa','2023-02-21',NULL,'2023-09-23','massimiliano.drudi@cmcc.it',NULL,NULL,0,0),(263,'med_inter','med_inter','project account',12541,12500,'opa-dev','opa','2023-02-24',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(264,'med_inter-dev','med_inter-dev','project account',12610,12600,'opa-dev','opa','2023-02-24',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(265,'medens','medens','project account',12564,12500,'opa-dev','opa','2023-02-24',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(266,'medens-dev','medens-dev','project account',12609,12600,'opa-dev','opa','2023-02-24',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(267,'medwav','medwav','project account',12510,12500,'opa-dev','opa','2023-02-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(268,'medwav-dev','medwav-dev','project account',12602,12600,'opa-dev','opa','2023-02-20',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(269,'mfs','mfs','project account',12518,12500,'opa-dev','opa','2023-02-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(270,'mfs-dev','mfs-dev','project account',12603,12600,'opa-dev','opa','2023-02-20',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(271,'mg20022','Marcelo','Guatura',5317,5300,'ada','csp','2023-03-14','2025-01-31','2025-01-31','marcelo.guatura@cmcc.it',NULL,NULL,0,0),(272,'mh12621','Megi','Hoxhaj',12075,12000,'opa','opa','2023-04-27','2024-04-30','2024-04-30','megi.hoxhaj@cmcc.it',NULL,NULL,0,0),(273,'mi19918','Mehmet','Ilicak',12053,12000,'opa','opa','2023-02-24',NULL,'2024-08-31','milicak@itu.edu.tr',NULL,NULL,0,0),(274,'mp07423','Matteo','Pesce',6031,6000,'remhi','remhi','2023-03-15','2023-09-30',NULL,'matteo.pesce@cmcc.it',NULL,NULL,0,0),(275,'mr10923','Marcel','Ricker',12095,12000,'opa','opa','2023-04-19','2024-12-31',NULL,'marcel.ricker@hereon.de',NULL,NULL,0,0),(276,'mr29116','Mario','Raffa',6019,6000,'remhi','remhi','2023-02-21',NULL,'2024-06-27','mario.raffa@cmcc.it',NULL,NULL,0,0),(277,'ms02821','Mario Leonardo','Salinas',12070,12000,'opa','opa','2023-04-26','2024-12-31','2024-12-31','mario.salinas@cmcc.it',NULL,NULL,0,0),(278,'ms20618','Matteo','Scuro',12065,12000,'opa','opa','2023-04-19','2099-01-01','2023-10-19','matteo.scuro@cmcc.it',NULL,NULL,0,0),(279,'mv32222','Mariana','Vertenstein',5516,5500,'cmip','csp','2023-05-29','2023-12-31',NULL,'mariana.vertenstein@cmcc.it',NULL,NULL,0,0),(280,'nb21722','Nicolas','Biocca',12083,12000,'opa','opa','2023-04-27','2024-08-31','2024-07-31','nicolas.biocca@cmcc.it',NULL,NULL,0,0),(281,'nj04221','Nejm','Jaafar',12071,12000,'opa','opa','2023-02-24',NULL,'2024-01-31','nejm.jaafar@cmcc.it',NULL,NULL,0,0),(282,'now_rsc','now_rsc','project account',12521,12500,'opa-dev','opa','2023-04-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(283,'oceansar','oceansar','project account',12533,12500,'opa-op','opa','2023-04-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(284,'oceansar-dev','oceansar-dev','project account',12534,12500,'opa-dev','opa','2023-04-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(285,'ov','ov','project account',12531,12500,'opa-dev','opa','2023-02-24',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(286,'ov-dev','ov-dev','project account',12611,12600,'opa-dev','opa','2023-02-24',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(287,'pf28319','Pier Giuseppe','Fogli',4003,4000,'oda','oda','2023-02-21',NULL,'2023-10-04','piergiuseppe.fogli@cmcc.it',NULL,NULL,0,0),(288,'pm09220','Paola','Mercogliano',6004,6000,'remhi','remhi','2023-02-22',NULL,'2024-04-14','paola.mercogliano@cmcc.it',NULL,NULL,0,0),(289,'pm28621','Pietro','Miraglio',4038,4000,'oda','oda','2023-05-15','2023-09-30','2023-10-13','pietro.miraglio@cmcc.it',NULL,NULL,0,0),(290,'rc15122','Rocco','Caccioppoli',12081,12000,'opa','opa','2023-04-18','2024-04-30','2024-04-30','rocco.caccioppoli@cmcc.it',NULL,NULL,0,0),(291,'rl25919','Rita','Lecci',12002,12000,'opa','opa','2023-02-15',NULL,'2023-09-14','rita.lecci@cmcc.it',NULL,NULL,0,0),(292,'sb09819','Simone','Bonamano',12058,12000,'opa','opa','2023-04-19','2023-07-31','2024-03-30','simo_bonamano@unitus.it',NULL,NULL,0,0),(293,'sc25619','Sergio','Creti',12001,12000,'opa','opa','2023-02-24',NULL,'2023-09-10','sergio.creti@cmcc.it',NULL,NULL,0,0),(294,'sc33616','Salvatore','Causio',12041,12000,'opa','opa','2023-04-26','2025-01-31','2023-10-06','salvatore.causio@cmcc.it',NULL,NULL,0,0),(295,'sk10923','Saloni','Kyal',12097,12000,'opa','opa','2023-04-19','2025-01-31',NULL,'saloni.kyal@cmcc.it',NULL,NULL,0,0),(296,'sl35719','Svitlana','Liubartseva',12007,12000,'opa','opa','2023-04-19','2099-01-01','2023-12-07','svitlana.liubartseva@cmcc.it',NULL,NULL,0,0),(297,'sm09722','Swapan','Mallick',5316,5300,'ada','csp','2023-01-10','2024-03-31','2022-09-30','swapan.mallick@cmcc.it',NULL,NULL,0,0),(298,'sp1','sp1','project account',5219,5200,'cpo','csp','2022-12-22',NULL,NULL,'sp1@cmcc.it',NULL,NULL,0,0),(299,'sp2','sp2','project account',5220,5200,'cpo','csp','2022-12-22',NULL,NULL,'sp1@cmcc.it',NULL,NULL,0,0),(300,'ss35621','Sami','Saarinen',5314,5300,'ada','csp','2023-02-08','2023-06-30','2023-06-30','sami.t.saarinen@gmail.com',NULL,NULL,0,0),(301,'sysm01','Osvaldo','Marra',3001,3000,'sysm','sysm','2022-11-23','2099-01-01','2025-05-14','osvaldo.marra@cmcc.it',NULL,NULL,0,0),(302,'sysm02','Mauro','Tridici',3002,3000,'sysm','sysm','2022-11-23','2099-01-01','2025-05-14','mauro.tridici@cmcc.it',NULL,NULL,0,0),(303,'sysm03','Giuseppe','Calo',3003,3000,'sysm','sysm','2022-11-17','2030-11-22','2025-05-14','giuseppe.calo@cmcc.it',NULL,NULL,0,0),(304,'sysm04','Fabio','Martano',3004,3000,'sysm','sysm','2022-11-23','2099-01-01','2023-07-26','fabio.martano@cmcc.it',NULL,NULL,0,0),(305,'sysm05','Davide','Nocco',3005,3000,'sysm','sysm','2022-11-16',NULL,'2023-07-19','davide.nocco@cmcc.it',NULL,NULL,0,0),(306,'sysm06','Danilo','Mazzarella',3006,3000,'sysm','sysm','2022-11-23','2024-05-16','2024-05-16','danilo.mazzarella@cmcc.it',NULL,NULL,0,0),(307,'sysm07','Marco','Chiarelli',3007,3000,'sysm','sysm','2022-12-22','2099-01-01','2024-12-14','marco.chiarelli@cmcc.it',NULL,NULL,0,0),(308,'tl28319','Tomas','Lovato',4004,4000,'oda','oda','2023-05-15',NULL,'2023-07-19','tomas.lovato@cmcc.it',NULL,NULL,0,0),(309,'u_tss','u_tss','project account',12562,12500,'opa-dev','opa','2023-02-24',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(310,'u_tss-dev','u_tss-dev','project account',12607,12600,'opa-dev','opa','2023-02-24',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(311,'ut11823','Ufuk','Turuncoglu',12101,12000,'opa','opa','2023-04-28','2023-11-30','2023-11-30','turuncu@ucar.edu',NULL,NULL,0,0),(312,'vl35719','Vladyslav','Lyubartsev',12006,12000,'opa','opa','2023-04-19','2099-01-01','2023-12-07','vladyslav.lyubartsev@cmcc.it',NULL,NULL,0,0),(313,'witoil','witoil','project account',12535,12500,'opa-op','opa','2023-04-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(314,'witoil-dev','witoil-dev','project account',12536,12500,'opa-dev','opa','2023-04-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(315,'wrf','wrf','project account',12515,12500,'opa-dev','opa','2023-02-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(316,'wrf-dev','wrf-dev','project account',12606,12600,'opa-dev','opa','2023-02-20',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(317,'ww3_cst','ww3_cst','project account',12526,12500,'opa-op','opa','2023-04-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(318,'ww3_cst-dev','ww3_cst-dev','project account',12537,12500,'opa-dev','opa','2023-04-17',NULL,NULL,'rita.lecci@cmcc.it',NULL,NULL,0,0),(319,'ab00920','Andrea','Borrelli',5402,5400,'cp','csp','2023-06-09',NULL,NULL,'andrea.borrelli@cmcc.it',NULL,NULL,0,0),(320,'am15923','Alberto','Merlo',4049,4000,'oda','oda','2023-06-08','2024-05-05',NULL,'alberto.merlo4@unibo.it',NULL,NULL,0,0),(321,'ar33317','Alfredo','Reder',6020,6000,'remhi','remhi','2023-06-08',NULL,'2024-09-05','alfredo.reder@cmcc.it',NULL,NULL,0,0),(322,'as15923','Andrea','Storto',4048,4000,'oda','oda','2023-06-08','2024-05-15','2024-05-15','andrea.storto@cmcc.it',NULL,NULL,0,0),(323,'gf27821','Giusy','Fedele',6026,6000,'remhi','remhi','2023-06-08','2023-06-30','2023-06-30','giusy.fedele@cmcc.it',NULL,NULL,0,0),(324,'gg21021','Giuseppe','Giugliano',6025,6000,'remhi','remhi','2023-06-08','2024-10-01','2023-07-29','giuseppe.giugliano@cmcc.it',NULL,NULL,0,0),(325,'ms15923','Mattia','Scalas',6034,6000,'remhi','remhi','2023-06-08','2023-10-31',NULL,'mattia.scalas@cmcc.it',NULL,NULL,0,0),(326,'ms16023','Muhammad','Saad',12102,12000,'opa','opa','2023-06-09','2025-12-31',NULL,'muhammad.saad9@unibo.it',NULL,NULL,0,0),(327,'rh11221','Riccardo','Henin',5110,5100,'sim','csp','2023-06-09','2023-11-15','2025-05-03','riccardo.henin@cmcc.it',NULL,NULL,0,0);
/*!40000 ALTER TABLE `user_idm` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Final view structure for view `tmp_user_group`
--

/*!50001 DROP TABLE IF EXISTS `tmp_user_group`*/;
/*!50001 DROP VIEW IF EXISTS `tmp_user_group`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `tmp_user_group` AS (select `user_idm`.`iduser_idm` AS `iduser_idm`,`user_idm`.`username` AS `username`,`user_idm`.`name` AS `name`,`user_idm`.`surname` AS `surname`,`user_idm`.`uid` AS `uid`,`user_idm`.`gid` AS `gid`,`user_idm`.`group_name` AS `group_name`,`user_idm`.`division` AS `division`,`user_idm`.`creation_date` AS `creation_date`,`user_idm`.`expiration_date` AS `expiration_date`,`user_idm`.`vpn_expiration_date` AS `vpn_expiration_date`,`user_idm`.`email` AS `email`,`user_idm`.`notes` AS `notes`,`user_idm`.`closing_date` AS `closing_date`,`user_idm`.`no_cmcc` AS `no_cmcc`,`user_idm`.`status` AS `status`,`group`.`name` AS `real_group_name`,`user_group_idm`.`type` AS `type` from ((`user_idm` join `user_group_idm` on(`user_idm`.`iduser_idm` = `user_group_idm`.`iduserExt`)) join `group` on(`user_group_idm`.`idgroupExt` = `group`.`idgroup`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `user_gd_idm`
--

/*!50001 DROP TABLE IF EXISTS `user_gd_idm`*/;
/*!50001 DROP VIEW IF EXISTS `user_gd_idm`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`sysm01`@`172.16.%.%` SQL SECURITY DEFINER */
/*!50001 VIEW `user_gd_idm` AS (select `tmp_ug1`.`iduser_idm` AS `iduser_idm`,`tmp_ug1`.`name` AS `name`,`tmp_ug1`.`surname` AS `surname`,`tmp_ug1`.`uid` AS `uid`,`tmp_ug1`.`gid` AS `gid`,`tmp_ug2`.`real_group_name` AS `group_name`,`tmp_ug1`.`real_group_name` AS `division`,`tmp_ug1`.`creation_date` AS `creation_date`,`tmp_ug1`.`expiration_date` AS `expiration_date`,`tmp_ug1`.`vpn_expiration_date` AS `vpn_expiration_date`,`tmp_ug1`.`email` AS `email`,`tmp_ug1`.`notes` AS `notes`,`tmp_ug1`.`closing_date` AS `closing_date` from (`tmp_user_group` `tmp_ug1` join `tmp_user_group` `tmp_ug2` on(`tmp_ug1`.`iduser_idm` = `tmp_ug2`.`iduser_idm` and `tmp_ug1`.`type` = 0 and `tmp_ug2`.`type` = 1))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-06-13 11:39:56
