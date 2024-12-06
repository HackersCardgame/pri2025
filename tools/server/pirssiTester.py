#!/usr/bin/python3
#coding=utf-8
# cython: language_level=3, boundscheck=False
import os
import sys
import time
import socket
#import logging
import re
from termcolor import colored, cprint

#add the parent directory to path
sys.path.append('.')
sys.path.append('..')

from common.logger             import LoggerSingleton as logging

# ===================================================================================================
# class to transmit extracted data to cbftp udp port
# ===================================================================================================
class PirssiTester:
   
  def send(host, port, release):
  
    release = PirssiTester.cleanReleaseName(release)

    server_address = (host, int(port))

    UtfMessage =  release
    MESSAGE = bytes(UtfMessage, 'utf-8')

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 

    try:
      sent = sock.sendto(MESSAGE, server_address)
    except:
      logging.error('socket error')
    finally:
      sock.close()

    logging.info(MESSAGE)


  def cleanReleaseName(release):
    if "/" in release:

#      return release.split("/")[1]
#    else: return release

      #strip trailing characters
      release = re.sub(r'[^A-Z0-9]*$', r'', release)

      #Strip anything prior and including /
      release = re.sub(r'.*\/', r'', release)
      if "-" in release:
        return release

    else: return release


# MAIN
# mostly for testing this module at the console
# ------------------------------------------------
def main():

  #TODO: this is not a real unittest


  print()
  print()
  cprint("U N I T  T E S T s  FOR PIRSSITESTER:", 'red', 'on_white')
  print()

  #TO test the releases run python3 pirssiTester.py test
  testArray = [ [ "0125/Lucer-Ghost_Town-WEB-2019-ENTiTLED", "Lucer-Ghost_Town-WEB-2019-ENTiTLED"  ] ,
                [ "#@!$/Lucer-Ghost_Town-WEB-2019-ENTiTLED/", "Lucer-Ghost_Town-WEB-2019-ENTiTLED"  ] ,
                [ "aaa/#@!$/Lucer-Ghost_Town-WEB-2019-ENTiTLED/.", "Lucer-Ghost_Town-WEB-2019-ENTiTLED"  ] ,
                [ "0125/Lucer-Ghost_Town-WEB-2019-ENTiTLED/lala", "Lucer-Ghost_Town-WEB-2019-ENTiTLED"  ] ,
                [ "a//a0125/Lucer-Ghost_Town-WEB-2019-ENTiTLED/lala////", "Lucer-Ghost_Town-WEB-2019-ENTiTLED"  ] ,
                [ "0125#/Lucer-Ghost_Town-WEB-2019-ENTiTLED/@lala", "Lucer-Ghost_Town-WEB-2019-ENTiTLED"  ] ,
                [ "Lucer-Ghost_Town-WEB-2019-ENTiTLED/asdf", "Lucer-Ghost_Town-WEB-2019-ENTiTLED"  ],
                [ "0125#/thisoneshouldfail/@lala", "thisoneshouldfail"  ] ]

  try:
    if len(sys.argv)<2:
      for test in testArray:
        result = PirssiTester.cleanReleaseName(test[0])
        if result  == test[1]:
          cprint(test[0] + " -- " + test[1], "white", "on_green")
        else:
          cprint(result + " -- " + test[1], "white", "on_red")
  except:
    print("ERROR")

  #TO TEST: python3 lib/pirssiTester.py 0125/Lucer-Ghost_Town-WEB-2019-ENTiTLED

  try:
    if not sys.argv[1] == None:
      PirssiTester.send("localhost",5555, sys.argv[1])

  except:
    print("NO PARAMETER GIVEN")
    PirssiTester.send("localhost",5555, "the.matrix.1999.x264.bluray-ASDF")
    PirssiTester.send("localhost",5555, "asddasgff-codex")
    PirssiTester.send("localhost",5555, "the.matrix.1999.x264.bluray-ASDF")
    PirssiTester.send("localhost",5555, "asddasgff-codex")

  
if __name__ == "__main__": main()




