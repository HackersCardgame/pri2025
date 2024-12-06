
#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "../lib/Jaro-Winkler/cpp/jwdistance.h"
#include "../common/dbConnection.cpp"
#include "./lookupImdb.cpp"
#include <mysql.h>

#include <future>


#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <mysql.h>

#include "../lib/colorize/color.hpp"
#include "../common/helperFunctions.cpp"



//TODO: ./li-ut "Alita bATTLE ANGEL" 2019


int main(int argc, char* argv[])
{
    string title;
    int year;


    if (argc>2)
    {
      title = argv[1];
      std::istringstream iss (argv[2]);
      iss >> year;
    }
    else
    {
      cout << "no argv" << endl;
      title = "black panther";
      year = 2018;
    }

    DbConnection::getInstance();

    chrono::time_point<chrono::system_clock> begin;
    chrono::time_point<chrono::system_clock> end;

    cout << "getBasics(): " << endl;
    begin = std::chrono::system_clock::now();
    vector<string> movie = LookupImdb::get(title, year);
    for ( auto m : movie )
      cout << m << endl;
    end = std::chrono::system_clock::now();
    string text = "getBasics() " + title + " (" + to_string(year) + ")";
    cout << HelperFunctions::sprintTime(text, begin, end);

/*
    cout << "getAkas(): " << endl;
    begin = std::chrono::system_clock::now();    
    result = LookupImdb::getAkas(title, year);
    movie = LookupImdb::getImdbVector(result, title);
    end = std::chrono::system_clock::now();
    text = "getAkas() " + title + " (" + to_string(year) + ")";
    cout << HelperFunctions::sprintTime(text, begin, end);
*/
/*
    cout << "getBasicsAkas(): " << endl;
    begin = std::chrono::system_clock::now();
    vector<string> movie = LookupImdb::get(title, year);
    end = std::chrono::system_clock::now();
    string text = "getBasics() " + title + " (" + to_string(year) + ")";
    cout << HelperFunctions::sprintTime(text, begin, end);
*/
}


/*


#coding=utf-8


class LookupImdb:

  akasHD = "title_akas_tsv"
  basicsHD = "title_basics_tsv"
  ratingsHD = "title_ratings_tsv"
  tmdb = "tmdb" #make seperate mem table?

  akasMEM = "akas"
  basicsMEM = "basics"
  ratingsMEM = "ratings"

  akas = "akas"
  basics = "basics"
  ratings = "ratings"



  movieNames = [[ "The Matrix", 1999, True, "['tt0133093', 'The Matrix', 'The Matrix', '1999'"],
                [ "The Matrix revolut", 2003, True, "['tt0242653', 'The Matrix Revolutions', 'The Matrix Revolutions', '2003'," ],
                [ "Krieg der Sterne", 1977, True, "['tt0076759', 'Krieg der Sterne', 'Star Wars', '1977'," ],
                [ "Life Of The Party", 2018, True, "['tt5619332', 'Life of the Party', 'Life of the Party', '2018', " ],
                [ "23rd Psalm Branch", 1969, True, "['tt6391052', '23rd Psalm Branch', '23rd Psalm Branch', '1968'," ],
                [ "Slaughterhouse Rulez", 2018, True, "['tt6905696', 'Slaughterhouse Rulez', 'Slaughterhouse Rulez', '2018', "],
                #these are tests for the akas database since "the matrix" is written in frencht (la matrice)
                [ "La matrice", 1982, True, "['tt8205972', \"De la matrice à l'asile\", \"De la matrice à l'asile\", '1982'," ],
                [ "La matrice", 1995, True, "[None, None, None, None, None, None, None]" ],
                [ "La matrice", 1998, True, "['tt0133093', 'La matrice', 'The Matrix', '1999',"],
                [ "3 Idiots", 2009, True, "['tt1187043', '3 Idiots', '3 Idiots', '2009'," ],
                [ "3 Idiots", 2017, True, "['tt3685624', '3 Idiotas', '3 Idiotas', '2017', " ],
                [ "Ident", 1990, True, "THIS ONE DOES NOT WORK BECAUSE WE DONT HAVE THE SHORT MOVIES IN THE MEMORY COPY" ],              #TODO: is that ok?
                [ "101 Dalmatians 2", 2003, True, "['tt0324941', \"101 Dalmatians 2: Patch's London Adventure\", \"101 Dalmatians II: Patch's London Adventure\", '2002'"],
                [ "IT", 2017, True, "['tt1396484', 'It', 'It', '2017'," ],
                [ "IT", 1990, True, "DOES NOT WORK BECAUSE ITS NOT A MOVIE BUT A Mini-Seris" ]
               ]

*/
