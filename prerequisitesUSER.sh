#!/bin/bash

green="\e[1;92m"
yellow="\e[1;93m"
red="\e[91m"
default="\e[39m"

SCRIPT=$(readlink -f "$0")
BASEPATH=$(dirname "$SCRIPT")

echo ********************************************************************************
echo ********************************************************************************
echo LC_ALL: $LC_ALL
echo LC_CTYPE: $LC_CTYPE
echo ********************************************************************************
echo ********************************************************************************

echo "
--------------------------------
  started prerequisitesUSER.sh
-------------------------------- " >> $BASEPATH/log.txt

# ==============================================================================
function ShowAndExecute {
echo ================================================================================
echo -e "EXECUTING: ${yellow} $@ ${default}"
echo --------------------------------------------------------------------------------
#execute command
if [ "$2" = "" ]
then
  $1 >/dev/null
else
  $1 "$2"
fi
#test if it worked or give an ERROR Message in red, return code of apt is stored in $?
rc=$?;
if [[ $rc != 0 ]]
  then
	  echo -e ${red}ERROR${default} $rc
		echo ERROR: $0 >>$BASEPATH/log.txt
		if [ "$2" = "" ]
	  echo -e ${red}trying a second time ${yellow}$1 "$2" ${default}
		then
			$1 >/dev/null
		else
			$1 "$2"
		fi
		if [[ $rc != 0 ]]
			then
				echo -e ${red}SECOND ERROR${default} $rc
				echo ERROR 2: $1 >>$BASEPATH/log.txt
				echo giving up
			else
				echo -e ${green}SUCCESS${default} $rc
				echo SUCCESS 2: $1 >>$BASEPATH/log.txt
    fi
	  #exit 1 #REMOVED since pr-test crashes, TODO: reenable
	else
	  echo -e ${green}SUCCESS${default} $rc
		echo SUCCESS 1: $1 >>$BASEPATH/log.txt
fi
}
# ==============================================================================

  #do the python stuff not with root

  python3 -m venv ~/pri2025venv
  source ~/pri2025venv/bin/activate

  ShowAndExecute "pip install requests"
  ShowAndExecute "pip install lxml"
  ShowAndExecute "pip install distance"
  ShowAndExecute "pip install wget"
  ShowAndExecute "pip install termcolor"
  ShowAndExecute "pip install iso639"
  ShowAndExecute "pip install mysqlclient"
  ShowAndExecute "pip install wget"


  echo possibly all packages installed

cat $BASEPATH/log.txt


echo "

now you should cd to tools/insallation and run setup.sh"
