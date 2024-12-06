#!/usr/bin/python3
#coding=utf-8
# cython: language_level=3, boundscheck=False
import re
import os
import sys
import inspect
import logging
import datetime
from termcolor import colored, cprint

from inspect import getframeinfo, stack

from io import StringIO

#add the parent directory to path
sys.path.append('.')
sys.path.append('..')

from results.release                    import Release

debug = False


class LoggerSingleton:
  __instance = None
  _logger = None
  _log_stream = StringIO()
  _logdata = ""

  @staticmethod
  def getInstance():
      """ Static access method. """
      if LoggerSingleton.__instance == None:
          LoggerSingleton()
      return LoggerSingleton.__instance


#TODO: also caller.methodName      
      
  @staticmethod
  def debug(message):
    severity = "DEBUG"
    caller = getframeinfo(stack()[1][0])
    line = ("%s %s:%d - %s" % ( severity, caller.filename.split("/")[-1], caller.lineno, message))
    LoggerSingleton.getInstance()._logdata += line + "\n"      

  @staticmethod
  def info(message):
    severity = "INFO"
    caller = getframeinfo(stack()[1][0])
    line = ("%s %s:%d - %s" % ( severity, caller.filename.split("/")[-1], caller.lineno, message))
    LoggerSingleton.getInstance()._logdata += line + "\n"
      
  @staticmethod
  def warning(message):
    severity = "WARNING"
    caller = getframeinfo(stack()[1][0])
    line = ("%s %s:%d - %s" % ( severity, caller.filename.split("/")[-1], caller.lineno, message))
    LoggerSingleton.getInstance()._logdata += line + "\n"

  @staticmethod
  def error(message):
    severity = "ERROR"
    caller = getframeinfo(stack()[1][0])
    line = ("%s %s:%d - %s" % ( severity, caller.filename.split("/")[-1], caller.lineno, message))
    LoggerSingleton.getInstance()._logdata += line + "\n"
    
  @staticmethod
  def flush():
    LoggerSingleton.getInstance()._logdata = ""

  def __init__(self):
      """ Virtually private constructor. """
      if LoggerSingleton.__instance != None:
          raise Exception("This class is a singleton!")
      else:
          LoggerSingleton.__instance = self
      
      logging.basicConfig(stream=self._log_stream, format='%(asctime)s [%(levelname)s | %(filename)s:%(lineno)s] > %(message)s', level=logging.INFO)


  def debuginfo(self, severity, message):
      caller = getframeinfo(stack()[1][0])
      return 
        

# ===================================================================================================
# Print Formated user list with site and rules from an 3D array
# ===================================================================================================
  @staticmethod
  def printLog(rls):
    result = ""
    if rls.fullLog:
      for line in LoggerSingleton.getInstance()._logdata.split("\n"):
        if "ERROR" in line:
          result += colored(line, "white", "on_red") + "\n"
        else:
          result += line + "\n"
    else:
      for line in LoggerSingleton.getInstance()._logdata.split("\n"):
        if "ERROR" in line:
          result += colored(line, "white", "on_red") + "\n"
    if debug: print("FLUSHING LOG")
    LoggerSingleton.getInstance().flush()

    return result


if __name__ == "__main__":


  print()
  print()
  cprint("U N I T  T E S T s  FOR LOGGER:", 'red', 'on_white')
  print()

  logging = LoggerSingleton()

  rls = Release("TEST.RELEASE.x264.1080p-ASDF")
  rls.fullLog = True

  logging.debug("DEBUG")
  logging.info("INFO")
  logging.warning("WARNING")
  logging.error("ERROR")

  print(logging.printLog(rls))

  logging.flush()

  print(logging.printLog(rls))



    
    

