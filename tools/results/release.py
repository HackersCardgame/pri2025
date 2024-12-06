#!/usr/bin/python3
#coding=utf-8
# cython: language_level=3, boundscheck=False
import os
import re
import sys
import cmd
import glob
import signal
import socket
import inspect
import distance
import requests
import datetime
import traceback
#import logging

debug = False

#add the parent directory to path
sys.path.append('.')
sys.path.append('..')

from collections             import OrderedDict
from termcolor               import colored, cprint 
from difflib                 import SequenceMatcher
from datetime                import datetime, timedelta, date



#serializedData.py


class Release:

  version = "3.5.0"
  
  arrowLeft = " \033[0;36m- \033[0;37m>\033[0;36m>\033[1;36m>\033[0m "
  arrowRight = " \033[1;36m<\033[0;36m<\033[0;37m<\033[0;36m -"
  
  coloredLogo = """
\033[1;36m                      _ __ ____
 _  __ ___ ______________     /______       __ ____________ _________ ___ __  _
\033[0;36m       _ __ _______     /    / _ __ /_________ _           /    ___ ___ _
\033[0;36m             /  __/    /    /    __________    /   /_____ /    ___ _
\033[90m            /  _______/           /    ___/   /______    /    /_____ _
\033[90m           /      : /     .      /           /      /   /
\033[1;37m        __/_______|______/      /____________\\          \\________    /
"""+arrowLeft+"""\033[0m ==/===[PRI-"""+version+"""]/_______\\+==[pHASE.rELEASE.iNTERFACE]+===\\__/===="""+arrowRight+"""\033[0m
""";

  def __init__(self, release):

    self.tabspace = 19                                                          #width of first output colomn
    
    self.login = False
    self.fullLog = False
    self.onlyUser = None
    self.addDupe = True
    
    self.generalBlacklists = [ None, None, None ]
    
    self.time = str(datetime.now()).split(".")[0]

    self.releaseName = release       #the initial Release Name
    self.content = None              #the content extracted from release name
    self.substitutedContent = None   #the content extracted from release name    
    self.releaseInfo = None          #everything but the name
    self.group = ""                #the Year extracted from release name
    self.section = None              #the Section guessed with sectionGuesser
    self.releaseYear = None          #the Year extracted from release name
    self.featuresStripped = None     #features only after separator
    self.featuresFull = None         #all features
    self.features = {}               #selected features

    self.imdbTconst = None           #the tconst id from IMDB Basic / Akas Table
    self.imdbPrimaryTitle = None     #the Primary  Title from IMDB Basic Table
    self.imdbOriginalTitle = None    #the Original Title from IDMB Basic Table
    self.imdbYear = None             #the movie Year concerning IMDB
    self.imdbRating = None           #the rating from imdb
    self.imdbRatings = None          #How many people voted
    self.imdbGenre = None
    self.tmdbLanguage = None         #originalLanguage from TMDB
    
    self.mojoUsScreens = None        #the max number of cinemas running the movie at the same time in US
    self.mojoUkScreens = None        #the max number of cinemas running the movie at the same time in UK

    self.episode = None              #SEASON:EPISODE, eg. S01E06
    self.season = None
            
    self.tvmazeID = None             #the show ID from TVMAZE
    self.tvmazeTitle = None          #the title from TVMAZE
    self.tvmazeYear = None
    self.tvmazePremiere = None
    self.tvmazeType = None
    self.tvmazeLanguage = None
    self.tvmazeGenre = None
    self.tvmazeStatus = None
    self.tvmazeCountry = None
    self.tvmazeCurerntSeason = None
    self.tvmazeAirdate = None
    self.tvmazeNetwork = None
    self.tvmazeNetworkWeb = None
    self.tvmazeAirdateGiven = None
    
    self.dupes = []
    
    self.sites = []
    self.siteResults = {}
    self.users = []
    self.userResults = {}

    self.benchmark = None
    
  def printLong(self, user):
    result = ""
    if self.features["SECTION"] == "*":
      result += self.createLine("release", str(self.releaseName))
      result += self.createLine("warning", "section could not be found " + self.features["SECTION"], "red")
    else:
      result += self.coloredLogo
      result += self.createLine("release", self.releaseName)
      result += self.createLine("content", self.content)
      result += self.createLine("time", self.time)
      result += self.createLine("dupeType", self.features["DUPE"])
      if not self.dupes == []:
        for dupe in self.dupes:
          result += self.createLine("dupe", dupe)
      result += self.createLine("section", self.section)
      #if "TV" in self.section.upper():
      

      if self.features["SECTION"].upper() == "DAILYSHOWS":
        result += self.createLine("Episode Date", self.episode, "red")      
      
      
      if self.features["SECTION"].upper() == "TV":
        if self.tvmazeID == None:
          result += self.createLine("tv-info", "tvmaze info not found", "red")
        else:
          try:
            #TODO: here the problem was that tvmazeID is int => str()
            result += self.createLine("tv-info title", self.tvmazeTitle + " (id: " + str(self.tvmazeID)           + ")")
            result += self.createLine("tv-info premiere", str(self.tvmazePremiere))
            result += self.createLine("tv-info airdate ep", str(self.tvmazeAirdateGiven))
            result += self.createLine("tv-info c. season", str(self.tvmazeCurerntSeason))
            result += self.createLine("tv-info status", self.tvmazeStatus)
            result += self.createLine("tv-info language", self.tvmazeLanguage + " (country: " + self.tvmazeCountry + ")")
            result += self.createLine("tv-info genre", self.tvmazeGenre)
            result += self.createLine("tv-info type", self.tvmazeType)
            if not self.tvmazeNetwork == "None": result += self.createLine("tv-info channel", self.tvmazeNetwork)
            if not self.tvmazeNetworkWeb == "None": result += self.createLine("tv-info channel", self.tvmazeNetworkWeb) #TODO tvmazeNetwork or tvmazeNetworkWeb
          except:
            result += self.createLine("warning", "tvmaze info not found", "red")
            #TODO: to get infomration about the bug you can add the following line to the code so you would get detailed info
            if debug: traceback.print_exc(file=sys.stdout)
