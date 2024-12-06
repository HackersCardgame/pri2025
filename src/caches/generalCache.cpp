
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

#ifndef GENERAL_CACHE
#define GENERAL_CACHE


/*******************************************************************************
* holds the data from general config file in memory that is fast than it would
* load it each time from database
*******************************************************************************/
class GeneralCache
{
    
  private:
    bool debug = false;

  public: 
  
    chrono::system_clock::time_point begin;
    chrono::system_clock::time_point end;
                                                                                // XXX XXX README: READ FROM TOP TO DOWN, and multiple times if you dont understand XXX XXX
                                                                                // this things are done by the featureExtractor.cpp
                                                                                
    unordered_map<string, string> admin = {};                                   // users with admin access
    //admin = user01 user02 ...                                                 // they have access to eg debug *

    unordered_map<string, string> order = {};                                   // the order in which each section shall be processed
    //order MOVIES = SECTION CODEC VIDEOSOURCE SCREENS RESOLUTION LANGUAGE      // in this example the order "MOVIES" has a SECTION, CODEC.... (these things are called Feature)


    unordered_map<string, string> feature = {};                                 // the features, these are things like resolution, section, codec
    //feature SECTION = $GAMES $SPORTS XXX $MOVIES $TV PS4 $DOX $APPS           // in this example the feature SECTION can be Games, Sprots, TV, PS4... 
                                                                                // (the $-Sign referes to a topic, without a $-sign it will only search the release for eg .XXX.
                                                                                
    unordered_map<string, string> topic = {};                                   // the topic are the values that a feature can have
    //topic MOVIES = !$sport !$TVEP !$DATE $progressiveScan|$source|$codec      // in this example the topic MOVIES must have a videoSoruce or a codec or a resolution like p720 
                                                                                // (p stands for progressive Scan) but it must not have a TV-EPISODE !-sign means not, $-sign means a keyword
                                                                                
    unordered_map<string, string> keyword = {};                                 // the topic are the values that a feature can have feature can have
    //keyword TVEP = S\d+E\d+ E\d\d                                             // in this example defines with a regex how a Episode is recognized S01E01
                                                                                // \d means digit 
                                                                                // + means one or more
                                                                                // \d+ means one or more digit, so it matches on S1E1 and on S01E01 or on S123E02
    
    // XXX XXX XXX TO SUMMARIZE: an order -> feature -> topic -> keyword -> regex XXX XXX XXX
    
    unordered_map<string, string> ruletemplate = {};                            // ruletemplates are thigns that you always need and dont want to write individually each time
    //ruletemplate std = EXTERNAL nodupe EN US 2D nfo wide new                  // the std template defines as non-INTERNAL nodupe EN or US with nfo and wide-release
                                                                                // (these refer to topics (see above) 
                                                                                // nfo or nonfo is a topic from the FEATURE IMDB
                                                                                // this will not be processed eg. in GAMES since IMDB is not a feature of GAMES

    //the blacklist = ASDF would give for all sectiosn ASDF would be blacklisted,  blacklist TV = QWER would mean QWER is only blacklisted if section is TV
    unordered_map<string, string> blacklist = {};
    unordered_map<string, string> whitelist = {};  

    unordered_map<string, string> groupblacklist = {};
    unordered_map<string, string> groupwhitelist = {};  

    unordered_map<string, string> sectionblacklist = {};  //to blacklist a complet section
    unordered_map<string, string> sectionwhitelist = {};  //whitelist has higher priority than blacklist, so you can blacklist * and whitelist GAMES
    
    unordered_map<string, string> networkblacklist = {};
    unordered_map<string, string> networkwhitelist = {};
    
    unordered_map<string, string> replacetvmaze = {};
    unordered_map<string, string> replaceimdb = {};
    unordered_map<string, string> separator = {};
    unordered_map<string, string> screenoverridegroups = {};

    unordered_map<string, unordered_map<string, string>> content =  { { "admin",                this->admin },
                                                                      { "order",                this->order },
                                                                      { "feature",              this->feature },
                                                                      { "topic",                this->topic },
                                                                      { "keyword",              this->keyword },
                                                                      { "ruletemplate",         this->ruletemplate },
                                                                      { "blacklist",            this->blacklist },
                                                                      { "whitelist",            this->whitelist },
                                                                      { "groupblacklist",            this->groupblacklist },
                                                                      { "groupwhitelist",            this->groupwhitelist },
                                                                      { "sectionblacklist",     this->sectionblacklist },
                                                                      { "sectionwhitelist",     this->sectionwhitelist },
                                                                      { "networkblacklist",     this->networkblacklist },
                                                                      { "networkwhitelist",     this->networkwhitelist },
                                                                      { "replacetvmaze",        this->replacetvmaze },
                                                                      { "replaceimdb",          this->replaceimdb },
                                                                      { "separator",            this->separator },
                                                                      { "screenoverridegroups", this->screenoverridegroups }   };

    
    GeneralCache()
    {
      if ( GeneralCache::debug ) cout << "DEBUG ENABLED, makes it slower" << endl;    
      this->begin = std::chrono::high_resolution_clock::now();
    }
    
    ~GeneralCache()
    {
      this->end = std::chrono::high_resolution_clock::now();
      string text = "~GeneralCache()";
      cout << HelperFunctions::sprintTime(text, begin, end);
    }
    
};

#endif

