#!/usr/bin/python3
#coding=utf-8
# cython: language_level=3, boundscheck=False
#---------------------------------------#
# code
#---------------------------------------#

import os
import sys
import glob
import wget
import gzip
import time
import traceback

debug = False

sys.path.append('.')
sys.path.append('..')

#we use a class, that is a container that contains the variables and the functions
class imdbUpdater:
                     # added the direcory of the file executed so that script can also be run from root directory
        basepath = os.path.dirname(os.path.abspath(".")) + "/install/" #we get the basepath, the path where the running update.py is in
        targetpath = os.getenv("HOME")+"/imdb_dumps/"
        imdb_links = basepath + "imdb_links.txt"                    #txt file where the links are stored

        print(basepath)

        #init
        def __init__(self):
                if debug: print("ii: INIT imdbUpdater")
                try:
                        os.mkdir(self.targetpath)
                except:
                        if debug: traceback.print_exc(file=sys.stdout)
                        print("Could not make path " + self.targetpath)


        #walking through the imdb_links file, the self in parseFiles(self) is required because this function is a member of the class
        def parseFiles(self):
                with open(self.imdb_links) as fileList:
                        for line in fileList:
                                print(line, end='')
                                filename = line.split("/")[-1][:-4]
                                self.renameOldFile(filename)
                                self.downloadNewFile(line, filename)
                                

        #we have a function for renaming the file, we dont delete because if it runs at 3 days and the url changed we dont have any file anymore
        def renameOldFile(self, filename):
                print(filename + "=>" + str(int(time.time())))
                filenameOld = filename+".old."+str(int(time.time()))
                try:
                        os.rename(self.targetpath+filename, self.targetpath+filenameOld)
                        print("Renamed {} to {}".format(filename, filenameOld))
                except:
                        print("Could not rename the file "+filename)


        #we have a function that downlaods the file, also class member function, and decompresses it
        def downloadNewFile(self, url, filename):
                print(url)
                print("Downloading...")
                wget.download(url=url[:-1], out=self.targetpath) #we need to remove the \n character (newline character) so we use [:-1] and use an output path out=
                print("")
                print("Decompress...")

                with gzip.open(self.targetpath + filename+".gz", 'rb') as f:
                        content = f.read()
    
                with open(self.targetpath + filename, 'wb') as f:
                        f.write(content)

                for oldGZ in glob.glob(self.targetpath + "/*.gz"): #we need to remove the old .gz files, otherwise the wget will create ...(1).gz ...(2).gz and we will not get the new version but always import the oldest
                        os.remove(oldGZ)
                        print(oldGZ)

#---------------------------------------#
# MAIN
# --------------------------------------#
def main():

        iu = imdbUpdater()

        print("Parsing: " + iu.imdb_links) #if you created a class (line 63) then the class-members are accessable like this
                                           #from outside you would use here ui.imdb_links and from inside the class you would use self.imdb_links
        iu.parseFiles()                    #the renamveOldFile and the downloadNewFile is now called in the iu.parseFiles() method = function


if __name__ == "__main__": main()


