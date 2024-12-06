#!/usr/bin/python3
#coding=utf-8
import os
import re
import sys
import glob
import signal
import requests
import traceback
#import logging
from termcolor import colored, cprint

#add the parent directory to path
sys.path.append( os.path.abspath(os.path.join(os.path.realpath(__file__), "../..")))

from common.dbConnector        import DBConnector
from common.logger             import LoggerSingleton as logging
from results.release           import Release

debug = False

class DbConfigFiles:

  def __init__(self):
    self.dbConn = DBConnector.getInstance()
  
  errors = 0
  inserted = 0

  configtable = "config2"

  generalConfigFile = os.path.dirname(os.path.abspath(__file__))+"/../../cfg/general.cfg"
  configFilePath = os.path.dirname(os.path.abspath(__file__)) + "/../../cfg/"


# =========================================
# TOOLS
# =========================================


  def truncateDublespaces(self, line):
    configWords = "order", "feature", "topic", "keyword", "rule", \
                  "replacetvmaze", "replaceimdb", "ruletemplate", \
                  "separator", "site", "user", "sites", "host", \
                  "port", "password", "dir", "affils", "blacklist", \
                  "whitelist", "sectionblacklist", "sectionwhitelist", \
                  "screenoverridegroups", "networkblacklist", "networkwhitelist"

    newLine = ""

    if line.split(" ")[0] in configWords:
      parts = line.split("#")
      newLine = parts[0].strip()
      newLine = re.sub('\s{2,}', ' ', newLine)
      if len(parts)>1:
        newLine = newLine + " #" + " ".join(parts[1:])
    else:
      newLine = line

    return newLine




  def getFromDB(self, configFile):
    sql = "SELECT configFile, parameter, modifier, term, notes, lineNumber FRoM " + self.configtable + " where configFile = " + repr(configFile) + " ORDER BY lineNumber;"
    if debug: print(sql)
    result = []

    try:
      results = self.dbConn.fetchall(sql)
      if debug: print(results)
    except:
      if debug: traceback.print_exc(file=sys.stdout)
      logging.error("Could not fetch config " + configFile + " from Database")

    return results









