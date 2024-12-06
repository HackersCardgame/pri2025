#g++ -std=c++17 dbConnection.cpp -o dbConnection `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 dbConnection_unittest.cpp -o ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 extractName.cpp -o en `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 regexFunctions_unittest.cpp -o rf `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 extractName_unittest.cpp -o en-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 regexParser_unittest.cpp -o rp-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#./rp-ut
#g++ -std=c++17 configFileReader_unittest.cpp -o cfr-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 configFileReader_unittest.cpp -o cfr-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 featureExtractor_unittest.cpp -o fe-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#./cfr-ut

g++ -std=c++17 checkRule_unittest.cpp -o cr-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 parseSite_unittest.cpp -o ps-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 parseUser_unittest.cpp -o pu-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#g++ -std=c++17 checkRuleSyntax_unittest.cpp -o crs-ut `mariadb_config --cflags --libs` -g -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall

