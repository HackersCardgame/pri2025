#!/usr/bin/python3
#coding=utf-8
# cython: language_level=3, boundscheck=False
import os
import sys
import time
import signal
import traceback
from termcolor import colored, cprint

#add the parent directory to path
sys.path.append('.')
sys.path.append('..')

from common.dbConnector        import DBConnector


debug = False


#TODO: ctrl-c handler everywhere
#TODO: maybe integrate into udp-server so that there is only one database connection, so no locks


class ImdbImporter:

  path = os.getenv("HOME")+"/imdb_dumps/"
  files = ["title.basics.tsv", "title.ratings.tsv","title.akas.tsv"]
#  files = ["title.ratings.tsv"]

  #akas = "title_akas_tsv"
  #basics = "title_basics_tsv"
  #ratings = "title_ratings_tsv"

  #init the database connection
  #-----------------------------------------------------------------------------
  def init(self):
    signal.signal(signal.SIGINT, self.signal_handler)
    self.dbConn = DBConnector.getInstance()
    print('to Interrupt press Ctrl+C')
    #signal.pause()


  def signal_handler(self, sig, frame):
    print()
    print('You pressed Ctrl+C! Aborting...')
    self.dbConn.mariadb_connection.commit()
    self.dbConn.mariadb_connection.close()
    sys.exit(0)



  #import of the files
  #-----------------------------------------------------------------------------
  def importDB(self, table):
    print("importing the files from imdb.com database dumps from", ImdbImporter.path)

    try:
      if not table == None:
        files = [ table ]
    except:
      print("IMPORTING ALL")

    #import file per file
    for file in self.files:
      #replace points in filename since points are reserved in mysql/mariaDB
      tableName = file.replace('.','_')
      print("importing: " + colored(file, "green"), "to Table: " + colored(tableName, "green"))
      lineNumber=0;
      try:
        rows = self.dbConn.fetchall("describe "+tableName+";")
        if len(rows)<2: raise("ERROR") #TODO: TEST IF WORKING
      except:
        if debug: traceback.print_exc(file=sys.stdout)
        print("have you run the script (sudo mysql -h localhost  < ./imdb.sql) that creates the database, user and tables as root?")


      #open the tsv file and get the first line as table descripion
      with open(self.path+file, 'r', encoding="utf-8", errors='ignore') as lines:
        header = lines.readline()

      queryHeader = "REPLACE INTO " + tableName + " ("    #TODO: check if working

      colNumber = len(header.split("\t"))
      if debug: print(colNumber)

      #split the col headers for the first part of the sql query
      for col in header.split("\t"):
        colNumber-=1
        queryHeader = queryHeader + col.rstrip()          #strip the newline carachter at the end of the line
        if colNumber>0: queryHeader = queryHeader + ", "  #add a comma except at to the last header entry

      queryHeader = queryHeader + ") VALUES ("

      file = open(self.path+file, 'r', encoding="utf-8", errors='ignore')
      lines = file.readlines()

      errors=0

      #parse each line to insert it in to the database always using the same queryHeader
      for line in lines:
        lineNumber+=1
        if lineNumber==1: continue
        queryBody = ""
        cellNumber=len(header.split("\t"))
        
        #walk throught the cells from that line and add it to the sql query
        for cell in line.rstrip().split("\t"):
          cellNumber -= 1
          queryBody = queryBody + "'" + cell.replace("'","''").replace("\\N","") + "'"
          if cellNumber>0: queryBody = queryBody + ", "

        #complete the insert sql query
        sql = queryHeader + queryBody + ");"
        if debug: print(sql)
        
        try:
          self.dbConn.massModify(sql)
        except:
          if debug: print("Error:", sql)
          errors+=1


        #we print out some info that the "stupid" user can see that the import is still running
        if lineNumber % 10000 == 0: print("Imported: " + colored(str(lineNumber), "yellow") + " Errors: " + colored(str(errors), "red") + " inserted: " + colored(str(lineNumber-errors-1), "green") , end="\r", flush=True)

      print("\ncommiting transaction...")
      self.dbConn.mariadb_connection.commit()
      print()
  #TODO: check if >1000 entries      


  def createMemCopy(self):
    print("use sudo mysql -f pci < <repo>/tools/install/tables/memorycopy.sql")


  def createMemCopySmall(self):
    print("use sudo mysql -f pci < <repo>/tools/install/tables/memorycopy-small.sql")
    
    
# MAIN
# ------------------------------------------------
def main():

  ii = ImdbImporter()
  ii.init()

    
  if sys.argv[1] == "--import-ratings":
    ii.importDB("title.ratings.tsv")

  if sys.argv[1] == "--import-basics":
    ii.importDB("title.basics.tsv")

  if sys.argv[1] == "--import-akas":
    ii.importDB("title.akas.tsv")

  if sys.argv[1] == "--import":
    ii.importDB(None)

  if sys.argv[1] == "--memorycopy":
    ii.createMemCopy()
    
  print(sys.argv[1])
  if sys.argv[1] == "--memorycopy-small":
    ii.createMemCopySmall()
    

  ii.dbConn.mariadb_connection.close()


if __name__ == "__main__": main()





