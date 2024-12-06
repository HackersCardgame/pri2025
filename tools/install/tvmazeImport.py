#!/usr/bin/python3
#coding=utf-8
# cython: language_level=3, boundscheck=False
import os
import sys
import urllib.request
import json
import time
import signal
import datetime
from datetime import datetime, timedelta

import traceback
from datetime import datetime
from termcolor import colored, cprint

debug = False

sys.path.append('.')
sys.path.append('..')

from common.dbConnector        import DBConnector


class TvmazeFetcher:

  table = "tvmaze"
  table_episodes = "episodes"   #TODO:  => http://api.tvmaze.com/shows/ $VARIABLE /episodes  => for each row: insert into episodes (TVMAZEID, season, episode, airdate) VALUES (variable variable


  def __init__(self):
    signal.signal(signal.SIGINT, self.signal_handler)
    self.dbConn = DBConnector.getInstance()
    print('to Interrupt press Ctrl+C')


  def signal_handler(self, sig, frame):
    print()
    print('You pressed Ctrl+C! Commiting changes')
    self.dbConn.mariadb_connection.commit()
    self.dbConn.mariadb_connection.close()
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
        time.sleep(.500)
      except urllib.error.URLError as e:
      # Not an HTTP-specific error (e.g. connection refused)
        print('\r EE: URLError: {}'.format(e.reason) + " (trying another " + str(3-count) + " times)", end="")
        if debug: traceback.print_exc(file=sys.stdout)
        time.sleep(.500)
      else:
      # 200
        if debug: print('ii: fetch good, 200')
      if debug: print(req)

    ##parsing response
    try:
      r = req.read()
      if debug: print(">>>",r)
      cont = json.loads(r.decode('utf-8'))
    except:
      print()
      print("Failed 3 Times, giving up: " + url)
      raise Exception("no Data from URL")

    if debug: 
      for key, value in cont.items():
        print("CONT: Key: "+str(key)+ " Value: "+str(value))

    time.sleep(.500)

    return cont
    
    
# ===================================================================================================
# get the values from tvmaze api by showId
# ===================================================================================================
  def parseJSON(self, json, lookupValues):
    if debug: print("entering parseJSON()")
    results = []
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

        if type(value[0]) == list:
          if not len(value[0]) == 0:
            temp = " ".join(value[0])
            value[0] = temp
                                 #TODO: implement by sql, select max(season) from episodes where showId=blah

    for value in lookupValues:
      results.append(value[0])

    return results


# ===================================================================================================
# get a shop by id and put it into the database
# ===================================================================================================
  def getBoth(self, showId, timestamp):
    if debug: print("entering getBoth()")
    show = ""
    episodes = ""
    if self.newer(showId, timestamp):
      #print("newer")
      show = self.getShow(showId)
      episodes = self.getEpisodes(showId)
    return show, episodes

# ===================================================================================================
# get a shop by id and put it into the database
# ===================================================================================================
  def getShow(self, showId):
    if debug: print("entering getShow()")
    try:
      json = self.getJSON("http://api.tvmaze.com/shows/"+str(showId))
    except:
      return "error"
      

    #this values structure defines the return vealue, each will be initialized with eg "" and will get the values in the first subelement
    values = [["", "id"],["", "name"], ["", "type"], ["", "language"], ["", "genres"], ["", "status"], ["", "premiered"], ["", "network", "name"], ["", "webChannel", "name"], ["", "network", "country", "code"], ["", "webChannel", "country", "code"], ["", "updated"] ]
    try:
      self.insertShow(showId, self.parseJSON(json, values))
      return ("inserted")
    except:
      print()
      print("Show IMPORT ERROR: " + str(showId))
      if debug: traceback.print_exc(file=sys.stdout)
      return "error"



# ===================================================================================================
# get all episodes
# ===================================================================================================
  def getEpisodes(self, showId):
    json = None
    if debug: print("entering getEpisode()")
    try:
      url = "http://api.tvmaze.com/shows/"+str(showId)+"/episodes"
      if debug: print(url)
      json = self.getJSON(url)
    except:
      if debug: print(">>>", json)
      if debug: traceback.print_exc(file=sys.stdout)
      return "error"

    #this values structure defines the return vealue, each will be initialized with eg "" and will get the values in the first subelement
    values = [ ["",  "season"], ["", "number"], ["", "airdate"] ]
    results = []
    
    error = False
    
    for show in json:
      try:
        self.insertEpisode(showId, self.parseJSON(show, values))
      except:
        print()
        print("EPISODE IMPORT ERROR: " + str(show))
        error = True
        if debug: traceback.print_exc(file=sys.stdout)

    if error:
      return "error"
    else:
      return "inserted"
   
        
# ===================================================================================================
# insert single episode
# ===================================================================================================
  def insertEpisode(self, showId, values):
    if debug: print("entering insertEpisode()")
    season, episode, airdate = values

    if airdate == "":
      raise Exception("noAirdate")

    sql = "REPLACE INTO " + self.table_episodes + " (showId, season, episode, airdate) VALUES (" + repr(str(showId)) + ", " + repr(str(season)) + ", " + repr(str(episode)) + ", " + repr(str(airdate)) + ");"
        
    try:
      self.dbConn.modify(sql)
    except:
      print()
      print(sql)
      raise Exception("showInsertFailed")

