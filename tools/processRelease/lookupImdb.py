#!/usr/bin/python3
#coding=utf-8
# cython: language_level=3, boundscheck=False
import os
import re
import sys
import glob
import time
import signal
import requests
import traceback
import iso639
#import logging
from termcolor import colored, cprint

#add the parent directory to path
sys.path.append('.')
sys.path.append('..')

from common.dbConnector        import DBConnector
from common.helperFunctions    import HelperFunctions
from common.logger             import LoggerSingleton as logging

debug = False

#TODO: also implement in rust to see how much faster _IT would be
#TODO: first query excact year number => less waste of time

# ===================================================================================================
# class to fetch movie from imdb database
# ===================================================================================================
class LookupImdb:

  akasHD = "title_akas_tsv"
  basicsHD = "title_basics_tsv"
  ratingsHD = "title_ratings_tsv"
  tmdb = "tmdb" #make seperate mem table?

  akasMEM = "akas"
  basicsMEM = "basics"
  ratingsMEM = "ratings"

  akas = "akas"
  basics = "basics"
  ratings = "ratings"

  maxDeltaYear = 2                                                              #maximal difference between year specified in release and year in imdb
  similarity = 0.65
                                                                                #that not an older version of the movie is matched "3 idiots" from 2017 or from 2009
  def __init__(self):
    self.dbConn = DBConnector.getInstance()
    self.checkTables()


  def checkTables(self):
    self.akas = self.akasMEM
    self.basics = self.basicsMEM
    self.ratings = self.ratingsMEM
    try:
#        sql = "select count(*) from "+self.basics+";"
      sql = "select tconst from "+self.basics+" limit 5;"
      result = self.dbConn.fetchall(sql)
      if len(result)<2:                                                       #CASE EMPTY TABLE AFTER REBOOT
        self.akas = self.akasHD
        self.basics = self.basicsHD
        self.ratings = self.ratingsHD
        if debug: cprint("USING slow DB from disk, you should run install/createMemorycopy.sh", 'white', 'on_red')
        logging.error("useing SLOW DB FROM DISK, you should run install/createMemorycopy.sh")

    except:                                                                   #CASE NO TABLE AFTER FORGETTING TO MAKE MEMORYCOPY
      self.akas = self.akasHD
      self.basics = self.basicsHD
      self.ratings = self.ratingsHD
      try:
#        sql = "select count(*) from "+self.basics+";"
        sql = "select tconst from "+self.basics+" limit 5;"

        result = self.dbConn.fetchall(sql)
        print("select tconst from basics limit 5;")
        if len(result)<2:                                                     #CASE EMPTY title_basics_tsv table after some broken imdb tsv import
          if debug: cprint("no imdb DATABASE found, run install/createImdbDatabase.sh", 'white', 'on_red')
          logging.error("no imdb DATABASE found, run install/createImdbDatabase.sh")
        else:
          if debug: cprint("USING slow DB from disk, you should run install/createMemorycopy.sh", 'white', 'on_red')
          logging.error("useing SLOW DB FROM DISK, you should run install/createMemorycopy.sh")
      except:                                                                 #CASE DATABASE TABLE DOES NOT EXISTS
        if debug: traceback.print_exc(file=sys.stdout)
        cprint("DATABASE NOT FOUND", "white", "on_red")
        logging.error("no imdb DATABASE found, run install/createImdbDatabase.sh")


  def __del__(self):
    if debug: print ("DESTRUCTOR")




# ===================================================================================================
# get primaryTitle and startYear from Basics Table using tconst and release year
# ===================================================================================================
  def getBasics(self, search, year):
    if len(search)>10:
      if debug: print("stripping year for faster mysql search")
      year = None
    try:
      year = int(year)
    except:
      if debug: print("YEAR ERROR")
    logging.debug("getBasicsWithYear()")  
    if year:
      #this costs a lot of processing time for mysql, so we dont use it for long names, those dont have a lot entries that match
      sql = "SELECT tconst, primaryTitle, originalTitle, startYear, genres FROM " + self.basics + " WHERE primaryTitle like '%"+ search + "%' AND startYear >= " + str(year - 1) + " AND startYear <= " + str(year + 1) + ";" 
      #sql = "SELECT tconst, primaryTitle, originalTitle, startYear, genres FROM " + self.basics + " WHERE primaryTitle like '%"+ search + "%' ;"
    else:
      sql = "SELECT tconst, primaryTitle, originalTitle, startYear, genres FROM " + self.basics + " WHERE primaryTitle like '%"+ search + "%' ;"

    results = self.dbConn.fetchall(sql)
    
    return results

