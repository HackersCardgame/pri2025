#!/bin/bash
#sudo apt-get install valkyrie valgrind vim-gtk3
#g++ -std=c++17 lookupImdb_unittest.cpp -o li-ut `mariadb_config --cflags --libs` -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#./li-ut
#g++ -std=c++17 parseRelease_unittest.cpp -o pr-ut `mariadb_config --cflags --libs` -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#./pr-ut
#g++ -std=c++17 dbConnection_Unittest.cpp -o ut `mariadb_config --cflags --libs` -g -Wall
#g++ -std=c++17 dbConnection.cpp -o dbConnection `mariadb_config --cflags --libs` -Wall
#g++ -std=c++17 dbConnection.cpp -o dbConnection `mariadb_config --cflags --libs` -Wall
g++ -std=c++17 lookupTvmaze_unittest.cpp -o tl-ut `mariadb_config --cflags --libs` -g -lpcre2-8 -lpcre2-16 -lpcre2-32 -Wall
#TODO valkyrie pr-ut
