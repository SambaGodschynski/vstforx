-- phpMyAdmin SQL Dump
-- version 3.5.8.1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Erstellungszeit: 12. Mai 2013 um 14:46
-- Server Version: 5.1.66-0+squeeze1-log
-- PHP-Version: 5.3.3-7+squeeze15

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Datenbank: `usr_web22_1`
--

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `forx_beta_download`
--

CREATE TABLE IF NOT EXISTS `forx_beta_download` (
  `os` varchar(20) NOT NULL,
  `version` varchar(20) NOT NULL,
  `flag` int(11) DEFAULT '0',
  `built` date DEFAULT NULL,
  `ext` varchar(10) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  PRIMARY KEY (`os`,`version`),
  UNIQUE KEY `built` (`built`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `frx_donations`
--

CREATE TABLE IF NOT EXISTS `frx_donations` (
  `juser` int(25) NOT NULL,
  `transaction_id` varchar(255) NOT NULL,
  `email` varchar(255) NOT NULL,
  `amount` double NOT NULL,
  `rquest` varchar(255) NOT NULL,
  UNIQUE KEY `transaction_id` (`transaction_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `frx_downloads`
--

CREATE TABLE IF NOT EXISTS `frx_downloads` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `productid` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  `path` varchar(255) NOT NULL,
  `filename` varchar(50) NOT NULL,
  `date` date NOT NULL,
  `osid` int(11) NOT NULL,
  `noteid` int(11) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=21 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `frx_download_notes`
--

CREATE TABLE IF NOT EXISTS `frx_download_notes` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `text` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=7 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `frx_juser_download`
--

CREATE TABLE IF NOT EXISTS `frx_juser_download` (
  `juser` int(255) NOT NULL,
  `download` int(255) NOT NULL,
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `ip` varchar(50) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `frx_products`
--

CREATE TABLE IF NOT EXISTS `frx_products` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `description` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=7 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `frx_product_os`
--

CREATE TABLE IF NOT EXISTS `frx_product_os` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(25) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `frx_selled`
--

CREATE TABLE IF NOT EXISTS `frx_selled` (
  `juser` int(25) NOT NULL,
  `transaction_id` varchar(255) NOT NULL,
  `email` varchar(255) NOT NULL,
  `amount` double NOT NULL,
  `rquest` varchar(255) NOT NULL,
  `productid` varchar(50) NOT NULL,
  `date` date NOT NULL,
  UNIQUE KEY `transaction_id` (`transaction_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `frx_selled_response_mail`
--

CREATE TABLE IF NOT EXISTS `frx_selled_response_mail` (
  `productid` int(11) NOT NULL,
  `subject` varchar(255) NOT NULL,
  `text` text NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `frx_selling_event`
--

CREATE TABLE IF NOT EXISTS `frx_selling_event` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `productid` int(11) NOT NULL,
  `text` varchar(255) NOT NULL DEFAULT '-',
  `amount` float NOT NULL,
  `amount_txt` varchar(255) NOT NULL,
  `begin` date DEFAULT NULL,
  `end` date DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=6 ;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
