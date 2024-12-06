#!/usr/bin/python3
#coding=utf-8
# cython: language_level=3, boundscheck=False
import os
import re
import sys
import signal
import glob
import traceback
import logging
import datetime
from datetime import datetime, timedelta
from termcolor import colored, cprint


#TODO: way to debug -> eg logging to mem with parameter
#python3 MOJOimport.py weeklyMojoDumps/201504.usmojo
#print those who are wrong to add them manually => maybe show in irc

from lxml import etree

sys.path.append('.')
sys.path.append('..')

from common.dbConnector              import DBConnector
from common.helperFunctions          import HelperFunctions
from processRelease.lookupImdb       import LookupImdb

debug = True

class mojoFetcher:

  noScreens=0; notFound=0; couldNotUpdate=0; getDataTreeError=0; parseError=0; success=0; total=0
  
  akas = "akas"
  basics = "basics"
  table = "mojo"
  
  basepath = os.getcwd() + "/"
  
  weeklyMojoDumps = basepath + "weeklyMojoDumps/"

# ===================================================================================================
# initialize sql connection
# ===================================================================================================
  def __init__(self):
    self.dbConn = DBConnector.getInstance()
    self.il = LookupImdb()
    sql = "select count(*) from "+self.basics+";"
    result = None
    result = self.dbConn.fetchall(sql)
    if debug: print(result)
    if result == None:
      print()
      print(colored("EE: IMDB-Memory-DBs non existent, run memorycopy:", "white", "on_red"), "run", colored("python3 imdbImport.py --memorycopy", "green"))
      raise Exception("NO MEMORY DB")
    if int(result[0][0]) < 1000:
      print()
      print(colored("EE: IMDB-Memory-DBs empty, run memorycopy:", "white", "red"), "run", colored("python3 imdbImport.py --memorycopy", "green"))
      raise Exception("EMPTY MEMORY DB")
    else:
      print("Using existing Memory DB")
    signal.signal(signal.SIGINT, self.signal_handler)
    print('to Interrupt press Ctrl+C')

  def signal_handler(self, sig, frame):
    print()
    print('You pressed Ctrl+C! Aborting...')
    self.dbConn.mariadb_connection.commit()
    self.dbConn.mariadb_connection.close()
    sys.exit(0)


  def getCurrentWeek(self, country):
    now = datetime.now()
    
    #we take the date 2 weeks before since mojo lags 2 weeks
    last = now - timedelta(days=14)
    year = last.year
    week = last.strftime("%V")
    print("YEAR:", year, "WEEK:", week)
    
    #self.downloadBoth(year, week)
    fileName = self.weeklyMojoDumps+str(year)+str(week)+"."+country+"mojo"
    return fileName

# ===================================================================================================
# parse one single week
# ===================================================================================================
  def parseWeek(self):
    if debug: print("parseWeek()", "basedir:", self.weeklyMojoDumps)
    us = self.getCurrentWeek("us")
    uk = self.getCurrentWeek("uk")

    print("parsing:", us)
    try:
      self.parseFile(us, "usScreens")
    except:
      if debug: traceback.print_exc(file=sys.stdout)
    
    print("parsing:", uk)
    try:
      self.parseFile(us, "ukScreens")
    except:
      if debug: traceback.print_exc(file=sys.stdout)
    

# ===================================================================================================
# parse the directory where the weekly dumps are stored
# ===================================================================================================
  def parseDirectory(self, filename):
    if debug: print("parseDirectory()", "basedir:", self.weeklyMojoDumps)
    if filename == None:
      dirUS = glob.glob(self.weeklyMojoDumps+'*.usmojo')
      dirUK = glob.glob(self.weeklyMojoDumps+'*.ukmojo')
    else:
      filename = os.path.basename(filename)
      date = os.path.splitext(filename)[0]
      dirUS = [ self.weeklyMojoDumps + date+".usmojo" ]
      dirUK = [ self.weeklyMojoDumps + date+".ukmojo" ]
      


#parsing one file with type
    for filename in dirUS: #make also dirUS
      print()
      print(filename)
      try:
        self.parseFile(filename, "usScreens")
      except:
        if debug: traceback.print_exc(file=sys.stdout)
        self.parseError = self.parseError + 1
        
      
    for filename in dirUK: #make also dirUS
      print()
      print(filename)
      try:
        self.parseFile(filename, "ukScreens")
      except:
        if debug: traceback.print_exc(file=sys.stdout)
        self.parseError = self.parseError + 1

