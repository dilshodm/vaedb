USE vaedb;

CREATE TABLE `kvstore` (
  `subdomain` char(25) NOT NULL,
  `k` varchar(191) NOT NULL,
  `v` text NOT NULL,
  `expire_at` datetime DEFAULT NULL,
  `is_filename` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`subdomain`,`k`),
  KEY `subdomain_v` (`subdomain`,`v`(64))
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE `locks` (
  `subdomain` varchar(25) DEFAULT NULL,
  `created_at` datetime DEFAULT NULL,
  UNIQUE KEY `subdomain` (`subdomain`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE `session_data` (
  `id` char(33) NOT NULL,
  `subdomain` char(25) DEFAULT NULL,
  `data` longtext NOT NULL,
  `expires` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE `slow_queries` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `subdomain` varchar(25) DEFAULT NULL,
  `runtime` float DEFAULT NULL,
  `created_at` datetime DEFAULT NULL,
  `query` text,
  `options` text,
  PRIMARY KEY (`id`),
  KEY `subdomain` (`subdomain`,`runtime`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4;

CREATE TABLE `slow_times` (
  `query` text,
  `total_time` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;


