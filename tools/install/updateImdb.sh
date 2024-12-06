#!/bin/bash


green="\e[1;92m"
red="\e[91m"
default="\e[39m"

SCRIPT=$(readlink -f "$0")
BASEPATH=$(dirname "$SCRIPT")

# ==============================================================================
# ==============================================================================
function ShowAndExecute {
#show command
echo ================================================================================
echo -e "EXECUTING: ${green} $@ ${default}"
echo --------------------------------------------------------------------------------
#execute command
if [ "$2" = "" ]
then
  $1
  echo x $1
else
  $1 "$2"
fi
#test if it worked or give an ERROR Message in red, return code of apt is stored in $?
rc=$?;
if [[ $rc != 0 ]]
  then
	  echo -e ${red}ERROR${default} $rc
	  echo $1 >>errors.txt
#	  cat errors.txt
	  #exit 1 #REMOVED since pr-test crashes, TODO: reenable
	else
	  echo -e ${green}SUCCESS${default} $rc
fi
}
# ==============================================================================

#DEBIAN
if test -f "/etc/apt/sources.list"
then
  echo debian-oid system like Debian, Ubuntu...

fi

#ON CENTOS
################################################################################
if test -f "/bin/rpm"
then
  echo maybe centos
  sleep 10

fi



#ON ALL OPERATING SYSTEMS
################################################################################

ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM title_akas_tsv;"
ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM title_basics_tsv;"
ShowAndExecute 'mysql pci --user pci -p -e' "SELECT count(*) FROM title_ratings_tsv;"


pwd
pushd $(pwd)
cd $BASEPATH/
pwd

#DEBIAN
if test -f "/etc/apt/sources.list"
then

	date
	echo getting newesd imdb files from imdb.com
	time python3 imdbGet.py

  date
	date
	echo skipping imdbImport
	time python3 imdbImport.py --import

  date
	echo creating memorycopy
	time python3 imdbImport.py --memorycopy

  date
	echo creating memorycopy-small
	time python3 imdbImport.py --memorycopy-small

fi


#CENTOS
if test -f "/bin/rpm"
then
	scl enable rh-python36 bash <<EOF

		date
		echo getting newesd imdb files from imdb.com
		time python3 imdbGet.py

		date
		echo skipping imdbImport
		time python3 imdbImport.py --import

    date
		echo creating memorycopy
		time python3 imdbImport.py --memorycopy

    date
		echo creating memorycopy-small
		time python3 imdbImport.py --memorycopy-small

		date
		echo importing one single mojo to check functionality
		python3 mojoUpdate.py

    ls weeklyMojoDumps

		date
		echo importing one single mojo to check functionality
		python3 mojoImport.py

    ShowAndExecute 'mysql pci -e' "SELECT * from mojo;"

EOF
fi

popd

ShowAndExecute 'mysql pci -e' "SELECT count(*) FROM title_akas_tsv;"
ShowAndExecute 'mysql pci -e' "SELECT count(*) FROM title_basics_tsv;"
ShowAndExecute 'mysql pci -e' "SELECT count(*) FROM title_ratings_tsv;"


