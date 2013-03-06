-- phpMyAdmin SQL Dump
-- version 3.5.7
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Mar 06, 2013 at 08:51 PM
-- Server version: 5.1.66-0+squeeze1-log
-- PHP Version: 5.3.3-7+squeeze15

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `usr_web22_1`
--

-- --------------------------------------------------------

--
-- Table structure for table `forx_beta_download`
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

--
-- Dumping data for table `forx_beta_download`
--

INSERT INTO `forx_beta_download` (`os`, `version`, `flag`, `built`, `ext`) VALUES
('win32', '0.2.124', 0, '2010-12-12', 'exe'),
('win32', '0.2.120', 0, NULL, 'exe'),
('win32', '0.2.115', 0, NULL, 'exe'),
('win32', '0.2.117', 1, NULL, 'exe'),
('win32', '0.2.118', 0, NULL, 'exe'),
('win32;mac', '0.3.223', 1, '2011-04-02', 'zip'),
('win32;mac', '0.4.275', 1, '2011-07-07', 'zip'),
('win32;mac', '0.4.300', 0, '2011-07-22', 'zip'),
('win32;mac', '0.4.444', 1, '2012-04-10', 'zip');

-- --------------------------------------------------------

--
-- Table structure for table `frx_donations`
--

CREATE TABLE IF NOT EXISTS `frx_donations` (
  `juser` int(25) NOT NULL,
  `transaction_id` varchar(255) NOT NULL,
  `email` varchar(255) NOT NULL,
  `amount` double NOT NULL,
  `rquest` varchar(255) NOT NULL,
  UNIQUE KEY `transaction_id` (`transaction_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `frx_donations`
--

INSERT INTO `frx_donations` (`juser`, `transaction_id`, `email`, `amount`, `rquest`) VALUES
(0, 'x', 'x', 0, '0'),
(411, '4J700035P1391342C', '_samba_1358112367_pre@googlemail.com', 117.9, 'mc_gross=123.00&protection_eligibility=Partially+Eligible+-+INR+Only&address_status=unconfirmed&payer_id=ZQKG45BV3XNW8&tax=0.00&address_street=ESpachstr.+1&payment_date=14%3A11%3A33+Jan+13%2C+2013+PST&payment_status=Completed&charset=windows-1252&address_');

-- --------------------------------------------------------

--
-- Table structure for table `frx_downloads`
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
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=14 ;

--
-- Dumping data for table `frx_downloads`
--

INSERT INTO `frx_downloads` (`id`, `productid`, `name`, `path`, `filename`, `date`, `osid`, `noteid`) VALUES
(1, 3, 'VSTForx-DevTest', 'win/', 'vstforx-devtest-win.zip', '2013-04-08', 1, 1),
(2, 3, 'VSTForx-DevTest', 'mac/', 'vstforx-devtest-mac.zip', '2013-04-07', 2, 2),
(7, 2, 'VSTForx-0.4.444', 'win/', 'VSTForx-0.4.444-win.zip', '2012-04-10', 1, 1),
(4, 2, 'VSTForx-0.4.300', 'win/', 'VSTForx-0.4.300-win.zip', '2011-07-26', 1, 1),
(5, 2, 'VSTForx-0.4.300', 'mac/', 'VSTForx-0.4.300-mac.zip', '2011-07-26', 2, 1),
(3, 2, 'VSTForx-0.4.444', 'mac/', 'VSTForx-0.4.444-mac.zip', '2012-04-09', 2, 1),
(8, 2, 'VSTForx-0.4.275', 'win/', 'VSTForx-0.4.275-win.zip', '2011-07-07', 1, 3),
(9, 2, 'VSTForx-0.4.275', 'mac/', 'VSTForx-0.4.275-mac.zip', '2011-07-07', 2, 3),
(10, 2, 'VSTForx-0.3.223', 'win/', 'VSTForx-0.3.223-win.zip', '2011-06-20', 1, 3),
(11, 2, 'VSTForx-0.3.223', 'mac/', 'VSTForx-0.3.223-mac.zip', '2011-06-20', 2, 1),
(12, 2, 'VSTForx-0.2.124', 'win/', 'VSTForx-0.2.124-win.exe', '2011-02-10', 1, 1),
(13, 2, 'VSTForx-0.2.120', 'win/', 'VSTForx-0.2.120-win.exe', '2010-12-01', 1, 1);

-- --------------------------------------------------------

--
-- Table structure for table `frx_download_notes`
--

CREATE TABLE IF NOT EXISTS `frx_download_notes` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `text` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=4 ;

--
-- Dumping data for table `frx_download_notes`
--

INSERT INTO `frx_download_notes` (`id`, `text`) VALUES
(1, '-'),
(2, 'this version is for developer purpose only. you should never see this note.'),
(3, 'saved banks are not compatible to previous versions');

-- --------------------------------------------------------

--
-- Table structure for table `frx_products`
--

CREATE TABLE IF NOT EXISTS `frx_products` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `description` varchar(255) NOT NULL,
  `amount` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5 ;

--
-- Dumping data for table `frx_products`
--

INSERT INTO `frx_products` (`id`, `name`, `description`, `amount`) VALUES
(2, 'VSTForx.beta', 'The <b>outdated</b> beta version of VSTForx.', 0),
(3, 'VSTForx', 'the <b>pre-release</b> version of  VSTForx.', 15);

-- --------------------------------------------------------

--
-- Table structure for table `frx_product_os`
--

CREATE TABLE IF NOT EXISTS `frx_product_os` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(25) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=3 ;

--
-- Dumping data for table `frx_product_os`
--

INSERT INTO `frx_product_os` (`id`, `name`) VALUES
(1, 'Windows'),
(2, 'MacOs');

-- --------------------------------------------------------

--
-- Table structure for table `frx_selled`
--

CREATE TABLE IF NOT EXISTS `frx_selled` (
  `juser` int(25) NOT NULL,
  `transaction_id` varchar(255) NOT NULL,
  `email` varchar(255) NOT NULL,
  `amount` double NOT NULL,
  `rquest` varchar(255) NOT NULL,
  `productid` varchar(50) NOT NULL,
  UNIQUE KEY `transaction_id` (`transaction_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `frx_selled`
--

INSERT INTO `frx_selled` (`juser`, `transaction_id`, `email`, `amount`, `rquest`, `productid`) VALUES
(66, '2SL99756S5645843B', '_samba_1358112367_pre@googlemail.com', 14.11, 'mc_gross=15.00&protection_eligibility=Ineligible&payer_id=ZQKG45BV3XNW8&tax=0.00&payment_date=06%3A50%3A26+Feb+07%2C+2013+PST&payment_status=Completed&charset=windows-1252&first_name=sascha&mc_fee=0.89&notify_version=3.7&custom=66&payer_status=verified&bu', '3'),
(65, '5NK042816P669161W', '_samba_1358112367_pre@googlemail.com', 14.11, 'mc_gross=15.00&protection_eligibility=Ineligible&payer_id=ZQKG45BV3XNW8&tax=0.00&payment_date=01%3A29%3A13+Feb+08%2C+2013+PST&payment_status=Completed&charset=windows-1252&first_name=sascha&mc_fee=0.89&notify_version=3.7&custom=65&payer_status=verified&bu', '3'),
(0, '', 'defaultuser@vstforx.de', 0, '', '2');

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
