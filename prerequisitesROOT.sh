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



# Helper Function for YES or NO Answers
#------------------------------------------------------------------------------
# Example YESNO "Question to ask" "command to be executed"
#==============================================================================
function YESNO {
echo -e -n "
${red}$1 [y/N]${default} "
read -d'' -s -n1 answer
echo
if  [ "$answer" = "y" ] || [ "$answer" = "Y" ]
then
return 0
else
echo -e "
"
return 1
fi
}
#==============================================================================


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
		echo ERROR: $1 >>$BASEPATH/log.txt
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




if test -f "/etc/apt/sources.list"
then
  echo ${green}debian${default}oid system detected
  
  echo -e "
if you are using debain stretch add the following line to /etc/apt/sources.list

   deb http://ftp.ch.debian.org/debian/ stretch-backports main contrib
   
   ${red}
    updated to bookworm ${default}

   ${green}
    python things are now only in user mode
    you should use always the same debian-user,
    eg pci (phase combogen interface)
   ${default}
    

and then type

   sudo apt-get update
   sudo apt-get install clang-6.0 (or use gcc)
"

# get the newest updates
#==============================================================================
  ShowAndExecute "cat -e /var/lib/dpkg/lock"

  ShowAndExecute "apt --fix-broken $ip install"

  ShowAndExecute "dpkg --configure -a"

  ShowAndExecute "apt-get -y $ip update"

  ShowAndExecute "apt-get -y $ip upgrade"

  export DEBIAN_FRONTEND=noninteractive

  #std ubuntu mirrir is fucking slow, taking the swiss one
  sed -i -e 's/http:\/\/archive.ubuntu.com\/ubuntu\//http:\/\/mirror.init7.net\/ubuntu\//g' /etc/apt/sources.list
  echo if it is not an ubuntu repository, it would just change nothing


  if YESNO "remove mysql for new install?"
  then
    apt-get purge -y mysql* *mysql*
  fi

  if YESNO "remove ${red}ALL${default} existing mysql databases for new install?"
  then
    rm -rf /var/lib/mysql
  fi



  ShowAndExecute "apt-get -y update"
  ShowAndExecute "apt-get -y upgrade"
  
  ShowAndExecute "apt-get purge python3-mysqldb"

  ShowAndExecute "apt-get install -y apt-utils"
  ShowAndExecute "apt-get install -y psmisc"

  ShowAndExecute "apt-get install -y time"
  ShowAndExecute "apt-get install -y curl"
  ShowAndExecute "apt-get install -y python3-dateutil"
  
#ubuntu
apt-get install -y mysql-server

  ShowAndExecute "apt-get install -y mysql"
  ShowAndExecute "apt-get install -y mysql-client"
  ShowAndExecute "apt-get install -y libmysqlclient-dev"

#debian
  ShowAndExecute "apt-get install -y default-mysql-server"
  ShowAndExecute "apt-get install -y default-libmysqlclient-dev"
  ShowAndExecute "apt-get install -y default-mysql-client"

#debian && ubuntu
  ShowAndExecute "apt-get install -y libmariadb-dev"

  ShowAndExecute "apt-get install -y g++"
  ShowAndExecute "apt-get install -y build-essential"
  ShowAndExecute "apt-get install -y libssl-dev"
  ShowAndExecute "apt-get install -y libpcre2-dev"

  echo -e " different distros, different package names, so it is
  ok if only one of the following three packages can be installed
  ---------------------------------------------------------"
  ShowAndExecute "apt-get install -y libreadline-dev"
  ShowAndExecute "apt-get install -y libreadline"
  ShowAndExecute "apt-get install -y libreadline7"
  echo "
  ---------------------------------------------------------" 

  ShowAndExecute "apt-get install -y python3"
  ShowAndExecute "apt-get install -y python3-pip"
  
  ShowAndExecute "apt-get install -y libssl-dev"

  ShowAndExecute "apt-get install -y valkyrie"
  echo valkyrie was removed in buster, was for debugging
  ShowAndExecute "apt-get install -y valgrind"
  ShowAndExecute "apt-get install -y vim-gtk3"

  #python3 -m venv ~/pri2025venv
  #source ~/pri2025venv/bin/activate

  #### THESE THIGNS ARE NOW in prerequisiteUSER.sh
  
  #python3 -m venv venv
  #source venv/bin/activate

  #ShowAndExecute "pip install requests"
  #ShowAndExecute "pip install lxml"
  #ShowAndExecute "pip install distance"
  #ShowAndExecute "pip install wget"
  #ShowAndExecute "pip install termcolor"
  #ShowAndExecute "pip install iso639"
  #ShowAndExecute "pip install mysqlclient"


#for old debian (stretch)
	if grep -q stretch "/etc/apt/sources.list"
	then
		echo "deb http://ftp.ch.debian.org/debian/ stretch-backports main contrib" >> /etc/apt/sources.list
		ShowAndExecute "apt-get -y update"
		ShowAndExecute "apt-get -y install clang-6.0"
	fi

  echo possibly all packages installed
fi

cat $BASEPATH/log.txt

mysql -e 'create database pci'
mysql -e 'show databases'
mysql pci -e 'source tools/install/tables/debian-user.sql;'
echo password is 123456, you SHOULD really change that NOW and also in the files and source
mysql pci --user pci -p -e 'show tables;' 
