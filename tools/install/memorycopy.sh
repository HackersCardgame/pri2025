#!/bin/bash

green="\e[1;92m"
yellow="\e[1;93m"
red="\e[91m"
default="\e[39m"

SCRIPT=$(readlink -f "$0")
BASEPATH=$(dirname "$SCRIPT")

date
echo -e "${yellow}creating memorycopy${default}"
time mysql --user pci -p -fv pci < $BASEPATH/tables/memorycopy.sql

date
echo -e "${yellow}creating memorycopy-small${default}"
#       time python3 imdbImport.py --memorycopy-small
time mysql --user pci -p -fv pci < $BASEPATH/tables/memorycopy-small.sql

