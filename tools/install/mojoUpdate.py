#!/usr/bin/python3
#coding=utf-8

import os
import sys
import glob
import gzip
import time
import wget
import random
import traceback
import urllib.request

from datetime import datetime, timedelta


debug = False

sys.path.append('.')
sys.path.append('..')

class MojoUpdate:

  basepath = os.path.dirname(os.path.abspath("."))
  #targetpath = os.getenv("HOME")+"/imdb_dumps/"
  targetpath = "./weeklyMojoDumps/"

  print(basepath)

  #init
  def __init__(self):
    if debug: print("ii: __init__")
    try:
      os.mkdir(self.targetpath)
    except:
      if debug: traceback.print_exc(file=sys.stdout)
      print("Could not create directory: " + self.targetpath)


  def downloadCurrentWeek(self):
    now = datetime.now()
    
    #we take the date 2 weeks before since mojo lags 2 weeks
    last = now - timedelta(days=14)
    year = last.year
    week = last.strftime("%V")
    print("YEAR:", year, "WEEK:", week)
    
    self.downloadBoth(year, week)
    

  def downloadSince(self, year):
    start = datetime(year, 1, 1)
    now = datetime.now()
    #we take the date 2 weeks before since mojo lags 2 weeks
    last = now - timedelta(days=14)

    while ( start < last ):
      print(start)
      time.sleep((random.randint(500,1000))/1000)
      year = start.year
      week = start.strftime("%V")
      self.downloadBoth(year, week)
      start = start + timedelta(days=7)
      

  def downloadBoth(self, year, week):
    self.downloadUs(year, week)
    self.downloadUk(year, week)


  def downloadUs(self, year, week):
    yearString = str(year)
    weekWithLeadingZero = str(week).zfill(2)
    url = "http://www.boxofficemojo.com/weekly/chart/?yr="+yearString+"&wk="+weekWithLeadingZero+"&p=.htm";
    print(url)
    self.fetch(url, yearString, weekWithLeadingZero, "us")


  def downloadUk(self, year, week):
    yearString = str(year)
    weekWithLeadingZero = str(week).zfill(2)
    url = "http://www.boxofficemojo.com/intl/uk/?yr="+yearString+"&wk="+weekWithLeadingZero+"&p=.htm";
    print(url)
    self.fetch(url, yearString, weekWithLeadingZero, "uk")


  def fetch(self, url, year, month, country):
    fileName = MojoUpdate.targetpath+"/"+year+month+"."+country+"mojo"
    
    if os.path.isfile(fileName):
      print(fileName, "already exists, skipping")
      return
    
    fetched = False
    count = 0
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

    response = None
    ##parsing response
    try:
      r = req.read()
      if debug: print(">>>",r)
      response = r.decode('latin-1')
    except:
      print()
      print("Failed 3 Times, giving up: " + url)
      raise Exception("no Data from URL")

    if debug: 
      for key, value in cont.items():
        print("CONT: Key: "+str(key)+ " Value: "+str(value))

    file = open(fileName, "w", encoding="utf-8", errors='ignore')
    file.write(response)
    file.close()

    time.sleep(.500)


#---------------------------------------#
# MAIN
# --------------------------------------#
def main():

  mu = MojoUpdate()

  if len(sys.argv) < 2:
    mu.downloadCurrentWeek()
  else:
    if sys.argv[1] == "--all":
      print("fetching all since 1980")
      mu.downloadSince(1999)

if __name__ == "__main__": main()


