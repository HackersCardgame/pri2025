#!/usr/bin/python3
#coding=utf-8

import os
import sys
import urllib.request
import json
import time
import signal
#import datetime
#from datetime import datetime, timedelta

import traceback
from datetime import datetime
from termcolor import colored, cprint

debug = False

sys.path.append( os.path.abspath(os.path.join(os.path.realpath(__file__), "../..")))
basepath = os.path.dirname(os.path.abspath(__file__))

#from lib.dbConnector      import DBConnector
from common.dbConnector        import DBConnector

class TmdbFetcher:

  table = "tmdb"
  api_key = "apikey"

  def __init__(self):
    signal.signal(signal.SIGINT, self.signal_handler)
    self.dc = DBConnector.getInstance()
    self.updated = datetime.now()
    print('to Interrupt press Ctrl+C')


  def signal_handler(self, sig, frame):
    print()
    print('You pressed Ctrl+C! Commiting changes')
    self.dc.mariadb_connection.commit()
    self.dc.mariadb_connection.close()
    sys.exit(0)


# ===================================================================================================
# update values from fetch in the database
# we need this part multiple times so we have an own function for getting json files from URL
# ===================================================================================================
  def getJSON(self, url):
    if debug: print("entering getJSON()")
    fetched = False
    count = 0
    cont = ""

    while not fetched and count < 3:
      count = count + 1
      try:
        req = urllib.request.urlopen(url, timeout=3)
        fetched = True
      except urllib.error.HTTPError as e:
      # Return code error (e.g. 404, 501, ...)
        print('\r EE: HTTPError: {}'.format(e.code) + " (trying another " + str(3-count) + " times)", end="")
        if debug: traceback.print_exc(file=sys.stdout)
        time.sleep(.250)
      except urllib.error.URLError as e:
      # Not an HTTP-specific error (e.g. connection refused)
        print('\r EE: URLError: {}'.format(e.reason) + " (trying another " + str(3-count) + " times)", end="")
        if debug: traceback.print_exc(file=sys.stdout)
        time.sleep(.250)
      else:
      # 200
        if debug: print('ii: fetch good, 200')
      if debug: print(req)

    ##parsing response
    try:
      r = req.read()
      if debug: print(r)
      cont = json.loads(r.decode('utf-8'))
    except:
      print()
      print("Failed 3 Times, giving up: " + url)
      raise Exception("no Data from URL")

    if debug: 
      for key, value in cont.items():
        print("CONT: Key: "+str(key)+ " Value: "+str(value))

    time.sleep(.250)

    return cont
    
    
# ===================================================================================================
# get the values from themoviedb api movieId
# ===================================================================================================
  def parseJSON(self, json, lookupValues):
    if debug: print("entering parseJSON()")
    results = []
    companies = []
    rdDigital = [ "NULL", "NULL" ]
    rdPhysical = [ "NULL", "NULL"]
    if json:
      for value in lookupValues:
        try:
          if len(value) == 2:
            value[0] = json[value[1]]
          if len(value) == 3:
            value[0] = json[value[1]][value[2]]
          if len(value) == 4:
            value[0] = json[value[1]][value[2]][value[3]]
        except:
          value[0] = None

        #production companies are a dictonary in a list
        if type(value[0]) == list:
          if not len(value[0]) == 0:
            print(value[0])
            for dicts in value[0]:
              if 'name' in dicts:
                companies.append(dicts['name']) #TODO: there is probably a better way to do this
                value[0] = ",".join(companies)
              if 'release_dates' in dicts:
                for value2 in dicts['release_dates']:
                  if value2['type'] == 4:
                    date = value2['release_date'].split("T")
                    rdDigital.append(date[0])
                  if value2['type'] == 5:
                    date = value2['release_date'].split("T")
                    rdPhysical.append(date[0])
                  if not len(rdDigital) == 0:
                    tmp = min(rdDigital)
                    rdDigital.clear()
                    rdDigital.append(tmp)
                  if not len(rdPhysical) == 0:
                    tmp = min(rdPhysical)
                    rdPhysical.clear()
                    rdPhysical.append(tmp)

