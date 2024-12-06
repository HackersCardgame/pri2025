################################################################################
# Table where the the command and release history is stored
################################################################################

USE pci;

DROP TABLE IF EXISTS history;

CREATE TABLE IF NOT EXISTS history ( configFile VARCHAR(64) NOT NULL,           # user01.cfg, RATS.cfg or general.cfg
                                     action VARCHAR(64) NOT NULL,               # udp2cbftp, phaseClient, pythonImport ...
                                     command VARCHAR(256),
                                     releaseName VARCHAR(256),
                                     time DATETIME(3),

                                     PRIMARY KEY (configFile,time)

                                   ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

