################################################################################
# Table where the dupes are stored in
################################################################################

USE pci;

DROP TABLE IF EXISTS dupes2;

CREATE TABLE IF NOT EXISTS dupes2 ( dupeID INT AUTO_INCREMENT,                  # unique identification of the tuple
                                    releaseName VARCHAR(128),                   # HINT: release is a reserved word in mysql so it does not need quotes or back-quotes
                                                                                #       (that do different things in many programm language)
                                    section VARCHAR(128),                       # the main section (we store this for later evaluation)
                                    content VARCHAR(128),                       # The Matrix
                                    withoutGroup VARCHAR(128),                  # The Matrix
                                    rlsGroup VARCHAR(32),                       # release Group
                                    year VARCHAR(4),                            # 1999
                                    videosource VARCHAR(16),                    # PDTV HDTV WEB VIDEOSOURCE
                                    resolution VARCHAR(16),                     # resolutin, eg. 720p 1080p
                                    episode VARCHAR(16),                        # S01E10
                                    tconst VARCHAR(12),                         # tt01234567
                                    tvmazeID VARCHAR(12),                       # 2353
                                    consoleRegion VARCHAR(16),                  #
                                    override VARCHAR(32),                       #
                                    count INT,                                  # to find mistakes, if something is wrong you would find very high count on a specific release
                                    timestamp DATETIME,                         # timestamp when the row was added

                                    PRIMARY KEY (dupeID)

                                  ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

