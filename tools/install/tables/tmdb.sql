################################################################################
# Table where the site / user / general configs are stored in
################################################################################

USE pci;

DROP TABLE IF EXISTS tmdb;

CREATE TABLE IF NOT EXISTS tmdb ( id int PRIMARY KEY,                           # tmdb id
                                  tconst VARCHAR(12),                           # tt0297718
                                  originalLanguage VARCHAR(8),                  # en, da
                                  updated DATETIME DEFAULT NULL                 # yyyy-mm-dd

                                ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