# ===================================================================================================
# insert single episode
# ===================================================================================================
  def insertShow(self, showId, values):
    if debug: print("entering insertShow()")
    id, title, type, language, genre, status, premiere, network, networkWeb, countryCode, countryCodeWeb, updated  = values
    
    if countryCode == None:
      countryCode = countryCodeWeb
    
    if premiere == None: premiere = datetime.utcfromtimestamp(0)
       
    sql = "REPLACE INTO " + self.table + " (id, title, type, language, genre, status, premiere, " + \
                                           "network, networkWeb, countryCode, updated) VALUES ('" + \
                                            str(id)               + "', "+ repr(str(title))       + ", " + repr(str(type))           + ", " + repr(str(language))   +  ", " + \
                                            repr(str(genre))      + ", " + repr(str(status))      + ", " + repr(str(premiere))       + ", " + repr(str(network)) + ", " + \
                                            repr(str(networkWeb)) + ", " + repr(str(countryCode)) + ", " +\
                                            repr(str(datetime.fromtimestamp(updated).strftime('%Y-%m-%d %H:%M:%S'))) + ");"

        
    try:
      self.dbConn.modify(sql)
    except:
      print()
      print(sql)
      raise Exception("showInsertFailed")



# ===================================================================================================
# check if an entry already exists in Database
# ===================================================================================================
  def newer(self, showId, timestamp):
    if debug: print("entering newer()")
    if timestamp == None:
      print("No Timestamp")
      return True

    sql = "SELECT updated FROM " + self.table + " WHERE id = '"+str(showId)+"';"

    sqlDateInDictionary = None

    try:
      sqlDateInDictionary = self.dbConn.fetchall(sql)
    except:
      traceback.print_exc(file=sys.stdout)
      if debug: print("Error:", sql)
      return True

    #print(sqlDateInDictionary)

    if sqlDateInDictionary == None:
      return True
    
    for sqlDate in sqlDateInDictionary[0]:
#      unixtime = time.mktime(sqlDate.timetuple())
      if timestamp > sqlDate:
        if debug: print(timestamp, sqlDate)
        return True
      else:
        return False



# MAIN
# -----------------------------------------------------------------------
def main():

  tf = TvmazeFetcher()
  
  if len(sys.argv)<2:
    cprint("IMPRORT s  FOR TvmazeFetcher:", 'red', 'on_white')  
    print()
    print("To make the baseimport type (needs 24h, dont do this on a productive System):")
    print()
    cprint("TVMAZEimport.py --baseimport:", 'red')
    print()
    print()
    print("To update existing Database type:")
    print()
    cprint("TVMAZEimport.py --update:", 'red')
    print()
    print()
    print("To update a single show type:")
    print()
    cprint("TVMAZEimport.py --single <showNumber>:", 'red')
    cprint("eg. TVMAZEimport.py --single 66", "blue")
    print("(to update Big Bang Theory)")
    print()

  if len(sys.argv)>1:
    
    showsuccess = 0
    showerrors = 0
    episodesuccess = 0
    episodeerrors = 0
    
    
    if sys.argv[1] == "--baseimport":
      for i in range(40000):
        show, episodes = tf.getBoth(i, None)
        if show == "inserted": showsuccess = showsuccess + 1
        if show == "error": showerrors = showerrors + 1
        if episodes == "inserted": episodesuccess = episodesuccess + 1
        if episodes == "error": episodeerrors = episodeerrors + 1
        cprint("\rSHOW: ", "white", end="")
        cprint(i, "yellow", attrs=['bold'], end="")
        #TODO: http error count
        cprint("    show: ", "white", end="")
        cprint(showsuccess, "green", attrs=['bold'], end="")
        cprint("    show error: ", "white", end="")
        cprint(showerrors, "red", attrs=['bold'], end="")
        cprint("    episodes: ", "white", end="")
        cprint(episodesuccess, "green", attrs=['bold'], end="")
        cprint("    episodes: ", "white", end="")
        cprint(episodeerrors, "red", attrs=['bold'], end="")
        sys.stdout.flush()
    
    if sys.argv[1] == "--update":
      json = tf.getJSON("http://api.tvmaze.com/updates/shows")
      for key, value in json.items():
        ts = int(value)
        dt = datetime.utcfromtimestamp(ts)
        if debug: print(key, value, ts, dt)
        show, episodes = tf.getBoth(key, dt)                      #TODO: value must be datetime not string
        if show == "inserted": showsuccess = showsuccess + 1
        if show == "error": showerrors = showerrors + 1
        if episodes == "inserted": episodesuccess = episodesuccess + 1
        if episodes == "error": episodeerrors = episodeerrors + 1
        cprint("\rSHOW: ", "white", end="")
        cprint(key, "yellow", attrs=['bold'], end="")
        #TODO: http error count
        cprint("    show: ", "white", end="")
        cprint(showsuccess, "green", attrs=['bold'], end="")
        cprint("    show: ", "white", end="")
        cprint(showerrors, "red", attrs=['bold'], end="")
        cprint("    episodes: ", "white", end="")
        cprint(episodesuccess, "green", attrs=['bold'], end="")
        cprint("    episodes: ", "white", end="")
        cprint(episodeerrors, "red", attrs=['bold'], end="")
        sys.stdout.flush()
    print()
    if sys.argv[1] == "--single":
      print("Importing show " + sys.argv[2])
      tf.getBoth(sys.argv[2], None)    
    
      

if __name__ == "__main__": main()