# =========================================
# IMPORT
# =========================================


  # import configfile from pci32/cfg/configFile into the database
  # ----------------------------------------------------------------------------
  def importFromFile(self, configFile):

    configFileWithPath = self.configFilePath + configFile              #TODO: create possibility to override path cfg and use maybe backup/*/user01.cfg
                                                                                #python3 lib/dbConfigFiles.py --import ./backups/1902012135/user01.cfg works already wiht cfg as basepath
    if not os.path.exists(configFileWithPath):
      logging.error(configFileWithPath + " CONFIG FILE NOT FOUND")
      return False
    
    sql = "DELETE FROM " + self.configtable + " where configFile like " + repr(configFile) + ";"


    try:
      self.dbConn.modify(sql)
    except:
      logging.error("Could not insert line to config Database")
      if debug: traceback.print_exc(file=sys.stdout)

    lineNumber = 0

    

    with open(configFileWithPath, encoding="latin1", mode="r") as myfile:
      for line in myfile:
        lineNumber+=1
        parameterPostfix = ""
        inserted = False
        if debug: print(line)
        while (not inserted):
          if debug: print("while" + str(parameterPostfix))
          if not parameterPostfix == "" and parameterPostfix > 1000:
            print("ERROR: " + str(configFileWithPath) + " (EXITING)")
            exit()
          try:
            self.processLine(configFile, line, parameterPostfix, lineNumber)
            inserted = True
          except:
            if parameterPostfix == "":
              parameterPostfix = 1
            else:
              if debug: traceback.print_exc(file=sys.stdout)
              parameterPostfix += 1

        #we print out some info that the "stupid" user can see that the import is still running
        if self.inserted % 500 == 0:
          print("importing: " + colored(str(configFile) + ": " + str(lineNumber), "yellow") + " Errors: " + \
              colored(str(self.errors), "red") + " inserted: " + colored(str(self.inserted), "green") + "             ", end="\r", flush=True)



  def processLine(self, configFile, line, parameterPostfix, lineNumber):
      line = self.truncateDublespaces(line)

      parameter = ""  
      modifier = ""
      notes = ""

      line = line.split("\n")[0]

      if len(line) > 0 and not line[0] == "#":
        try:
          parameter = line.split("=")[0].split(" ")[0].strip()
          if debug: print(parameter)

          if parameter == "": parameter = "#"+str(lineNumber)
          if parameterPostfix == "":
            modifier = line.split("=")[0].split(" ")[1].strip()
          else:
            modifier = line.split("=")[0].split(" ")[1].strip() + "-" + str(parameterPostfix)
        except:
          if debug: traceback.print_exc(file=sys.stdout)
          logging.error("EE: " + configFile + " Parameter/Modifier Error in Line " + str(lineNumber) + " " + line)

        try:
          notes = line.split("#")[1:]
          if notes == []: notes = ""
          else: notes = "# ".join(notes)
          line = line.split("#")[0]
        except:
          if debug: traceback.print_exc(file=sys.stdout)
          logging.error("EE: " + configFile + " Comment(#) Error in line: " + str(lineNumber) + " " + line)
          
        try:
          line = line.split("=")[1].strip()
          if parameter == "rule":
            line = re.sub('\(', '', line)
            line = re.sub('\)', '', line)
        except:
          if debug: traceback.print_exc(file=sys.stdout)
          logging.error("EE: " + configFile + " Term Error line: " + str(lineNumber) + " " + line)


        if parameter == "password":
          sql = "INSERT INTO " + self.configtable + " (configFile, parameter, modifier, term, notes, lineNumber) VALUES (" + \
              repr(configFile.lower()) + ", " + repr(parameter.lower()) + ", " + repr(modifier.lower()) + ", " + repr(line) + ", " + repr(notes) + ", " + repr(lineNumber) + ");"
        else:
          sql = "INSERT INTO " + self.configtable + " (configFile, parameter, modifier, term, notes, lineNumber) VALUES (" + \
              repr(configFile.lower()) + ", " + repr(parameter.lower()) + ", " + repr(modifier.lower()) + ", " + repr(line.lower()) + ", " + repr(notes) + ", " + repr(lineNumber) + ");"
        
      else:

        sql = "INSERT INTO " + self.configtable + " (configFile, parameter, modifier, term, notes, lineNumber) VALUES (" \
              + repr(configFile) + ", " + repr("#" + str(lineNumber)) + ", " + repr(modifier) + ", " + "''" + ", " + repr(line) + ", " + repr(lineNumber) + ");"

      if debug: print(sql)

      try:
        self.dbConn.modify(sql)
        self.inserted += 1
      except:
        self.errors += 1
        logging.error("Could not insert line to config Database: " + configFile + " : " + parameter + " : " + modifier)
        if debug: traceback.print_exc(file=sys.stdout)
        raise Exception("Line insertion error")