# ===================================================================================================
# get title and from Akas Table using prepared search string    
# ===================================================================================================
  def getAkas(self, search, year):
    if len(search)>10:
      if debug: print("stripping year for faster mysql search")
      year = None
    try:
      year = int(year)
    except:
      if debug: print("YEAR ERROR")
    logging.debug("getAkas()")
    if year:
      #this costs a lot of processing time for mysql, so we dont use it for long names, those dont have a lot entries that match
      sql = "SELECT titleId, title, originalTitle, startYear, genres from " + self.akas + " LEFT JOIN "+self.basics+" ON "+self.akas+".titleId = "+self.basics+".tconst where title like '%" + search + "%' AND "+self.basics+".startYear >= " + str(year - 1) + " AND "+self.basics+".startYear <= " + str(year + 1) +  ";"
    else:
      sql = "SELECT titleId, title, originalTitle, startYear, genres from " + self.akas + " LEFT JOIN "+self.basics+" ON "+self.akas+".titleId = "+LookupImdb.basics+".tconst where title like '%" + search + "%';"

    results = self.dbConn.fetchall(sql)

    return results

# ===================================================================================================
# get rating from tconst
# ===================================================================================================
  def getRating(self, tconst, indexed):

    if indexed:
      sql = "SELECT * from " + self.ratings + " where tconst = '"+ tconst + "';"
    else:
      sql = "SELECT * from " + self.ratings + " where tconst = '"+ tconst + "';"

    results = self.dbConn.fetchall(sql)
    
    if debug: print(results)
    rating = None
    nratings = None
    try:
      rating = results[0][1]
      nratings = results[0][2]
    except:
      if debug: print("NO RATING")

    return [ rating, nratings ]

# ===================================================================================================
# get originalLanguage from tconst
# ===================================================================================================
  def getOriginalLanguage(self, tconst, indexed):
    if indexed:
      sql = "SELECT originalLanguage from " + self.tmdb + " where tconst = '"+ tconst + "';"
    else:
      sql = "SELECT originalLanguage from " + self.tmdb + " where tconst = '"+ tconst + "';"

    results = self.dbConn.fetchall(sql)
    
    if debug: print(results)
    originalLanguage = None
    try:
      originalLanguage = iso639.to_name(results[0][0])
    except:
      if debug: print("NO LANGUAGE")

    return [ originalLanguage ]