#            value[0] = ",".join(companies)
                  value[0] = ",".join(rdDigital + rdPhysical)

    for value in lookupValues:
      results.append(value[0])

    return results


# ===================================================================================================
# get movie values and insert them into the database
# ===================================================================================================
  def getMovie(self, movieId):
    if debug: print("entering getMovie()")
    try:
      json = self.getJSON("https://api.themoviedb.org/3/movie/"+str(movieId)+"?api_key="+str(self.api_key)+"&append_to_response=release_dates")
    except:
      return "error"

    #this values structure defines the return vealue, each will be initialized with eg "" and will get the values in the first subelement
    values = [["genres", "id"],["genres", "imdb_id"],["genres", "original_language"],["", "production_companies"],["", "release_dates", "results"]]
    try:
      self.insertMovie(movieId, self.parseJSON(json, values))
      return ("inserted")
    except:
      print()
      print("Movie IMPORT ERROR: " + str(movieId))
      if debug: traceback.print_exc(file=sys.stdout)
      return "error"
    return self.parseJSON(json, values)


# ===================================================================================================
# insert single movie
# ===================================================================================================
  def insertMovie(self, movieId, values):
    if debug: print("entering insertMovie() {}".format(values))
    id, ttimdb, originalLanguage, productionCompany, releaseDates = values

    rdDigital, rdPhysical = releaseDates.split(",")

    sql = "REPLACE INTO " + self.table + " (id, tconst, originalLanguage, productionCompany, rdDigital, rdPhysical, updated) VALUES ('" + \
                                            str(id)               + "', '" + str(ttimdb)            + "', " + repr(str(originalLanguage)) + ", " + \
                                            repr(str(productionCompany)) + ", " + repr(rdDigital) + ", " + repr(rdPhysical) + "," + \
                                            repr(self.updated.strftime("%Y-%m-%d %H:%M:%S")) + ");"
    if debug: print(sql)
    try:
      if not debug: self.dc.execute(sql)
      if debug: print("Debug is TRUE, no insert")
    except mariadb.Error as error:
      print()
      print(sql)
      raise Exception("MovieInsertFailed")
    finally:
      self.dc.mariadb_connection.commit()  #if you commit each line seperately it will take 12h to import all, so we commit at the end of each file


# ===================================================================================================
# get max date so we know where to start for updating
# ===================================================================================================
  def getMaxDate(self):
    sql = "SELECT MAX(updated) FROM " + self.table + ";"

    if debug: print(sql)
    try:
      self.dc.execute(sql)
    except mariadb.Error as error:
      if debug: traceback.print_exc(file=sys.stdout)
      return "error"
      sys.exit(0)
    date = self.dc.cursor.fetchone()

    return date[0].strftime("%Y-%m-%d")


# ===================================================================================================
# check if the movie already exist in the database
# ===================================================================================================
  def getExist(self, id):
    sql = "SELECT * FROM " + self.table + " where id='" + str(id) + "';"

    result = False
    if debug: print(sql)
    try:
      self.dc.execute(sql)
      data = self.dc.cursor.fetchall()
    except mariadb.Error as error:
      if debug: traceback.print_exc(file=sys.stdout)
      return "error"
      sys.exit(0)
    if debug: print(data)
    if len(data)>0:
      result = True

    return result


# ===================================================================================================
# memorycopie
# ===================================================================================================
  def createMemCopySmall(self, cron):

    if not cron == None:
      f = open(self.logfile, "a+")
      f.write("%boldImporting IMDB languages into memorydb...\n")
      f.close()

    print("CREATING small Memory Copies")
    
    #these sql commands are to create a copy of titles_basics_tsv in memory that is faster than from ssd
    memoryCopyBasics1 = "CREATE TABLE tmdb_small like tmdb;"
    memoryCopyBasics2 = "ALTER TABLE tmdb_small ENGINE=MEMORY;"
#    memoryCopyBasics3 = "INSERT IGNORE INTO basics SELECT * FROM title_basics_tsv where titleType = 'movie' or titleType = 'video';"  #TODO: OVERWRITE NOT IGNORE
    memoryCopyBasics3 = "REPLACE into tmdb_small SELECT * FROM tmdb WHERE tconst IN (SELECT tconst FROM basics_small);"
