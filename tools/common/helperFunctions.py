#!/usr/bin/python3
#coding=utf-8
# cython: language_level=3, boundscheck=False
import os
import re
import sys
import signal
import traceback
#import logging
import inspect
import distance
from difflib import SequenceMatcher
from termcolor import colored, cprint
from multiprocessing import Process, Manager

#add the parent directory to path
sys.path.append('.')
sys.path.append('..')

from common.logger             import LoggerSingleton as logging


debug = False

class HelperFunctions:
  arrowLeft = " \033[0;36m- \033[0;37m>\033[0;36m>\033[1;36m>\033[0m"
  arrowRight = " \033[1;36m<\033[0;36m<\033[0;37m<\033[0;36m -"

  cpus = 16

# ===================================================================================================
# get LAST year in release, last because the movie 2012.2009.1080p.BluRay.X264-LAMERS has two years
# ===================================================================================================    
  def getYear(string):
    result = re.findall("(20\d\d|19\d\d)", string)
    length = len(result)
    try:
      year = result[length-1]
    except:
      year = None
    return year


# ===================================================================================================
# get Season/Episode S1E1, S1E01, S01E10, S100E100
# ===================================================================================================    
  def getSeasonEpisode(string):
    result = re.findall("S\d{1,3}E\d{1,3}|E\d\d", string, flags=re.IGNORECASE)

    try:
      seasonEpisode = result[0]
    except:
      seasonEpisode = None
    return seasonEpisode


# ===================================================================================================
# some shows have a date in the release, getting this date
# ===================================================================================================    
  def getDate(string):
    result = re.findall("\d\d\d\d[._-]\d\d[._-]\d\d", string, flags=re.IGNORECASE)

    try:
      episodeDate = result[0]
    except:
      episodeDate = None
      
    return episodeDate

# ===================================================================================================
# prepare the movie string for sql search that gives something like M%a%t%r%i%s, % is a wild char
# because there are some relases with additional characters like noted below
# ===================================================================================================
  def prepareStringForSqlSearch(string):
    string = string.lower()
    string = string.replace("and", "%")
    string = string.replace(" ", "%")
    string = string.replace(".", "%")
    string = string.replace("_", "%")
    string = string.replace("-", "%")
    string = string.replace("'", "%")

    letters = ""

    #inserting % between each letter for queries like "Journey's End" that is in release name "Journes.End"
    for letter in string:
      letters = letters + letter + "%"

    return letters
    

# ===================================================================================================
# Compare two Strings a,b and return similarity of the two strings
# ===================================================================================================
  def similar(a, b):
    return SequenceMatcher(None, a, b).ratio()
    #return distance.levenshtein(a, b)
    #return distance.nlevenshtein(a, b, method=2)
    #return distance.jaccard(a, b)


# ===================================================================================================
# find best matching entry from list (results)
# ===================================================================================================
  def findBest(rows, titleColumnNo, title, yearColumnNo, year):
    mpList = Manager().list()                                                   #multi processor list

    processes = []


    for cpu in range(HelperFunctions.cpus):
      proc = Process(target=HelperFunctions.searchSubprocess, args=(cpu, mpList, rows, titleColumnNo, title, yearColumnNo, year))
      processes.append(proc)
      proc.start()
    for p in processes:
      p.join()

    if not mpList:
      if debug:
        print("NOT FOUND")

    maxValue=0
    result = None

    for entry in mpList:
      if entry[0]>maxValue:
        maxValue = entry[0]
        result = entry
    return result


  def searchSubprocess(cpu, mpList, rows, titleColumnNo, title, yearColumnNo, year):
    bestSimilarity = 0
    similarity=0
    bonus=0
    bestRow = None
    bestDeltaYear = None
    
    if rows == None: return
    totalRows = len(rows)

    for row in rows[int((cpu/HelperFunctions.cpus)*totalRows) : int((cpu+1)/HelperFunctions.cpus*totalRows)]:        #TODO: rounding errors?

      bonus = 0                                                                 #give additional points if the year is correct
      deltaYear = None
      if year == None:                                                          #if no year specified dont calculate bonus
        similarity = HelperFunctions.similar(title, row[titleColumnNo])
      else:
        try:
          deltaYear = abs( int(year) - int(row[yearColumnNo]) )
          if deltaYear == 0:
            bonus = 0.3
          else:
            bonus = (100-deltaYear)/1000                                        #a maximum of 0.1 additional similarity ponts, the one with the neares year will be selected

        except:
          if debug: traceback.print_exc(file=sys.stdout)
          bonus = 0

        similarity = HelperFunctions.similar(title.lower(), row[titleColumnNo].lower()) + bonus

      if similarity > bestSimilarity:
        bestSimilarity = similarity
        bestRow = row
        bestDeltaYear = deltaYear

    if bestRow == None:
      logging.info("No Matching Movie Found")
      return [ None, None, None ]

    logging.info(inspect.stack()[0][3] + "() (SimilarityChecker 1.1 = 100%): " + str(bestSimilarity) + " Title: " + bestRow[titleColumnNo] + " -- " + bestRow[titleColumnNo])

