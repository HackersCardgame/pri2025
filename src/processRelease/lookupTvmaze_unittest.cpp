  
#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <future>

#include <cstdlib>
#include <cstdio>
#include <stdio.h>

#include "../lib/Jaro-Winkler/cpp/jwdistance.h"
#include "../lib/colorize/color.hpp"

#include "../common/dbConnection.cpp"

#include "../common/helperFunctions.cpp"

#include "../processRelease/lookupTvmaze.cpp"





int main(int argc, char* argv[])
{
    string title;
    string seasonEpisode;
    int year = 0;
    string country = "US";

    if (argc>2)
    {
      title = argv[1];
      seasonEpisode = argv[2];
      
    }
    else
    {
      title = "the big bang theory";
      seasonEpisode = "s05e04";
      cout << "No parameter given, using " << title << " " << seasonEpisode << endl;
    }

//    DbConnection::getInstance();

    chrono::time_point<chrono::system_clock> begin;
    chrono::time_point<chrono::system_clock> end;

    cout << "get(): " << endl;
    begin = std::chrono::system_clock::now();
    vector<string> tvmazeResult = LookupTvmaze::get(title, seasonEpisode, year, country);
    for ( auto item : tvmazeResult )
      cout << item << endl;
    tvmazeResult = LookupTvmaze::get("", "", 0, "");
    for ( auto item : tvmazeResult )
      cout << item << endl;
    end = std::chrono::system_clock::now();
    string text = "lookupTvmanze::get() " + title + " (" + seasonEpisode + ")";
    cout << HelperFunctions::sprintTime(text, begin, end);


}
