#g++ -std=c++17 dbConnection.cpp -o dbConnection `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
g++ -std=c++17 dbConnection_unittest.cpp -o dbc_ut `mariadb_config --cflags --libs` -g -lpthread -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
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
#g++ -std=c++17 dateTimeFunctions_unittest.cpp -o df-ut `mariadb_config --cflags --libs` -Wall -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall -O3 -gdwarf-2
#g++ -std=c++17 logger_unittest.cpp -o l-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 jw_ut.cpp -o jw-ut `mariadb_config --cflags --libs` -Wall -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall -O3 -gdwarf-2
#g++ -std=c++17 helperFunctions_unittest.cpp -o hf-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall


#TODO valgrind --leak-check=full --show-reachable=yes ./dbc_ut
