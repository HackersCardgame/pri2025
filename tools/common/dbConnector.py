#!/usr/bin/python3
#coding=utf-8
# cython: language_level=3, boundscheck=False
import os
import re
import sys
import glob
import signal
import traceback
#import logging
import multiprocessing

from termcolor import colored, cprint
from inspect import getframeinfo, stack

import MySQLdb as mariadb
#import mysql.connector as mariadb

#add the parent directory to path
sys.path.append('.')
sys.path.append('..')

#from lib.logger           import LoggerSingleton
from common.logger             import LoggerSingleton as logging

#host = "localhost"  #shoud take this because it goes directly to the to the socket not first over the tcpip stack
host = "127.0.0.1"    #but needet for docker testcd 
db = "pci"
user = "pci"
passwd = "123456"


debug = False

class DBConnector:

  __instance = None

  readlock = multiprocessing.Lock()
  writelock = multiprocessing.Lock()

  @staticmethod
  def getInstance():
    """ Static access method. """
    if DBConnector.__instance == None:
      DBConnector()
    return DBConnector.__instance


  def __init__(self):
    """ Virtually private constructor. """
    if DBConnector.__instance != None:
      raise Exception("This class is a singleton!")
    else:
      DBConnector.__instance = self
      self.createConnection()


  def __del__(self):
    #caller = getframeinfo(stack()[1][0])
    caller = "dbConnector.py:54"
    try:
      self.mariadb_connection.close()
      if debug: print ("DESTRUCTOR: Disconnected from Mariadb::pci")
      print("Disconnected from MariaDB::pci")
    except Exception as e:
      #caller = getframeinfo(stack()[1][0])
      if debug: print("%s %s:%s - %s" % ( "ERROR", caller, caller , e))
      if debug: traceback.print_exc(file=sys.stdout)
      if debug: print("COULD NOT DESTROY DB CONNECTION")

  def createConnection(self):
    self.mariadb_connection = mariadb.connect(host=host, user=user, password=passwd, database=db, charset='utf8')
    #if self.mariadb_connection.is_connected():
    if True:
      if debug: print("Connected to MariaDB::pci")
      self.cursor = self.mariadb_connection.cursor()
      self.massupdateCursor = self.mariadb_connection.cursor()
    else:
      print("DATABASE ERROR")
      logging.error("Database ERROR, is the mysql/mariaDB running and does the pci database exists?")
      raise Exception("Database ERROR, is the mysql/mariaDB running and does the pci database exists?")
   
   
   
  #to reconnect if it disconnects after a while
  def execute(self, sql):
    self.readlock.acquire()
    try:
      self.cursor.execute(sql)
    except:
      if debug: traceback.print_exc(file=sys.stdout)
      self.mariadb_connection.commit()
      self.createConnection()
      try:
        self.cursor.execute(sql)
      except:
        if debug: traceback.print_exc(file=sys.stdout)
        if debug: print("giving up after 2 tries")
    self.readlock.release()

  #to reconnect if it disconnects after a while
  def fetchall(self, sql):
    results = None
    self.readlock.acquire()
    try:
      self.cursor.execute(sql)
      results = self.cursor.fetchall()
    except:
      if debug: traceback.print_exc(file=sys.stdout)
      try:
        self.mariadb_connection.commit()
        self.createConnection()
        self.cursor.execute(sql)
        results = self.cursor.fetchall()
      except:
        if debug: traceback.print_exc(file=sys.stdout)
        if debug: print("giving up after 2 tries")
    finally:
      self.readlock.release()
      return results

  #to reconnect if it disconnects after a while
  def fetchone(self, sql):
    results = None
    try:
      self.cursor.execute(sql)
      row = self.cursor.fetchone()
    except:
      if debug: traceback.print_exc(file=sys.stdout)
      try:
        self.mariadb_connection.commit()
        self.createConnection()
        self.cursor.execute(sql)
      except:
        if debug: traceback.print_exc(file=sys.stdout)
        if debug: print("giving up after 2 tries")
    finally:
      return results

  def modify(self, sql):
    self.writelock.acquire()
    try:
      self.cursor.execute(sql)
      self.mariadb_connection.commit()
    except:
      if debug: print("could not modify Database")
      raise Exception("could not modify Database")
    finally:
      self.writelock.release()


  def massModify(self, sql):
    try:
      self.massupdateCursor.execute(sql)
    except:
      if debug: print("could not modify Database")
      raise Exception("could not modify Database")


# MAIN
# mostly for testing this module at the console
# ------------------------------------------------
def main():

  print()
  print()
  cprint("U N I T  T E S T s  FOR DBCONNECTOR:", 'red', 'on_white')
  print()

  dbConnector = None
  try:
    dbConnector = DBConnector.getInstance()
    cprint("connected properly", "white", "on_green")
    print()
    result = dbConnector.fetchall("show tables")
    if len(result)<11:
      cprint("table(s) missing", "white", "on_red")
    else:
      cprint("table(s) maybe not missing", "white", "on_green")
    print()
    for i in result:
      string = str(i[0])
      if string in [ "akas", "basics", "config", "dupes", "episodes", "mojo", "ratings", "title_akas_tsv", "title_basics_tsv", "title_ratings_tsv", "tvmaze" ]:
        cprint(string + " found", "white", "on_green")
      else:
        cprint(str(i) + " not found", "white", "on_red")
    print()
    result = dbConnector.fetchone("describe config")
    if result == None or len(result)<1:
      cprint("Database connection error", "white", "on_red")
    else:
      cprint("db connection working properly", "white", "on_green")
    print()
    result = dbConnector.fetchall("SELECT * from config where configFile like '%general.cfg%' LIMIT 10;")
    if len(result)<10:
      cprint("Database connection error", "white", "on_red")
    else:
      cprint("config file found", "white", "on_green")
    print()

  except:
    if debug: traceback.print_exc(file=sys.stdout)
    cprint("could not initialize dbConnector, or find all the tables, password correct?", "white", "on_red")
  


if __name__ == "__main__": main()




