################################################################################
# Table where the site / user / general configs are stored in
################################################################################

USE pci;

DROP TABLE IF EXISTS title_basics_tsv;

CREATE TABLE IF NOT EXISTS title_basics_tsv ( tconst VARCHAR(12) PRIMARY KEY,   # tt0297718
                                              titleType VARCHAR(16),            # tvMiniSeries
                                              primaryTitle VARCHAR(128),        # Canned Foods Are Not Just for Emergencies Anymore, They Serve as Appetizers and Side Dishes,
                                                                                # What Are the Most Popular Canned Food Items/Scientifically Analyzing Fatigue: The Development of the Fatigue Meter
                                              originalTitle VARCHAR(128),       # Breaking News on the Ebola Virus/Extremely Odd Cravings/Skin Feels Like Glass/Sudden Loss of Taste,
                                                                                # and Smell/Surprising Reasons for Your Digestive Disorders
                                              isAdult VARCHAR(1),               # 0/1
                                              startYear VARCHAR(4),             # 19xx
                                              endYear VARCHAR(4),               # 19xx
                                              runtimeMinutes VARCHAR(6),        # 125156
                                              genres VARCHAR(64)                #

                                            ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS title_ratings_tsv;

CREATE TABLE IF NOT EXISTS title_ratings_tsv ( tconst VARCHAR(12) PRIMARY KEY,  # tt0297718
                                               averageRating VARCHAR(4),        # 8.8
                                               numVotes VARCHAR(7)              # 2346234

                                             ) ENGINE=InnoDB DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS title_akas_tsv;

CREATE TABLE IF NOT EXISTS title_akas_tsv    ( titleId VARCHAR(12),             # tt0076320
                                               ordering VARCHAR(4),             # 131 (maximal 3 stellig)
                                               title VARCHAR(128),              # there are some few that are longer than 128 chars, about 500 movies or junk
                                               region VARCHAR(4),               # 4
                                               language VARCHAR(3),             # 3
                                               types VARCHAR(16),               # festivalworking
                                               attributes VARCHAR(64),          # added framing sequences and narration in Yiddishreissue title
                                               isOriginalTitle VARCHAR(1),      # 0/1

                                               PRIMARY KEY (titleId,ordering) 

                                             ) ENGINE=InnoDB DEFAULT CHARSET=utf8;


