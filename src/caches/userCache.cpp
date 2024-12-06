
#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>

#include <unordered_map>

#include "../common/helperFunctions.cpp"

using namespace std;

#ifndef USER_CACHE
#define USER_CACHE

/*******************************************************************************
* holds the data from user config file in memory that is fast than it would
* load it each time from database (all userCaches are aggregated in one array)
*******************************************************************************/
class UserCache
{
    
  private:
    inline static bool debug = false;

  public: 
  
    chrono::system_clock::time_point begin;
    chrono::system_clock::time_point end;
    
    unordered_map<string, string> userName = {};
    unordered_map<string, string> host = {};
    unordered_map<string, string> port = {};
    unordered_map<string, string> password = {};
    unordered_map<string, string> cbpassword = {};

    //maybe tvcountry and tvgenre but is not really a user specific thing, and whitelisting does also not speed up things

    //the blacklist = ASDF would give for all sectiosn ASDF would be blacklisted,  blacklist TV = QWER would mean QWER is only blacklisted if section is TV
    unordered_map<string, string> blacklist = {};
    unordered_map<string, string> whitelist = {};  

    unordered_map<string, string> groupblacklist = {};
    unordered_map<string, string> groupwhitelist = {};  

    unordered_map<string, string> sectionblacklist = {};  //to blacklist a complet section
    unordered_map<string, string> sectionwhitelist = {};  //whitelist has higher priority than blacklist, so you can blacklist * and whitelist GAMES
    
    unordered_map<string, string> networkblacklist = {};
    unordered_map<string, string> networkwhitelist = {};
    
    unordered_map<string, unordered_map<string, string>> content ={ { "userName",         this->userName },
                                                                      { "host",             this->host },
                                                                      { "port",             this->port },
                                                                      { "password",         this->password },
                                                                      { "cbpassword",         this->cbpassword },
                                                                      { "blacklist",        this->blacklist },
                                                                      { "whitelist",        this->whitelist },
                                                                      { "groupblacklist",        this->groupblacklist },
                                                                      { "groupwhitelist",        this->groupwhitelist },
                                                                      { "sectionblacklist", this->sectionblacklist },
                                                                      { "sectionwhitelist", this->sectionwhitelist },
                                                                      { "networkblacklist", this->networkblacklist },
                                                                      { "networkwhitelist", this->networkwhitelist }           };
    
    UserCache(string userName)
    {
      if ( UserCache::debug ) cout << "DEBUG ENABLED, makes it slower" << endl;
      this->begin = std::chrono::high_resolution_clock::now();
      this->userName["*"] = userName;
    }
    
    ~UserCache()
    {
      this->end = std::chrono::high_resolution_clock::now();
      string text = "~UserCache()";
      cout << HelperFunctions::sprintTime(text, begin, end);
    }
};

#endif