# parse one file with given filename and given country
  def parseFile(self, filename, country):

    title = ""
    screens = 0
    tconst = ""
    
    
    try:
      tree = self.getDataTree(filename)
    except:
      if debug: print("could not get Datatree")
      logging.error("could not get Datatree: " + filename)
      self.getDataTreeError = self.getDataTreeError + 1

    try:
      movies = self.parseDataTree(tree)
    except:
      if debug: print("could not parse Datatree: " + filename)
      logging.error("could not parse Datatree: " + filename)
      self.parseError = self.parseError + 1

    for movie in movies:
      self.total = self.total + 1
      if self.total % 20 == 0:
        self.counter(self.total, self.getDataTreeError, self.parseError, self.noScreens, self.notFound, self.couldNotUpdate, self.success)

      try:
        title = re.split(".\((.+)\)", movie[0])[0] # remove the bracets (additional info)
        title = re.split(" 3D", title)[0] # remove the bracets (additional info)

        screens = int(movie[1].replace(",",""))
        if debug: print(title + ": "+str(screens))
      except:
        screens = 0
        self.noScreens = self.noScreens + 1
        if debug: traceback.print_exc(file=sys.stdout)
        continue

      try:
        head, tail = os.path.split(filename)
        year = tail[0:4]
        if debug: print("--> "+title+ " " + year + " " + str(screens))
      except:
        print("EE: could not get year")
        
      try:
        if debug: print("====>"+title+" -- " + year)
        [ tconst, imdbPrimaryTitle, imdbOriginalTitle, imdbYear, imdbRating, imdbRatings, imdbGenre, tmdbLanguage ] = self.il.getMovieFromBasics(title, year, False)
        if debug: print(tconst)  
      except:
        if debug: print()
        if debug: print("EE: Could not find Movie in IMDB: " + title)
        logging.error("Could not find Movie in IMDB: " + title)
        self.notFound = self.notFound + 1
        if debug: traceback.print_exc(file=sys.stdout)
        continue
        
      try:
        self.updateMojo(tconst, country, screens, title)
      except:
        self.couldNotUpdate = self.couldNotUpdate + 1
        logging.error("could not update mojo db: " + title)
        print(tconst, country, screens, title)
        if debug: traceback.print_exc(file=sys.stdout)
        continue
        
      self.success = self.success + 1
          

  def counter(self, total, getDataTreeError, parseError, noScreens, notFound, couldNotUpdate, success):
    print( "Processed: "         + colored(str(total),"blue") +
           " parseError: "       + colored(str(parseError), "red") +     
           " noScreens: "        + colored(str(noScreens), "yellow") +
           " notFound: "         + colored(str(notFound), "red") + 
           " Updated: "          + colored(str(success), "green") + "         " , end="\r", flush=True)



# ===================================================================================================
# get the table from the html-file
# ===================================================================================================
  def updateMojo(self, tconst, row, screens, mojoTitle):
    sql = "SELECT " + row + " from " + self.table + " where tconst like " + repr(tconst) + ";"
  
    results = self.dbConn.fetchall(sql)

    if len(results) > 0:
      if debug: print(str(results[0][0]) +" r: "+row+"s: "+str(screens))
      if results[0][0] == None or int(results[0][0]) < int(screens):
        if debug: print("ii: UPDATING: " + tconst, end="",)
        self.update(tconst, row, screens)
    else:
      if debug: print("ii: INSERTING: " + tconst, end="",)
      self.insert(tconst, row, screens, mojoTitle)

      

# ===================================================================================================
# update mojo db when entry already exists
# ===================================================================================================
  def update(self, tconst, row, screens):
    sql = "UPDATE " + self.table + " SET " + row + "=" + repr(screens) + " WHERE tconst=" + repr(tconst) + ";"
  
    if debug: print(sql)
  
    try:
      self.dbConn.modify(sql)
    except:
      logging.error("Could not insert line to config Database")
      if debug: traceback.print_exc(file=sys.stdout)


# ===================================================================================================
# insert into the mojo db when not yet in db
# ===================================================================================================
  def insert(self, tconst, row, screens, mojoTitle):
    sql = "INSERT INTO " + self.table + " (tconst, " + row + ", mojoTitle) VALUES (" + repr(tconst) + ", " + repr(screens) + ", " + repr(mojoTitle) + ");"

    if debug: print(sql)
  
    try:
      self.dbConn.modify(sql)
    except:
      logging.error("Could not insert line to config Database")
      if debug: traceback.print_exc(file=sys.stdout)


# ===================================================================================================
# get the table from the html-file
# ===================================================================================================
  def getDataTree(self, filename):
    with open(filename, encoding="latin-1") as myfile:
      result = []
      
      resultLine = False
      for line in myfile:
      
      #FIND THE RIGHT TABLE
        if (not resultLine):
          tableStart = '<table border="0" cellspacing="1" cellpadding='
          if re.search(tableStart, line):
            line = tableStart + line.split(tableStart)[1]
            resultLine = True

      #AND APPEND ALL LINES FROM THE TABLE TO A STRING UNTIL </table>
        if resultLine:
          result.append(line)
          if re.search('</table>', line):
            resultLine = False

      result = "".join(str(x) for x in result)
      return result      


  def parseDataTree(self, tree):
    #We parse the part of the file we cut out with an xml parser (etree)
    tree = etree.HTML(tree).findall("body/table/*")
    rows = iter(tree)
    result = []
    rowCount = 0
    
    for row in rows:
      rowCount+=1
      if rowCount==1: continue
      i = 0;
      title = None
      screens = None

      for col in row:
        i += 1;

        if i==3: #MOVIE TITLE, we need to do this because it has some additional tags like font and so
          for child in iter(col[0][0]):
            title = child.text

        if i==7: #MOVIE SCREENS
          screens = str(col[0].text).replace(",","")

        if i==8:
          result.append([title, screens])

    return result
    


# MAIN
# to import all files in the directory ./weeklyMojoDumps/*
# -----------------------------------------------------------------------
def main():

  mf = None;
  try:
    mf = mojoFetcher()
  except:
    if debug: traceback.print_exc(file=sys.stdout)
    print("ERROR")
    exit(1)

  if len(sys.argv) < 2:
    mf.parseWeek()
  else:
    if sys.argv[1] == "--all":
      print("fetching all files from directory " + weeklyMojoDumps + " this will take +24h (imdb memorycopy must be updated before!!!)" )
      mf.parseDirectory(None)
    else:
      mf.parseDirectory(sys.argv[1])


if __name__ == "__main__": main()


