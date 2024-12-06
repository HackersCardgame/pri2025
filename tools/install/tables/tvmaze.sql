################################################################################
# Table for tvmaze data (except episodes, they are in table episodes
################################################################################

USE pci;

DROP TABLE IF EXISTS tvmaze;

CREATE TABLE tvmaze ( id int PRIMARY KEY,                                       # 1, 2, 1412
                      title VARCHAR(128),                                       # Under The Dome, Big Bang Theorie, ....
                      type VARCHAR(16),                                         # Scripted, Reality, Animation, Talk Show, Documentary, Sports, Variety, Panel Show, News, Game Show, Award Show 
                      language VARCHAR(16),                                     # US, DE, ...
                      genre VARCHAR(128),                                       # Drama, Food, Romance, Crime, Romance, Thriller, Adventure, Anime, Comedy, Children, Fantasy ....
                      status VARCHAR(16),                                       # Running, Ended, "To Be Determined", "In Development"
                      premiere DATE DEFAULT NULL,                               # 2009-11-26
                      network VARCHAR(32),                                      # FOX 
                      networkWeb VARCHAR(32),                                   # NETFLIX
                      countryCode VARCHAR(4),                                   # Germany, United States, Switzerland for Der Bestatter ...
                      currentSeason TINYINT DEFAULT 0,                          # season number
                      airdatePrevious DATE,                                     # airdate
#                     nextepisode                                               # possibly not needed and not in tvamze-api
                      updated DATETIME DEFAULT NULL                             # we use here a timestamp

                    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

# create indexed cols for speed up lookup  #TODO: add covering_index
################################################################################
ALTER TABLE tvmaze ADD INDEX covering_index (title);
