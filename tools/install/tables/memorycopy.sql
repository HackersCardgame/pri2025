USE pci;

CREATE TABLE IF NOT EXISTS basics like title_basics_tsv;
ALTER TABLE basics ENGINE=MEMORY;
#"INSERT IGNORE INTO basics SELECT * FROM title_basics_tsv where titleType = 'movie' or titleType = 'video';
REPLACE INTO basics SELECT * FROM title_basics_tsv where titleType = 'movie' or titleType = 'video';
ALTER TABLE basics ADD INDEX basics_index (originalTitle);

CREATE TABLE IF NOT EXISTS akas like title_akas_tsv;
ALTER TABLE akas ENGINE=MEMORY;
#INSERT IGNORE INTO akas SELECT * FROM title_akas_tsv WHERE titleId IN (SELECT tconst FROM " + basics + ");
REPLACE INTO akas SELECT * FROM title_akas_tsv WHERE titleId IN (SELECT tconst FROM basics);
ALTER TABLE akas ADD INDEX akas_index (title);

CREATE TABLE IF NOT EXISTS ratings like title_ratings_tsv;
ALTER TABLE ratings ENGINE=MEMORY;
#INSERT IGNORE INTO ratings SELECT * FROM title_ratings_tsv WHERE tconst IN (SELECT tconst FROM " + basics + ");
REPLACE INTO ratings SELECT * FROM title_ratings_tsv WHERE tconst IN (SELECT tconst FROM basics);
ALTER TABLE ratings ADD INDEX ratings_index (tconst);

