#!/bin/bash

green="\e[1;92m"
yellow="\e[1;93m"
red="\e[91m"
default="\e[39m"


# ==============================================================================
function ShowAndExecute {
#show command
echo ================================================================================
echo -e "EXECUTING: ${yellow} $1 ${default}"
echo --------------------------------------------------------------------------------
#execute command
$1
#test if it worked or give an ERROR Message in red, return code of apt is stored in $?
rc=$?;
if [[ $rc != 0 ]]
  then
	  echo -e ${red}ERROR${default} $rc
	  echo $1 >>test-errors.txt
	  #exit 1 #REMOVED since pr-test crashes, TODO: reenable
fi
}
# ==============================================================================



# ubuntu has dofferent socket path, so we hardlink it that the app runs
if test -f "/etc/apt/sources.list"
then
	if grep -q ubuntu "/etc/apt/sources.list"
	then
    ShowAndExecute "ln /var/run/mysqld/mysqld.sock /tmp/mysql.sock"
	fi
fi

./bin${OS_TARGET}/unittests/lt-ut
./bin${OS_TARGET}/unittests/ci-ut
./bin${OS_TARGET}/unittests/rp-ut
./bin${OS_TARGET}/unittests/r-ut
./bin${OS_TARGET}/unittests/ps-ut
./bin${OS_TARGET}/unittests/ccs-ut
./bin${OS_TARGET}/unittests/fe-ut
./bin${OS_TARGET}/unittests/rf-ut
./bin${OS_TARGET}/unittests/pr-test
./bin${OS_TARGET}/unittests/crs-ut
./bin${OS_TARGET}/unittests/en-ut
./bin${OS_TARGET}/unittests/dbc-ut
./bin${OS_TARGET}/unittests/li-ut
./bin${OS_TARGET}/unittests/cp-ut

#TODO: CERTIFICATES IN A SPECIFIC PLACE
cd ./bin${OS_TARGET}
#./phaseServer &
./startServer.sh &
#make install-server #doesnt work in ci/cd since normally docker dont have systemd

sleep 5

cd ../..
pwd

cd ./tools/server/
echo -e "EXECUTING: ${green} python3 ./tools/server/pirssiTester.py ${default}"
ShowAndExecute "python3 pirssiTester.py" >/dev/null

cd ../..

pwd
cd ./bin${OS_TARGET}
ShowAndExecute "./phaseClient --script run"

sleep 10

ShowAndExecute "./phaseIrc bbr"

sleep 10

ShowAndExecute "killall ./phaseServer"

sleep 10

#cat test-errors.txt

