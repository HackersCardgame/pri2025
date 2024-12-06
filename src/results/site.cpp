
#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>

#include <unordered_map>

#include "../common/helperFunctions.cpp"

#include "../processRules/checkBlacklist.cpp"


using namespace std;


#ifndef SITE
#define SITE

/*******************************************************************************
* is what it says: 
* the central datastorage for one site
* since the results are specific for each release it would not make sense to 
* cache this things
*******************************************************************************/
class Site
{
    
  private:
    inline static const bool debug = false;

  public: 
  
    chrono::system_clock::time_point begin;
    chrono::system_clock::time_point end;
    
    string siteName = "";
    vector<string> affils;
    string matchedRule = "";
    
    array<BlacklistItem,4> blacklist = { none, none, none };

    Site(string siteName)
    {
      if (Site::debug)
        this->begin = std::chrono::high_resolution_clock::now();
      this->siteName = siteName;
    }
    
    ~Site()
    {
      if (Site::debug)
      {
        this->end = std::chrono::high_resolution_clock::now();
        string text = "~Site()";
        cout << HelperFunctions::sprintTime(text, begin, end);
      }
    }
    
};

#endif
