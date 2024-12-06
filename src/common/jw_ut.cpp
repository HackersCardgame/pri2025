

#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "../lib/Jaro-Winkler/cpp/jwdistance.h"

#include "../lib/jarowinkler.c"

#include <cstdlib>
#include <cstdio>
#include <stdio.h>

#include "../lib/colorize/color.hpp"
#include "../common/helperFunctions.cpp"



//TODO: ./li-ut "Alita bATTLE ANGEL" 2019


int main(int argc, char* argv[])
{
  for( int i = 0; i < 100; i++)
  {
    cout << HelperFunctions::similar("black panther", "Black Panther") << " ";
    cout << HelperFunctions::similar("black panther", "Black Panther: Crowning of a New King") << " ";
    cout << HelperFunctions::similar("Black Panther", "black panther") << " ";
    cout << HelperFunctions::similar("Black Panther: Crowning of a New King", "black panther") << endl;
    cout << HelperFunctions::similar("It", "it") << endl;
    cout << HelperFunctions::similar("Siti", "it") << endl;
    cout << HelperFunctions::similar("it", "it") << endl;
  }
  
//  string a = "black panther";
//  string b = "Black Panther";
//  string c = "Black Panther: Crowning of a New King";
  
//  for ( int j = 0; j < 10; j++ )
//    cout << jw_distance(a, b) << " " << jw_distance(b, a) << " " << jw_distance(a, c) << " " << jw_distance(c, a) << endl;
}
