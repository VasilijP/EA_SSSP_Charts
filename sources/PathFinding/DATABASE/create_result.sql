# Host: localhost
# Database: ea
# Table: 'result'
# 
CREATE TABLE `result` (
  `id` int(11) NOT NULL auto_increment,
  `alg` char(32) NOT NULL default '',
  `vertcount` bigint(20) unsigned NOT NULL default '0',
  `edgecount` bigint(20) unsigned default NULL,
  `elenadd` bigint(20) unsigned NOT NULL default '0',
  `conn` bigint(20) unsigned NOT NULL default '0',
  `task` bigint(20) unsigned default NULL,
  `cycles` bigint(20) unsigned default NULL,
  `batchname` char(32) NOT NULL default '',
  `result` text,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8; 