# ===================================================================================================
# Finding movie in imdb based on a string (title) and use column index that makes the search faster
# ---------------------------------------------------------------------------------------------------
  def getMovieFromBasics(self, title, year, indexed):
    start = time.time()
    self.checkTables()
    
    title = title.replace(".", " ")
    if debug: print("getMovieFromBasicsWithYear()")

    search = HelperFunctions.prepareStringForSqlSearch(title)

    try:
      results = self.getBasics(search, year)
      if len(results)<1:
        logging.info("No MOVIEs NOT FOUND IN IMDB")
        raise Exception("Year Difference To Big")

      logging.info("SQL-Query Time: " + str((time.time()-start)*1000) + "ms")
      start = time.time()

      #we get all rows were the primary title matches and all rows where the original title matches
      [ PrimaryTitleSimilarity, bestPrimaryTitleRow, primaryBestDeltaYear ] = HelperFunctions.findBest(results, 1, title, 3, year)
      [ originalTitleSimilarity, bestOriginalTitleRow, originalBestDeltaYear ] = HelperFunctions.findBest(results, 2, title, 3, year)

      logging.info("similarity Time: " + str((time.time()-start)*1000) + "ms")

      if PrimaryTitleSimilarity == None and originalTitleSimilarity == None:

        raise Exception("No entries")


      if PrimaryTitleSimilarity<self.similarity and originalTitleSimilarity<self.similarity:            #check if primaryTitle and originalTitle is not completely wrong
        logging.info("Not Similar Enough: " + title + " primary: " + str(PrimaryTitleSimilarity) + " original: " + str(originalTitleSimilarity))
        raise Exception('not similar enough '+ title + " primary: " + str(PrimaryTitleSimilarity) + " original: " + str(originalTitleSimilarity))
      else:
        logging.info("found Movies: " + title + " primary: " + str(PrimaryTitleSimilarity) + " original: " + str(originalTitleSimilarity))

      if PrimaryTitleSimilarity>originalTitleSimilarity:
        tconst = bestPrimaryTitleRow[0]
        primaryTitle = bestPrimaryTitleRow[1]
        originalTitle = bestPrimaryTitleRow[2]
        imdbYear = bestPrimaryTitleRow[3]
        genres = bestPrimaryTitleRow[4]
        [ rating, nratings ] = self.getRating(tconst, indexed)
        [ originalLanguage ] = self.getOriginalLanguage(tconst, indexed)
        if primaryBestDeltaYear > self.maxDeltaYear:
          if debug: print("DELTAYEAR BIG")
          raise Exception("Year Difference To Big")
      else:
        tconst = bestOriginalTitleRow[0]
        primaryTitle = bestOriginalTitleRow[1]
        originalTitle = bestOriginalTitleRow[2]
        imdbYear = bestOriginalTitleRow[3]
        genres = bestOriginalTitleRow[4]
        [ rating, nratings ] = self.getRating(tconst, indexed)
        [ originalLanguage ] = self.getOriginalLanguage(tconst, indexed)
        if not originalBestDeltaYear == None:
          if originalBestDeltaYear > self.maxDeltaYear:
            if debug: print("DELTAYEAR BIG")
            raise Exception("Year Difference To Big")

      return [ tconst, primaryTitle, originalTitle, imdbYear, rating, nratings, genres, originalLanguage ]

    except Exception as e:
      logging.info("ERRoR: " + str(e) )
      if debug: print("EE: Exception occured")
      if debug: traceback.print_exc(file=sys.stdout)
      try:
        if debug: print("Not found in Basics Database Table, using Akas Database Table")
        logging.info("Not found in Basics Database Table, using Akas Database Table")
        if debug: traceback.print_exc(file=sys.stdout)
        return self.getMovieFromAkas(title, year, indexed)
      except:
        if debug: print("Not found in Akas")
        logging.info("MOVIE NOT FOUND, also not found in Akas Database Table")
        if debug: traceback.print_exc(file=sys.stdout)

      return [ None, None, None, None, None, None, None, None ]


# ===================================================================================================
# Finding movie in imdb based on a string (pName) and use column index that makes the search faster
# ---------------------------------------------------------------------------------------------------
  def getMovieFromAkas(self, title, year, indexed):
    logging.debug("getMovieFromAkas()") 

    search = HelperFunctions.prepareStringForSqlSearch(title)
    results = self.getAkas(search, year)

    [ bestSimilarity, bestRow, deltaYear ] = HelperFunctions.findBest(results, 1, title, 3, year)

    if bestSimilarity<self.similarity:
      logging.info("Not Similar Enough: " + title)
      raise Exception('not similar enough')

    try:
      tconst = bestRow[0]
      primaryTitle = bestRow[1]
      originalTitle = bestRow[2]
      imdbYear = bestRow[3]
      genres = bestRow[4]
      [ rating, nratings ] = self.getRating(tconst, indexed)
      [ originalLanguage ] = self.getOriginalLanguage(tconst, indexed)

      if deltaYear > self.maxDeltaYear:
        if debug: print("DELTAYEAR BIG")
        raise Exception("Akas Year Difference To Big")

      return [ tconst, primaryTitle, originalTitle, imdbYear, rating, nratings, genres, originalLanguage ]

    except:
      if debug: print("EE: Exception occured")
      if debug: traceback.print_exc(file=sys.stdout)
      return [ None, None, None, None, None, None, None, None ]


