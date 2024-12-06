
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

/*******************************************************************************
* is what it says: 
* the central datastorage for one user
* since the results are specific for each release it would not make sense to 
* cache this things
*******************************************************************************/
using namespace std;

#ifndef USER
#define USER

class User
{
    
  private:
    static inline const bool debug = false;


  public: 
  
    chrono::system_clock::time_point begin;
    chrono::system_clock::time_point end;
    
    string userName = "";
    vector<string> allSites;
    vector<string> preSites;
    vector<string> allowedSites;
    vector<string> skippedSites;
    
    unordered_map<string, string> siteCaches = {};
    
    array<BlacklistItem,4> blacklist = { none, none, none };

    
    User(string userName)
    {
      if ( User::debug ) this->begin = std::chrono::high_resolution_clock::now();
      this->userName = userName;
    }
    
    ~User()
    {
      if ( User::debug )
      {
        this->end = std::chrono::high_resolution_clock::now();
        string text = "~User()";
        cout << HelperFunctions::sprintTime(text, begin, end);
      }
    }
};

#endif
