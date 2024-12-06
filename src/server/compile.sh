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
#g++ -std=c++17 dateFunctions_unittest.cpp -o df-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 udpServer_unittest.cpp -o us-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 tcpServer_unittest.cpp -o ts-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 udpServer_unittest.cpp -o udpServer `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall -lpthread -lssl -lcrypto
#g++ -std=c++17 commandParser_unittest.cpp -o cp-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 hashPassword_unittest.cpp -o hp `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall -lpthread -lssl -lcrypto
g++ -std=c++17 commandParser_unittest.cpp -o cpb_ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall -lpthread -lssl -lcrypto
g++ -std=c++17 history_unittest.cpp -o h_ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall -lpthread -lssl -lcrypto