#    memoryCopyBasics4 = "ALTER TABLE basics_small ADD INDEX basics_index_small (originalTitle);"  #TODO: primary TITLE? use index?
    memoryCopyBasics5 = "create index ts_tt on tmdb_small(tconst);"
#    memoryCopyBasics6 =  "create index bs_ot on basics_small(originalTitle);"

    try:
      self.dc.modify(memoryCopyBasics1)
    except:
      if debug: traceback.print_exc(file=sys.stdout)
      print("TMDB already Exists")

    try:
      self.dc.modify(memoryCopyBasics2)
    except:
      if debug: traceback.print_exc(file=sys.stdout)
      print("TMDB Engine already Memory")

    print("INSERTING new rows")
    if not cron == None:
      f = open(self.logfile, "a+")
      f.write("Inserting new rows for IMDB languages...\n")
      f.close()
    try:
      self.dc.modify(memoryCopyBasics3)
    except:
      if debug: traceback.print_exc(file=sys.stdout)
      print("could not insert any/all rows in IMDB languages table")
      if not cron == None:
        f = open(self.logfile, "a+")  
        f.write("%redCould not insert any/all rows in IMDB languages table...\n")
        f.close()
#    print("creating index")      
#    try:
#      self.dbConn.modify(memoryCopyBasics4)
#    except:
#      if debug: traceback.print_exc(file=sys.stdout)
#      print("Index already exists for BASICS")

    print("creating index ts_tt")
    try:
      self.dc.modify(memoryCopyBasics5)
    except:
      if debug: traceback.print_exc(file=sys.stdout)
      print("Index already: index ts_tt on tmdb_small(tconst)")


# ===================================================================================================
# main
# ===================================================================================================