def main():

  movieNames = [[ "The Matrix", 1999, True, "['tt0133093', 'The Matrix', 'The Matrix', '1999'"],
                [ "The Matrix revolut", 2003, True, "['tt0242653', 'The Matrix Revolutions', 'The Matrix Revolutions', '2003'," ],
                [ "Krieg der Sterne", 1977, True, "['tt0076759', 'Krieg der Sterne', 'Star Wars', '1977'," ],
                [ "Life Of The Party", 2018, True, "['tt5619332', 'Life of the Party', 'Life of the Party', '2018', " ],
                [ "23rd Psalm Branch", 1969, True, "['tt6391052', '23rd Psalm Branch', '23rd Psalm Branch', '1968'," ],
                [ "Slaughterhouse Rulez", 2018, True, "['tt6905696', 'Slaughterhouse Rulez', 'Slaughterhouse Rulez', '2018', "],
                #these are tests for the akas database since "the matrix" is written in frencht (la matrice)
                [ "La matrice", 1982, True, "['tt8205972', \"De la matrice à l'asile\", \"De la matrice à l'asile\", '1982'," ],
                [ "La matrice", 1995, True, "[None, None, None, None, None, None, None]" ],
                [ "La matrice", 1998, True, "['tt0133093', 'La matrice', 'The Matrix', '1999',"],
                [ "3 Idiots", 2009, True, "['tt1187043', '3 Idiots', '3 Idiots', '2009'," ],
                [ "3 Idiots", 2017, True, "['tt3685624', '3 Idiotas', '3 Idiotas', '2017', " ],
                [ "Ident", 1990, True, "THIS ONE DOES NOT WORK BECAUSE WE DONT HAVE THE SHORT MOVIES IN THE MEMORY COPY" ],              #TODO: is that ok?
                [ "101 Dalmatians 2", 2003, True, "['tt0324941', \"101 Dalmatians 2: Patch's London Adventure\", \"101 Dalmatians II: Patch's London Adventure\", '2002'"],
                [ "IT", 2017, True, "['tt1396484', 'It', 'It', '2017'," ],
                [ "IT", 1990, True, "DOES NOT WORK BECAUSE ITS NOT A MOVIE BUT A Mini-Seris" ]
               ]

  il = LookupImdb()
    
  if len(sys.argv)<2:

    print()
    print()
    cprint("U N I T  T E S T s  FOR IMDB:", 'red', 'on_white')
    print()



    for movieName in movieNames:
      start = time.time()
      print(movieName[0] + " ("+str(movieName[1])+")")
      result = il.getMovieFromBasics(movieName[0], movieName[1], movieName[2])
      if result[0] == None:
        il.akas = "title_akas_tsv"
        il.basics = "title_basics_tsv"
        il.ratings = "title_ratings_tsv"
        result = il.getMovieFromBasics(movieName[0], movieName[1], movieName[2])
        il.akas = "akas"
        il.basics = "basics"
        il.ratings = "ratings"

      if movieName[3] in str(result):
        cprint(result, 'white', 'on_green')
      else:
        cprint(result, 'white', 'on_red', end="")
        print(" should be: ", end="")
        cprint(movieName[3], 'white', 'on_blue')

      print("Time: " + str((time.time()-start)*1000) + "ms")

      print("--------------------------------------------------------------------------------")

    print()
    print("You can also specify a Movie and a year: ", end="")
    cprint("python3 imdbLookup.py \"The Matrix\" 1999", 'cyan')
    print()
  
  else:
    if len(sys.argv)>2:
      result =  il.getMovieFromBasics(sys.argv[1], int(sys.argv[2]), False)
      print(result)
    else:
      print("you need to specify the year: ", end="")
      cprint("python3 imdbLookup.py \"The Matrix\" 1999", 'white', 'on_red')

#VERY SLOW, 20 SEConds: 10x10.2018.1080p.BluRay.x264-PSYCHD
#50.50.2011.PROPER.1080p.BluRay.x264-REFiNED

  
if __name__ == "__main__": main()

