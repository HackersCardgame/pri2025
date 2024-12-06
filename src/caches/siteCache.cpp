
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

#ifndef SITECACHE
#define SITECACHE

/*******************************************************************************
* holds the data from one site config file in memory that is fast than it would
* load it each time from database (alle siteCaches are stored in 1 Array)
*******************************************************************************/
class SiteCache
{
    
  private:
    inline static bool debug = false;

  public: 
  
    chrono::system_clock::time_point begin;
    chrono::system_clock::time_point end;
    
    string site = "";
    
    unordered_map<string, string> siteName = {};
    unordered_map<string, string> location = {};
    unordered_map<string, string> size = {};
    unordered_map<string, string> isp = {};
    unordered_map<string, string> control = {};

    unordered_map<string, string> siteBans = {};

    unordered_map<string, string> status = {};
    unordered_map<string, string> comments = {};
    
    unordered_map<string, string> affils = {};

    unordered_map<string, string> rule = {};                                    //since phase would like to have similar rules an automatic index will be added
    unordered_map<string, string> dir = {};
    
    //the blacklist = ASDF would give for all sectiosn ASDF would be blacklisted,  blacklist TV = QWER would mean QWER is only blacklisted if section is TV
    unordered_map<string, string> blacklist = {};
    unordered_map<string, string> whitelist = {};  

    unordered_map<string, string> groupblacklist = {};
    unordered_map<string, string> groupwhitelist = {};  

    unordered_map<string, string> sectionblacklist = {};  //to blacklist a complet section
    unordered_map<string, string> sectionwhitelist = {};  //whitelist has higher priority than blacklist, so you can blacklist * and whitelist GAMES
    
    unordered_map<string, string> networkblacklist = {};
    unordered_map<string, string> networkwhitelist = {};
    
    unordered_map<string, unordered_map<string, string>> content =  { { "siteName",         this->siteName },
                                                                      { "location",         this->location },
                                                                      { "size",             this->size },
                                                                      { "isp",              this->isp },
                                                                      { "control",          this->control },
                                                                      { "siteBans",         this->siteBans },
                                                                      { "status",           this->status },
                                                                      { "comments",         this->comments },
                                                                      { "affils",           this->affils },
                                                                      { "rule",             this->rule },
                                                                      { "dir",        this->dir },
                                                                      { "blacklist",        this->blacklist },
                                                                      { "whitelist",        this->whitelist },
                                                                      { "groupblacklist",        this->groupblacklist },
                                                                      { "groupwhitelist",        this->groupwhitelist },
                                                                      { "sectionblacklist", this->sectionblacklist },
                                                                      { "sectionwhitelist", this->sectionwhitelist },
                                                                      { "networkblacklist", this->networkblacklist },
                                                                      { "networkwhitelist", this->networkwhitelist }           };
    
    SiteCache(string siteName)
    {
      if ( SiteCache::debug ) cout << "DEBUG ENABLED, makes it slower" << endl;
      this->begin = std::chrono::high_resolution_clock::now();
      this->site = siteName;
      this->siteName["*"] = siteName;
    }
    
    ~SiteCache()
    {
      this->end = std::chrono::high_resolution_clock::now();
      string text = "~SiteCache()";
      cout << HelperFunctions::sprintTime(text, begin, end);
    }
    
};

#endif
