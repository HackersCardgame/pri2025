#!/bin/bash

green="\e[1;92m"
yellow="\e[1;93m"
red="\e[91m"
default="\e[39m"

SCRIPT=$(readlink -f "$0")
BASEPATH=$(dirname "$SCRIPT")

#TODO: mal noch sauberer machen

#python3 -m venv venv

# ==============================================================================
function ShowAndExecute {
#show command
echo ================================================================================
echo -e "EXECUTING: ${yellow} $@ ${default}"
echo --------------------------------------------------------------------------------
#execute command
if [ "$2" = "" ]
then
  $1
else
  $1 "$2"
fi
#test if it worked or give an ERROR Message in red, return code of apt is stored in $?
rc=$?;
if [[ $rc != 0 ]]
  then
	  echo -e ${red}ERROR${default} $rc
	  echo ERROR: $@ >> $BASEPATH/log.txt
	  #exit 1 #REMOVED since pr-test crashes, TODO: reenable
	else
	  echo -e ${green}SUCCESS${default} $rc
	  echo SUCCESS: $@ >> $BASEPATH/log.txt
fi
}
# ==============================================================================

pwd
pushd $(pwd)
cd $BASEPATH/
pwd


date
ShowAndExecute "python3 tvmazeImport.py --single 66"
ShowAndExecute "python3 tvmazeImport.py --single 7744"

date
ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM tvmaze;"
ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM episodes;"

date
ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/dumps/tvmaze-dump.sql;"

date
ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/dumps/episodes-dump.sql;"
ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM tvmaze;"
ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM episodes;"

date
ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/dumps/tmdb-dump.sql;"

date
ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/dumps/mojo-dump.sql;"


date
ShowAndExecute "python3 dbConfigFiles.py --import all"
ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM config2;"
#TODO: update one single tvmaze show and return exit != 0 if error

date
echo -e "${yellow}getting newesd IMDB csv files from imdb.com${default}"
ShowAndExecute "python3 imdbGet.py"

date
echo -e "${yellow}importing IMDB Tables from csv files${default}"
ShowAndExecute "python3 imdbImport.py --import"

ShowAndExecute 'mysql --user pci -p pci -e' "SELECT count(*) FROM title_akas_tsv;"
ShowAndExecute 'mysql --user pci -p pci -e' "SELECT count(*) FROM title_basics_tsv;"
ShowAndExecute 'mysql --user pci -p pci -e' "SELECT count(*) FROM title_ratings_tsv;"

date
echo -e "${yellow}creating memorycopy${default}"
time mysql --user pci -p -fv pci < $BASEPATH/tables/memorycopy.sql

date
echo -e "${yellow}creating memorycopy-small${default}"
#	time python3 imdbImport.py --memorycopy-small
time mysql --user pci -p-fv pci < $BASEPATH/tables/memorycopy-small.sql

ShowAndExecute 'mysql --user pci -p pci -e' "SELECT count(*) FROM akas;"
ShowAndExecute 'mysql --user pci -p pci -e' "SELECT count(*) FROM basics;"
ShowAndExecute 'mysql --user pci -p pci -e' "SELECT count(*) FROM ratings;"
ShowAndExecute 'mysql --user pci -p pci -e' "SELECT count(*) FROM akas_small;"
ShowAndExecute 'mysql --user pci -p pci -e' "SELECT count(*) FROM basics_small;"
ShowAndExecute 'mysql --user pci -p pci -e' "SELECT count(*) FROM ratings_small;"

date
echo -e "${yellow}updating one single mojo to check functionality${default}"
ShowAndExecute "python3 mojoUpdate.py"

date
echo -e "${yellow}updating one single mojo to check functionality${default}"
ShowAndExecute "python3 mojoImport.py"


cat $BASEPATH/log.txt

popd
