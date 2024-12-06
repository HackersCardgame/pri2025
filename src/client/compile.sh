#g++ -std=c++17 dbConnection.cpp -o dbConnection `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 dbConnection_unittest.cpp -o dbc_ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 extractName.cpp -o en `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 regexFunctions_unittest.cpp -o rf `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 extractName_unittest.cpp -o en-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 regexParser_unittest.cpp -o rp-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#./rp-ut
#g++ -std=c++17 configFileReader_unittest.cpp -o cfr-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 configFileReader_unittest.cpp -o cfr-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 featureExtractor_unittest.cpp -o fe-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#./cfr-ut
#g++ -std=c++17 dbConnection.cpp -o dbConnection `mariadb_config --cflags --libs` -Wall
#g++ -std=c++17 dbConnection.cpp -o dbConnection `mariadb_config --cflags --libs` -Wall
#g++ -std=c++17 dateFunctions_unittest.cpp -o df-ut `mariadb_config --cflags --libs` -g -3g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 tcpClient.cpp -o tcpClient -lreadline `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -lreadline -Wall
#clang++-6.0 -std=c++17 tcpClientNew.cpp -o tcpClient -lreadline -I/usr/include/mysql -I/usr/include  -L/usr/lib64/mysql -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -lreadline -lpthread -Wall -lssl -lcrypto
#g++ -std=c++17 tcpClient.cpp -o phaseClient -lreadline -I/usr/include/mysql -I/usr/include  -L/usr/lib64/mysql -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -lreadline -lpthread -Wall -lssl -lcrypto
#g++ tcpServer.cpp -o ts
#g++ term4.cpp -lreadline


#yum provides mariadb_config  -> NO unfortunately not
#MySQL flags: -I/usr/include/mysql
#MySQL libs: -L/usr/lib64  -lmariadb -lpthread -lz -ldl -lm -lssl -lcrypto

g++ -std=c++17 ircInterface.cpp -o phaseIrc -lpthread -Wall -lssl -lcrypto

