################################################################################
# Table where the site / user / general configs are stored in
################################################################################

USE pci;

DROP TABLE if EXISTS mojo;

CREATE TABLE mojo ( tconst VARCHAR(12) PRIMARY KEY,                             # tt0297718
	                  filename VARCHAR(16),                                       # startrek2016.htm
                    usScreens INT,                                              # 561
                    ukScreens INT,                                              # 296
                    mojoTitle VARCHAR(128)                                      # black panther (this we only store for later debugging, this filed is not really required)

                  ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#required for fulltext search speedup
################################################################################
ALTER TABLE title_basics_tsv ADD FULLTEXT(tconst);