# =========================================
# EXPORT
# =========================================

  # import all the config files from pci32/cfg/*.cfg to the database
  # ----------------------------------------------------------------------------
  def importAllConfigfiles(self):
    print()
    print( colored("Errors will be shown after the import is finished, previous config files will be wiped", "yellow"))
    print()
    allConfigfiles = glob.glob(self.configFilePath + '*.cfg')
    for configFile in allConfigfiles:
      slashes = configFile.count("/")
      if debug: print(slashes, configFile, configFile.split("/")[slashes])
      self.importFromFile(configFile.split("/")[slashes])


  # list the items (*.cfg) configured in the database
  # ----------------------------------------------------------------------------
  def listItems(self):
    sql = "SELECT distinct configFile from " + self.configtable + ";"

    results = []
    
    try:
      results = self.dbConn.fetchall(sql)
    except:
      logging.error("Could get list from the configfiles from the database")
      traceback.print_exc(file=sys.stdout)

    for tupel in results:
      print(tupel[0])
  
    return results


  # exporting all configs from the database to a destinactionDirectory
  # ----------------------------------------------------------------------------
  def exportAllConfigfiles(self, destinationDirectory):
  
    results = self.listItems()
  
    for item in results:
      print(item[0])
      self.exportToFile(item[0], destinationDirectory)




  # export configFile from the database to a destinationDirectory
  # ----------------------------------------------------------------------------
  def exportToFile(self, configFile, destinationDirectory):
  
    results = self.getFromDB(configFile)
    #TODO, proper \n
    #TODO; Not working anymore

    configFileWithPath = destinationDirectory + configFile
    
    dest = open(configFileWithPath, 'w')

    for tupel in results:
      if tupel[1] == "":
        if tupel[4] == "":
          dest.write(tupel[3] + "\n")
        else:
          dest.write(tupel[3] + " # " + tupel[4] + "\n")
      elif tupel[2] == "":
        if tupel[4] == "":
          dest.write(tupel[1] + " = " + tupel[3] + "\n")    #TODO: add modifier parameter
        else:
          dest.write(tupel[1] + " = " + tupel[3] + " # " + tupel[4] + "\n")
      else:
        if tupel[4] == "":
          dest.write(tupel[1] + " " + tupel[2] + " = " + tupel[3] + "\n")
        else:
          dest.write(tupel[1] + " " + tupel[2] + " = " + tupel[3] + " # " + tupel[4] + "\n")
    dest.close()


  def exportToString(self, configFile): #TODO

    results = self.getFromDB(configFile)
    #TODO, proper \n
    
    result = ""
    
    for tupel in results:
      print(tupel[1])
      result += tupel[1] + "\n"

    return result
    






    
# MAIN
# -----------------------------------------------------------------------
def main():

  dbc = DbConfigFiles()

  order = {}
    
  if len(sys.argv)<2:
    dbc.listItems()
    print()
    print()
    cprint("U N I T  T E S T s  FOR DbConfigFiles:", 'red', 'on_white')
    print()
    cprint("You need to test this manually, here is how you test it:", 'white', 'on_red')
    print()
    print("to import a files from the cfg path into the database type:")
    cprint(" python3 configDatabase.py --import file.cfg", "cyan")
    print()

    print("to import all file from the cfg path into the database type:")
    cprint(" python3 configDatabase.py --import all", "cyan")  
    print()
    print("to export a config from the database to a file type:")
    cprint(" python3 configDatabase.py --export file.cfg <destinationPath>", "cyan")
    print()
    print("to export a config from the database to a file type:")
    cprint(" python3 configDatabase.py --export all <destinationPath>", "cyan")
    print()
    print("to list all configured items from the database type:")
    cprint(" python3 configDatabase.py --list", "cyan")
    print()

  else:
    if len(sys.argv) > 2:
      if sys.argv[1].lower() == "--import":
        if sys.argv[2].lower() == "all":
          print("importing ALL")
          dbc.importAllConfigfiles()
        else:
          print("importing " + sys.argv[2])
          dbc.importFromFile(sys.argv[2])
    if len(sys.argv) > 2:
      if sys.argv[1].lower() == "--export":
        if sys.argv[2].lower() == "all":
          print("exporting ALL")
          dbc.exportAllConfigfiles(sys.argv[3]+"/")
        else:
          print("exporting " + sys.argv[2])
          dbc.exportToFile(sys.argv[2], "./")
    if len(sys.argv) > 1:
      if sys.argv[1].lower() == "--list":
        dbc.listItems()

    rls = Release("only Log")
    rls.fullLog = True
    #print(logging.printLog(rls))          


    print("--------------------------------------------------------------------------------")
      

if __name__ == "__main__": main()