def main():

  import json

  tf = TmdbFetcher()

  GREEN = '\033[92m'
  YELLOW = '\033[93m'
  RED = '\033[91m'

  if len(sys.argv)<2:
    cprint("IMPORTER FOR THEMOVIEDB:", 'red', 'on_white')
    print()
    print("To make the first import type (needs a long time, dont do this on a productive System):")
    print()
    cprint("TMDBimport.py --fileimport <filename>", 'red')
    print()
    print()
    print("To update the existing database by filename type:")
    print()
    cprint("TMDBimport.py --fileupdate <filename>", 'red')
    print()
    print()
    print("To update existing Database (needs to be done within 14 days of last update) type:")
    print()
    cprint("TMDBimport.py --update:", 'red')
    print()
    print()
    print("To update a single movie type:")
    print()
    cprint("TMDBimport.py --single <movieId>:", 'red')
    cprint("eg. TMDBimport.py --single 2", "blue")
    print()
    print()
    print("To check if a movie ID exists in the database:")
    print()
    cprint("TMDBimport.py --exist <movieId>:", 'red')
    cprint("eg. TMDBimport.py --exist 2", "blue")
    print()

  if len(sys.argv) == 2:

    moviesuccess = 0
    movieerrors = 0


    if sys.argv[1] == "--fileimport":
      print("Please supply a filename: --fileimport filename")

    if sys.argv[1] == "--baseimport":
      for i in range(650000):
        movie = tf.getMovie(i)
        if movie == "inserted": moviesuccess = moviesuccess + 1
        if movie == "error": movieerrors = movieerrors + 1
        cprint("\rMOVIE: ", "white", end="")
        cprint(i, "yellow", attrs=['bold'], end="")
        #TODO: http error count
        cprint("    movie successful: ", "white", end="")
        cprint(moviesuccess, "green", attrs=['bold'], end="")
        cprint("    movie error: ", "white", end="")
        cprint(movieerrors, "red", attrs=['bold'], end="")
        sys.stdout.flush()

    if sys.argv[1] == "--update":
      date = tf.getMaxDate()
      json = tf.getJSON("https://api.themoviedb.org/3/movie/changes?api_key=" + tf.api_key + "&start_date=" + str(date))

      for key, values in json.items():
        if key == "results":
          for value in values:
            if value['adult'] is not True:
              id = value['id']
              if debug: print(id)
              movie = tf.getMovie(id)                      #TODO: value must be datetime not string
              if movie == "inserted": moviesuccess = moviesuccess + 1
              if movie == "error": movieerrors = movieerrors + 1
              cprint("\rMOVIE: ", "white", end="")
              cprint(id, "yellow", attrs=['bold'], end="")
              #TODO: http error count
              cprint("    movie successful: ", "white", end="")
              cprint(moviesuccess, "green", attrs=['bold'], end="")
              cprint("    movie error: ", "white", end="")
              cprint(movieerrors, "red", attrs=['bold'], end="")
              sys.stdout.flush()

    if sys.argv[1] == "--update-cron":
      date = tf.getMaxDate()
      json = tf.getJSON("https://api.themoviedb.org/3/movie/changes?api_key=" + tf.api_key + "&start_date=" + str(date))

      for key, values in json.items():
        if key == "results":
          for value in values:
            if value['adult'] is not True:
              id = value['id']
              if debug: print(id)
              movie = tf.getMovie(id)                      #TODO: value must be datetime not string
              if movie == "inserted": moviesuccess = moviesuccess + 1
              if movie == "error": movieerrors = movieerrors + 1

      tf.createMemCopySmall(True)

    if sys.argv[1] == "--single":
      print("Please supply a movie ID")

    if sys.argv[1] == "--exist":
      print("Please supply a movie ID")

    if sys.argv[1] == "--memorycopy-small":
      tf.createMemCopySmall(None)

    if sys.argv[1] == "--memorycopy-small-cron":
      tf.createMemCopySmall(True)


  if len(sys.argv) == 3:
    moviesuccess = 0
    movieerrors = 0

    if sys.argv[1] == "--fileimport":
      if not os.path.exists(sys.argv[2]):
        print("Filename {} does not exist".format(sys.argv[2]))
        sys.exit(0)

      with open(sys.argv[2]) as f:
        for line in f:
          tmdbj = json.loads(line)
          if tmdbj["adult"] == False:
            movie = tf.getMovie(tmdbj["id"])
            if movie == "inserted": moviesuccess = moviesuccess + 1
            if movie == "error": movieerrors = movieerrors + 1
            cprint("\rMOVIE: ", "white", end="")
            cprint(tmdbj["id"], "yellow", attrs=['bold'], end="")
            #TODO: http error count
            cprint("    movie successful: ", "white", end="")
            cprint(moviesuccess, "green", attrs=['bold'], end="")
            cprint("    movie error: ", "white", end="")
            cprint(movieerrors, "red", attrs=['bold'], end="")
            sys.stdout.flush()

    if sys.argv[1] == "--fileupdate":
      if not os.path.exists(sys.argv[2]):
        print("Filename {} does not exist".format(sys.argv[2]))
        sys.exit(0)

      with open(sys.argv[2]) as f:
        for line in f:
          tmdbj = json.loads(line)
          if tmdbj["adult"] == False:
            exist = tf.getExist(tmdbj["id"])
            if exist == False:
              movie = tf.getMovie(tmdbj["id"])
              if movie == "inserted": moviesuccess = moviesuccess + 1
              if movie == "error": movieerrors = movieerrors + 1
              cprint("\rMOVIE: ", "white", end="")
              cprint(tmdbj["id"], "yellow", attrs=['bold'], end="")
              #TODO: http error count
              cprint("    movie successful: ", "white", end="")
              cprint(moviesuccess, "green", attrs=['bold'], end="")
              cprint("    movie error: ", "white", end="")
              cprint(movieerrors, "red", attrs=['bold'], end="")
              sys.stdout.flush()

    if sys.argv[1] == "--single":
      print("Importing movie " + sys.argv[2])
      tf.getMovie(sys.argv[2])

    if sys.argv[1] == "--exist":
      print("Checking if movieid exists in database")
      print(tf.getExist(sys.argv[2]))


if __name__ == "__main__": main()
