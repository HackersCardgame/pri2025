#!/bin/bash
# Color Definitions
#==============================================================================
red="\e[91m"
blue="\e[0;34m"
default="\e[39m"
#==============================================================================

SCRIPT=$(readlink -f "$0")
BASEPATH=$(dirname "$SCRIPT")/..

echo ===========================================================================
echo -e "         ${blue}UPDATING IMDB DATABASE${default}"
echo ===========================================================================
echo press enter to continue
read -p "REALLY continue??? (press CTRL-C to interrupt)"
echo
echo
echo



python3 imdbGet.py
echo ------------------ importing IMDB DATAFILES TO DB -------------------------
time python3 $BASEPATH/install/IMDBimport.py --import
echo ------------------- creating memory copy of IMDB --------------------------
time python3 $BASEPATH/install/IMDBimport.py --memorycopy
echo ------------------ you must now restart server.py -------------------------
time python3 $BASEPATH/install/IMDBimport.py --memorycopy-small
echo ------------------ you must now restart server.py -------------------------
echo -e "you must now restart ${blue}../lib/server.py${default}"

