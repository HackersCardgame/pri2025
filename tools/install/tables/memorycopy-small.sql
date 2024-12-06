USE pci;

CREATE TABLE IF NOT EXISTS basics_small like title_basics_tsv;
ALTER TABLE basics_small ENGINE=MEMORY;
REPLACE INTO basics_small SELECT * FROM title_basics_tsv where (titleType = 'movie' or titleType = 'video') AND startYear >= "2019";
create index IF NOT EXISTS bs_pt on basics_small(primaryTitle);
create index IF NOT EXISTS bs_ot on basics_small(originalTitle);

CREATE TABLE IF NOT EXISTS akas_small like title_akas_tsv;
ALTER TABLE akas_small ENGINE=MEMORY;
REPLACE INTO akas_small SELECT * FROM title_akas_tsv WHERE titleId IN (SELECT tconst FROM basics_small);
create index IF NOT EXISTS as_ot on akas_small(title);
create index IF NOT EXISTS as_tt on akas_small(titleId);

CREATE TABLE IF NOT EXISTS ratings_small like title_ratings_tsv;
ALTER TABLE ratings_small ENGINE=MEMORY;
REPLACE INTO ratings_small SELECT * FROM title_ratings_tsv WHERE tconst IN (SELECT tconst FROM basics);
create index rs_tt on ratings_small(tconst);

