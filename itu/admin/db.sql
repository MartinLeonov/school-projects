-- Adminer 4.1.0 MySQL dump

SET NAMES utf8;
SET time_zone = '+00:00';
SET foreign_key_checks = 0;
SET sql_mode = 'NO_AUTO_VALUE_ON_ZERO';

DROP DATABASE IF EXISTS `ITU`;
CREATE DATABASE `ITU` /*!40100 DEFAULT CHARACTER SET utf8 COLLATE utf8_bin */;
USE `ITU`;

DROP TABLE IF EXISTS `CLIENT`;
CREATE TABLE `CLIENT` (
  `name` varchar(50) COLLATE utf8_bin NOT NULL,
  `ID` int(32) NOT NULL AUTO_INCREMENT,
  `surname` varchar(50) COLLATE utf8_bin NOT NULL,
  `phone` varchar(9) COLLATE utf8_bin NOT NULL,
  `email` varchar(50) COLLATE utf8_bin NOT NULL,
  `password` binary(20) NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

INSERT INTO `CLIENT` (`name`, `ID`, `surname`, `phone`, `email`, `password`) VALUES
('Mark',  1,  'Otto', '736182739',  'markotto@gmail.com', UNHEX('6E017B5464F820A6C1BB5E9F6D711A667A80D8EA'));

DROP TABLE IF EXISTS `DATABASE`;
CREATE TABLE `DATABASE` (
  `name` varchar(32) COLLATE utf8_bin NOT NULL,
  `login` varchar(50) COLLATE utf8_bin NOT NULL,
  `password` binary(20) NOT NULL,
  `server` varchar(50) COLLATE utf8_bin NOT NULL,
  `webhosting` int(32) NOT NULL,
  `type` int(8) NOT NULL,
  PRIMARY KEY (`name`,`webhosting`),
  KEY `webhosting` (`webhosting`),
  CONSTRAINT `DATABASE_ibfk_1` FOREIGN KEY (`webhosting`) REFERENCES `WEBHOSTING` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

INSERT INTO `DATABASE` (`name`, `login`, `password`, `server`, `webhosting`, `type`) VALUES
('admin', 'Admin',  UNHEX('6973613230313400000000000000000000000000'),  'admin.sql.ITU.org',  1,  1),
('test',  'ghost',  UNHEX('6973613230313400000000000000000000000000'),  'test.sql.ITU.org', 1,  0),
('test2', 'gdsgsdg',  UNHEX('6973613230313400000000000000000000000000'),  'test2.sql.ITU.org',  1,  1),
('test3', 'test', UNHEX('6973613230313400000000000000000000000000'),  'test3.sql.ITU.org',  1,  2);

DROP TABLE IF EXISTS `DNSRECORD`;
CREATE TABLE `DNSRECORD` (
  `name` varchar(100) COLLATE utf8_bin NOT NULL,
  `TTL` int(32) NOT NULL,
  `type` int(8) NOT NULL,
  `IPv4` varchar(32) COLLATE utf8_bin NOT NULL,
  `IPv6` varchar(128) COLLATE utf8_bin NOT NULL,
  `domain` varchar(100) COLLATE utf8_bin NOT NULL,
  `status` int(8) NOT NULL,
  PRIMARY KEY (`domain`,`name`),
  CONSTRAINT `DNSRECORD_ibfk_1` FOREIGN KEY (`domain`) REFERENCES `DOMAIN` (`fullname`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

INSERT INTO `DNSRECORD` (`name`, `TTL`, `type`, `IPv4`, `IPv6`, `domain`, `status`) VALUES
('email', 1800, 1,  '192.168.132.2',  '', 'ITU.org',  0),
('nameserver',  1800, 0,  '192.168.0.1',  '', 'ITU.org',  0),
('test',  1800, 0,  '192.168.132.2',  '', 'ITU.org',  0);

DROP TABLE IF EXISTS `DOMAIN`;
CREATE TABLE `DOMAIN` (
  `name` varchar(100) COLLATE utf8_bin NOT NULL,
  `expiration` date NOT NULL,
  `state` int(8) NOT NULL,
  `tld` varchar(32) COLLATE utf8_bin NOT NULL,
  `client` int(32) NOT NULL,
  `fullname` varchar(100) COLLATE utf8_bin NOT NULL,
  PRIMARY KEY (`fullname`),
  KEY `client` (`client`),
  CONSTRAINT `DOMAIN_ibfk_1` FOREIGN KEY (`client`) REFERENCES `CLIENT` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

INSERT INTO `DOMAIN` (`name`, `expiration`, `state`, `tld`, `client`, `fullname`) VALUES
('ITU', '2016-01-01', 0,  'org',  1,  'ITU.org'),
('domain',  '2015-01-01', 0,  'cz', 1,  'domain.cz');

DROP TABLE IF EXISTS `FTPACCOUNT`;
CREATE TABLE `FTPACCOUNT` (
  `login` varchar(50) COLLATE utf8_bin NOT NULL,
  `password` binary(20) NOT NULL,
  `directory` varchar(32) COLLATE utf8_bin NOT NULL,
  `status` int(32) NOT NULL,
  `server` varchar(32) COLLATE utf8_bin NOT NULL,
  `webhosting` int(32) NOT NULL,
  PRIMARY KEY (`login`,`webhosting`),
  KEY `webhosting` (`webhosting`),
  CONSTRAINT `FTPACCOUNT_ibfk_1` FOREIGN KEY (`webhosting`) REFERENCES `WEBHOSTING` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

INSERT INTO `FTPACCOUNT` (`login`, `password`, `directory`, `status`, `server`, `webhosting`) VALUES
('admin', UNHEX('6E017B5464F820A6C1BB5E9F6D711A667A80D8EA'),  '\\', 0,  'ftp.ITU.org',  1),
('test',  UNHEX('6E017B5464F820A6C1BB5E9F6D711A667A80D8EA'),  '\\', 1,  'ftp.ITU.org',  1),
('test2', UNHEX('6973613230313400000000000000000000000000'),  'http://www.test.cz', 0,  'ftp.ITU.org',  1),
('xjezad',  UNHEX('6973613230313400000000000000000000000000'),  'ftp.ITU.org',  0,  '', 1);

DROP TABLE IF EXISTS `MAILBOX`;
CREATE TABLE `MAILBOX` (
  `alias` varchar(50) COLLATE utf8_bin NOT NULL,
  `password` binary(20) NOT NULL,
  `type` int(8) NOT NULL,
  `webhosting` int(32) NOT NULL,
  `used` int(32) NOT NULL DEFAULT '0',
  `size` int(32) NOT NULL DEFAULT '100',
  PRIMARY KEY (`alias`,`webhosting`),
  KEY `webhosting` (`webhosting`),
  CONSTRAINT `MAILBOX_ibfk_1` FOREIGN KEY (`webhosting`) REFERENCES `WEBHOSTING` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

INSERT INTO `MAILBOX` (`alias`, `password`, `type`, `webhosting`, `used`, `size`) VALUES
('admin', UNHEX('6E017B5464F820A6C1BB5E9F6D711A667A80D8EA'),  0,  1,  60, 100),
('info',  UNHEX('6E017B5464F820A6C1BB5E9F6D711A667A80D8EA'),  0,  1,  95, 100),
('test',  UNHEX('6E017B5464F820A6C1BB5E9F6D711A667A80D8EA'),  1,  1,  20, 100),
('test1', UNHEX('6973613230313400000000000000000000000000'),  0,  1,  0,  100),
('test2', UNHEX('6973613230313400000000000000000000000000'),  2,  1,  0,  100),
('test3', UNHEX('6973613230313400000000000000000000000000'),  0,  1,  0,  100),
('test65',  UNHEX('6973613230313400000000000000000000000000'),  1,  1,  0,  100);

DROP TABLE IF EXISTS `STATS_VISIT`;
CREATE TABLE `STATS_VISIT` (
  `hosting` int(32) NOT NULL,
  `year` int(32) NOT NULL,
  `month` int(32) NOT NULL,
  `count` int(32) NOT NULL,
  PRIMARY KEY (`hosting`,`year`,`month`),
  CONSTRAINT `STATS_VISIT_ibfk_1` FOREIGN KEY (`hosting`) REFERENCES `WEBHOSTING` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;


DROP TABLE IF EXISTS `WEBHOSTING`;
CREATE TABLE `WEBHOSTING` (
  `id` int(32) NOT NULL AUTO_INCREMENT,
  `expiration` date NOT NULL,
  `type` int(8) NOT NULL,
  `state` int(8) NOT NULL,
  `client` int(32) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `client` (`client`),
  CONSTRAINT `WEBHOSTING_ibfk_1` FOREIGN KEY (`client`) REFERENCES `CLIENT` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

INSERT INTO `WEBHOSTING` (`id`, `expiration`, `type`, `state`, `client`) VALUES
(1, '2016-01-01', 1,  1,  1);

-- 2014-11-23 22:15:56
