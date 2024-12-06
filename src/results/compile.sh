#!/bin/bash

g++ -std=c++17 -o rel release_unittest.cpp `mariadb_config --cflags --libs`
./rel