#    return [ bestSimilarity, bestRow, bestDeltaYear ]
    mpList.append([bestSimilarity, bestRow, bestDeltaYear])



    
# ===================================================================================================
# Print Formated user list with site and rules from an 3D array
# ===================================================================================================
  def printLog(rls):
    result = ""
    if rls.fullLog:
      for line in logging.getInstance()._logdata.split("\n"):
        if "ERROR" in line:
          result += colored(line, "white", "on_red") + "\n"
        else:
          result += line + "\n"
    else:
      for line in logging.getInstance()._logdata.split("\n"):
        if "ERROR" in line:
          result += colored(line, "white", "on_red") + "\n"
    if debug: print("FLUSHING LOG")
    logging.getInstance().flush()

    return result


# MAIN
# mostly for testing this module at the console
# ------------------------------------------------
def main():


  if len(sys.argv)<2:
    print()
    print()
    cprint("U N I T  T E S T s  FOR helperFunctions:", 'red', 'on_white')  
    print()

    input = "asdlknaq1990sdnlgflkn1999asdfasdf2000asdfasdf"
    print(input)
    result = HelperFunctions.getYear(input)
    
    if result == "2000":
      cprint(result, 'white', 'on_green')    
    else:
      cprint(result, 'white', 'on_red', end="")
      print(" should be: ", end="")
      cprint("2000", 'white', 'on_blue')

    print("--------------------------------------------------------------------------------")

    input = "asdlknaqS01E02sdnlgflkn1999asdfasdf2000asdfasdf"
    print(input)
    result = HelperFunctions.getSeasonEpisode(input)
    
    if result == "S01E02":
      cprint(result, 'white', 'on_green')    
    else:
      cprint(result, 'white', 'on_red', end="")
      print(" should be: ", end="")
      cprint("S01E02", 'white', 'on_blue')

    print("--------------------------------------------------------------------------------")

    print("Similarity betwwen TESt1 and TEST2")
    if HelperFunctions.similar("TESt1", "TEST2") == 0.6:
      cprint("0.6", 'white', 'on_green')    
    else:
      cprint(result, 'white', 'on_red', end="")
      print(" should be: ", end="")
      #cprint(releaseName[1], 'white', 'on_blue')

    print("--------------------------------------------------------------------------------")

    input = "The Matrix AND Revolutions"
    print(input)
    result = HelperFunctions.prepareStringForSqlSearch(input)

    if result == "t%h%e%%%m%a%t%r%i%x%%%%%%%r%e%v%o%l%u%t%i%o%n%s%":
      cprint(result, 'white', 'on_green')    
    else:
      cprint(result, 'white', 'on_red', end="")
      print(" should be: ", end="")
      cprint("t%h%e%%%m%a%t%r%i%x%%%%%%%r%e%v%o%l%u%t%i%o%n%s%", 'white', 'on_blue')



    rows = [["ttxyz00000", "The Matrix", 1999],
            ["ttxyz00001", "The Matrixx", 1998],
            ["ttxyz00001", "The Matrixx", 1999]]

    print("--------------------------------------------------------------------------------")
    print("Finding best row from 3 entries")
    result = HelperFunctions.findBest(rows, 1, "The Matrix", 2, 1999)

    if str(result) == "[1.3, ['ttxyz00000', 'The Matrix', 1999], 0]":
      cprint(result, 'white', 'on_green')    
    else:
      cprint(result, 'white', 'on_red', end="")
      print(" should be: ", end="")
      cprint("[1.3, ['ttxyz00000', 'The Matrix', 1999], 0]", 'white', 'on_blue')

    print("--------------------------------------------------------------------------------")







if __name__ == "__main__": main()



