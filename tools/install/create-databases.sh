#!/bin/bash

green="\e[1;92m"
yellow="\e[1;93m"
red="\e[91m"
default="\e[39m"

SCRIPT=$(readlink -f "$0")
BASEPATH=$(dirname "$SCRIPT")

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
	  echo $1 >>errors.txt
	  #exit 1 #REMOVED since pr-test crashes, TODO: reenable
	else
	  echo -e ${green}SUCCESS${default} $rc
fi
}
# ==============================================================================


ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/tables/config2.sql;"

ShowAndExecute 'mysql pci --user pci -p -e' "DESCRIBE config2;"

ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/tables/history.sql;"

ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM history;"

ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/tables/dupes2.sql;"

ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM dupes2;"

ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/tables/tvmaze.sql;"

ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM tvmaze;"

ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/tables/episodes.sql;"

ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM episodes;"

ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/tables/imdb.sql;"

ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM title_basics_tsv;"
ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM title_akas_tsv;"
ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM title_ratings_tsv;"

ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/tables/mojo.sql;"

ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM mojo;"

ShowAndExecute 'mysql pci --user pci -p -e' "source $BASEPATH/tables/tmdb.sql;"

ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM tmdb;"