#        else:
#          self.printLine("warning", "tvmaze id not found", "red")

      if self.features["SECTION"].upper() == "MOVIES":
        if self.imdbTconst == None:
          result += self.createLine("imdb", "imdb info not found", "red")
        else:
          try:
            result += self.createLine("imdb-info title", self.imdbPrimaryTitle + " " + str(self.imdbYear) + " (id: " + self.imdbTconst + ")") #TODO only show aka if there is one
            if self.imdbPrimaryTitle != self.imdbOriginalTitle: result += self.createLine("imdb-info aka", self.imdbOriginalTitle)
            result += self.createLine("imdb-info r/v", str(self.imdbRating) + " - " + str(self.imdbVotes))
            result += self.createLine("imdb-info us/uk", str(self.mojoUsScreens) + " - " + str(self.mojoUkScreens))
            result += self.createLine("imdb-info language", self.tmdbLanguage)
            result += self.createLine("imdb-info genres", self.imdbGenre)
          except:
            result += self.createLine("warning", "imdb info not found", "red")
            if debug: traceback.print_exc(file=sys.stdout)

      result += self.createLine("General Blacklists", self.generalBlacklists) #TODO: Bor1s, you cant use banned, because we dont have banned
      result += self.createLine("parsed users", self.users)
      if not user == None:
        result += self.listSitesWithRules(user)
      else:
        for u in self.users:
          result += self.listSitesWithRules(u)
      result += self.createLine("benchmark", self.benchmark)

    return result


  def listSitesWithRules(self, user):
    result = self.createLine("all Sites", self.sortSites(self.userResults[user].allSites))
    result += self.createLine("sites", self.sortSites(self.userResults[user].allowed))   #TODO: not multiple times
    result += self.createLine("pre", self.sortSites(self.userResults[user].pre))
    result += self.createLine("skipped", self.sortSites(self.userResults[user].skipped))
    result += self.createLine("User Blacklists", self.userResults[user].userBlacklists) #TODO: Bor1s, you cant use banned, because we dont have banned anymore, we have different blacklists
    result += self.arrowLeft + colored('{1:<{0}}'.format(self.tabspace, user), "blue") + ": "
    for site in self.userResults[user].allowed:
      result += colored(site, "green") + "/" + colored(self.siteResults[site].matchedRule, "cyan") + " "
    result += "\n"
    return result



   
  def printShort(self):
    result = ""
    result += colored("Release:", "white")
    result += colored(self.content, "green")
    result += colored(self.releaseInfo +" ", "blue")
    result += colored(self.section, "cyan")
    result += colored(" year:" + str(self.releaseYear), "yellow")
    result += colored(" dupeType" + self.features["DUPE"], "red") + "\n"

    #if "TV" in self.section.upper():
    if self.featuresStripped["SECTION"].upper() == "TV":
      try:
        result += colored(" tvmazeID:", "white")
        result += colored(str(self.tvmazeID), "green")
        result += colored(" title:", "white")
        result += colored(str(self.tvmazeTitle), "green")
        result += colored(" Year:", "white")
        result += colored(str(self.tvmazeYear), "green")
        result += colored(" EPiSODE:", "white")
        result += colored(str(self.episode), "green")
        result += colored(" Type:", "white")
        result += colored(str(self.tvmazeType), "green")
        result += colored(" LaNgUagE:", "white")
        result += colored(str(self.tvmazeLanguage), "green")
        result += colored(" GenRe:", "white")
        result += colored(str(self.tvmazeGenre), "green")
        result += colored(" StaTus:", "white")
        result += colored(" Network:", "white")
        result += colored(str(self.tvmazeNetwork), "green")
        result += colored(" NetworkWeb:", "white")
        result += colored(str(self.tvmazeNetworkWeb), "green")
        result += colored(" Premier:", "white")
        result += colored(str(self.tvmazePremiere), "green")
        result += colored(str(self.tvmazeStatus), "green")
        result += colored(" CountryCode:", "white")
        result += colored(str(self.tvmazeCountry), "green")
        result += colored(" cur.Season:", "white")
        result += colored(str(self.tvmazeCurerntSeason), "green")
        result += colored(" AirDate:", "white")
        result += colored(str(self.tvmazeAirdate), "green")
      except:
        print("CAN NOT DiSPLaY TVmaze Info")
      print("")

    #if "MOVIE" in self.section.upper():
    if self.featuresStripped["SECTION"].upper() == "MOVIES":
      try:
        result += colored(" IMDB:", "white")
        result += colored(str(self.imdbTconst), "green")
        result += colored(" primary:", "white")
        result += colored(str(self.imdbPrimaryTitle), "green")
        result += colored(" original:", "white")
        result += colored(str(self.imdbOriginalTitle), "green")
        result += colored(" year:", "white")
        result += colored(str(self.imdbYear), "green")
        result += colored(" rating:", "white")
        result += colored(str(self.imdbRating), "green")
        result += colored(" votes:", "white")
        result += colored(str(self.imdbVotes), "green")
        result += colored(" genres:", "white")
        result += colored(str(self.imdbGenre), "green")
        result += colored(" MOJO:", "white")
        result += colored(str(self.imdbTconst), "green")
        result += colored(" US:", "white")
        result += colored(str(self.mojoUsScreens), "green")
        result += colored(" UK:", "white")
        result += colored(str(self.mojoUkScreens), "green")
      except:
        print("CAN NOT DiSPLaY IMDB Info")
    return result  + "\n"


  def printServer(self): 
    result = ""
    if self.features["SECTION"] == "*":
      result += self.createLine("release", str(self.releaseName))
      result += self.createLine("warning", "section could not be found " + self.features["SECTION"], "red")
    else:
      result += ""
      result += self.createLine("release", self.releaseName)
      result += self.createLine("time", self.time)
      result += self.createLine("section", self.section)
      result += self.createLine("benchmark", self.benchmark)
    return result

  
  def createLine(self, title, value, color="white", blinking=False, terminator="\n"):
    result = ""
    result += str(self.arrowLeft) 
    result += colored('{1:<{0}}'.format(self.tabspace, str(title)), color)
    result += ": "
    result += str(value)
    result += terminator
    #TODO: here i would not colorize the title but the value
    #TODO: and i would also use bold and blinking for important messages
    return result

  def sortSites(self, siteArray):
    return " ".join(sorted(siteArray))


  def printUser(self, user):
    print(self.usersString(user))


    
# MAIN
# mostly for testing this module at the console
# ------------------------------------------------
def main():
  rls=Release("Stephen.Colbert.2019.01.25.John.Goodman.1080p.WEB.x264-TBS")
  rls.features["SECTION"] = "*"
  print(rls.printLong(None))
  rls.features["DUPE"] = "nodupe"
  rls.features["SECTION"] = "TV"
  rls.tvmazeID = "1"
  print(rls.printLong(None))
  rls.features["SECTION"] = "MOVIES"
  rls.imdbTconst = "1"  
  print(rls.printLong(None))


if __name__ == "__main__": main()




