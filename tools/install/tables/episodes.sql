################################################################################
# Table where TVMAZE Episodes are are stored in
################################################################################

USE pci;

DROP TABLE if EXISTS episodes;

CREATE TABLE episodes ( showId int(11) NOT NULL,                                # show id must be the same like in tvmaze tabe
                        season int(11) NOT NULL,                                #
                        episode int(11) NOT NULL,                               #
                        airdate date DEFAULT NULL,                              #

                        PRIMARY KEY (showId,season,episode)

                      ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

