CREATE DATABASE `sensordata`;
USE `sensordata`;
CREATE TABLE IF NOT EXISTS `variable` (
  `id` varchar(30) NOT NULL AUTO_INCREMENT,
  `date` datetime NOT NULL,
  `sensorid` varchar(100) NOT NULL,
  `value` float NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;