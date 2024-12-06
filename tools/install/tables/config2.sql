################################################################################
# Table where the site / user / general configs are stored in
################################################################################

USE pci;

DROP TABLE IF EXISTS config2;

CREATE TABLE IF NOT EXISTS config2 ( configFile VARCHAR(64),                    # SITE1.cfg, RATS.cfg or general.cfg
                                     parameter VARCHAR(64),                     # these are things like rule, keyword, topic, feature, order...
                                     modifier VARCHAR(64),                      # these are things like MOVIE.X264, GAME, GAME.WIN
                                     term VARCHAR(10000),                       # rule TV2 = TV 720p group:(ALL) episodeAge:812 h264 x264 tvid:(ALL) web webrip hdtv 
                                                                                #            animation scripted dupesweb dupeshdtv std network:(hbo)
                                     notes VARCHAR(10000),                      # for notes / comments like who chainged the row last
                                     lineNumber INT,

                                     PRIMARY KEY (configFile,parameter,modifier)

                                   ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

