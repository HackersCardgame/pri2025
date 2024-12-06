#make -p

#CFLAGS: Extra flags to give to the C compiler.
CFLAGS = 

#CXXFLAGS: Extra flags to give to the C++ compiler.
CXXFLAGS = -std=c++17 `mariadb_config --cflags --libs` -lpcre2-8 -Wall -lpthread


GCC_CXXFLAGS = -DMESSAGE='"Compiled with G++"' -Wno-misleading-indentation
CLANG_CXXFLAGS = -DMESSAGE='"Compiled with Clang++"'
UNKNOWN_CXXFLAGS = -DMESSAGE='"Compiled with an unknown compiler"'

ifeq ($(CXX),g++)
  CXXFLAGS += $(GCC_CXXFLAGS)
else ifeq ($(CXX),clang++-6.0)
  CXXFLAGS += $(CLANG_CXXFLAGS)
else
  CXXFLAGS += $(UNKNOWN_CXXFLAGS)
endif

#CPPFLAGS: Extra flags to give to the C preprocessor and programs that use it (the C and Fortran compilers). 
CPPFLAGS = 

CCFLAGS = 

#for Debian 9 enable this or specify on command line: "make server client irc CXX=clang++-6.0"
#CXX = clang++-6.0

BIN=./bin${OS_TARGET}
UNITTESTS=$(BIN)/unittests
SOURCE=./src
INCL = -I$(SOURCE)

red="\e[91m"
yellow="\e[0;33m"
green="\e[0;32m"
default="\e[39m"


all: executable


debug: CXXFLAGS += -DDEBUG -g -gdwarf-2 -std=c++17 `mariadb_config --cflags --libs` -lpcre2-8 -lpthread -Wall
debug: CFLAGS += -DDEBUG -g
debug: executable

executable: server client irc test parserelease cbftp tvmaze imdb jaro checkrule checkrulesyntax parsesite commandparser dbconnection featureextractor extractname regexparser regexfunctions release

install: server client irc uninstall-server install-server install-client

server: src/server/udpServerMain.cpp
	mkdir -p $(BIN)
	$(CXX) src/server/udpServerMain.cpp -o $(BIN)/phaseServer $(CXXFLAGS) -lssl -lcrypto
	chmod u+x $(BIN)/phaseServer
	cp src/server/cert.pem $(BIN)
	cp src/server/key.pem $(BIN)
	cp src/server/startServer.sh $(BIN)
	cp tools/install/tables/memorycopy-small.sql $(BIN)

	cp src/server/phase-server.service $(BIN)/
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

client: src/client/tcpClient.cpp
	mkdir -p $(BIN)
	$(CXX) -std=c++17 src/client/tcpClient.cpp -o $(BIN)/phaseClient -lpcre2-8 -lpthread -lssl -lcrypto -lreadline
	chmod u+x $(BIN)/phaseClient
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

irc: src/client/ircInterface.cpp
	mkdir -p $(BIN)
	$(CXX) -std=c++17 src/client/ircInterface.cpp -o $(BIN)/phaseIrc -lpcre2-8 -lpthread -lssl -lcrypto
	chmod u+x $(BIN)/phaseIrc
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

#UNITTESTS
#-------------------------------------------------------------------------------
test: src/processRelease/parseReleaseTest.cpp	
	mkdir -p $(UNITTESTS)
	$(CXX) src/processRelease/parseReleaseTest.cpp -o $(UNITTESTS)/pr-test $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/pr-test
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

parserelease: src/processRelease/parseRelease_unittest.cpp	
	mkdir -p $(UNITTESTS)
	$(CXX) src/processRelease/parseRelease_unittest.cpp -o $(UNITTESTS)/pr-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/pr-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

cbftp: src/cbftp
	mkdir -p $(UNITTESTS)
	$(CXX) src/cbftp/cbftpInterface_unittest.cpp -o $(UNITTESTS)/ci-ut $(CXXFLAGS)
	$(CXX) src/cbftp/cbftpConfigSetter_unittest.cpp -o $(UNITTESTS)/ccs-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/ci-ut
	chmod u+x $(UNITTESTS)/ccs-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."
	
tvmaze: src/processRelease/lookupTvmaze_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/processRelease/lookupTvmaze_unittest.cpp -o $(UNITTESTS)/lt-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/lt-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

jaro: src/common/jw_ut.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/common/jw_ut.cpp -o $(UNITTESTS)/jw-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/jw-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

imdb: src/processRelease/lookupImdb_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/processRelease/lookupImdb_unittest.cpp -o $(UNITTESTS)/li-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/li-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

checkrulesyntax: src/processRules/checkRuleSyntax_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/processRules/checkRuleSyntax_unittest.cpp -o $(UNITTESTS)/crs-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/crs-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

checkrule: src/processRules/checkRule_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/processRules/checkRule_unittest.cpp -o $(UNITTESTS)/cr-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/cr-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

parsesite: src/processRules/parseSite_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/processRules/parseSite_unittest.cpp -o $(UNITTESTS)/ps-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/ps-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

commandparser: src/commandParser/commandParser_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/commandParser/commandParser_unittest.cpp -o $(UNITTESTS)/cp-ut $(CXXFLAGS) -lssl -lcrypto
	chmod u+x $(UNITTESTS)/cp-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

dbconnection: src/common/dbConnection_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/common/dbConnection_unittest.cpp -o $(UNITTESTS)/dbc-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/dbc-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

featureextractor: src/common/featureExtractor_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/common/featureExtractor_unittest.cpp -o $(UNITTESTS)/fe-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/fe-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

extractname: src/common/extractName_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/common/extractName_unittest.cpp -o $(UNITTESTS)/en-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/en-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

regexparser: src/common/regexParser_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/common/regexParser_unittest.cpp -o $(UNITTESTS)/rp-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/rp-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

regexfunctions: src/common/regexFunctions_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/common/regexFunctions_unittest.cpp -o $(UNITTESTS)/rf-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/rf-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."

release: src/results/release_unittest.cpp
	mkdir -p $(UNITTESTS)
	$(CXX) src/results/release_unittest.cpp -o $(UNITTESTS)/r-ut $(CXXFLAGS)
	chmod u+x $(UNITTESTS)/r-ut
	@echo -e "Compiled "$(red)$<$(green)" successfully!"$(default)
	@echo "..."


clean:
	rm -rf $(BIN)
	rm -f log.txt

install-client:
	cp $(BIN)/phaseClient /usr/bin/

#TODO: cert's should be in different path
install-server:
	adduser --system pri
	mkdir -p /opt/pri/bin/
	cp $(BIN)/phaseServer /opt/pri/bin/phaseServer
	cp $(BIN)/cert.pem /opt/pri/bin/
	cp $(BIN)/key.pem /opt/pri/bin/
	cp $(BIN)/startServer.sh /opt/pri/bin/
	cp $(BIN)/phase-server.service /lib/systemd/system/
	#cp /etc/ssl/openssl.cnf /etc/ssl/openssl.cfg
	systemctl enable phase-server.service
	systemctl start phase-server.service
	@echo
	@echo $(yellow)You can interrupt the log-output with CTRL-C $(green)the server will NOT stop$(default)
	@echo to STOP the server use $(red)systemctl stop phase-server.service$(default)
	@echo
	journalctl -f

uninstall-client:
	rm /usr/bin/phaseClient
	
uninstall-server:
	systemctl stop phase-server.service | /bin/true
	rm -f /opt/pri/bin/phaseServer
	rm -f /opt/pri/bin/cert.pem
	rm -f /opt/pri/bin/key.pem
	rm -f /opt/pri/bin/startServer.sh
	rm -f /lib/systemd/system/phase-server.service
	deluser pri | /bin/true
	

