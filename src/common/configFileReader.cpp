
// ==============================================================================
//                         GENERAL PARSING FUNCTION
// ------------------------------------------------------------------------------
//    parse(stringToParse [AND|OR]) ( Rule1 AND|OR Rule2 AND|OR Rule2 ... )  
//
//                  YOU SHOULD NOT CHANGE ANYTHING IN HERE 
//      -----------------------------------------------------------------
//              except you REALLY REALLY know what you are doing
//
// ------------------------------------------------------------------------------
//                             Dear Programmer:
//                    When I wrote this code, only god and
//                           I know how it worked.
//                          Now, only god knows it!
//
//                  Therefore, if you are trying to "optimize"
//                   this routine and it fails (most surely),
//                        warning for the next person:
//
//#define total_hours_wasted_here = 530
//
//                Please update this counter if you wasted time here
//
// ==============================================================================


#include <time.h>
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "../common/regexFunctions.cpp"
#include "../common/helperFunctions.cpp"

#include "../common/dbConnection.cpp"

#include "../caches/generalCache.cpp"
#include "../caches/userCache.cpp"
#include "../caches/siteCache.cpp"

using namespace std;

#ifndef CONFIG_FILE_READER
#define CONFIG_FILE_READER

/*******************************************************************************
* reads the config files from the database. database is generated with python3
* scripts that reside in this repository under /tools/install
* it initially loads the whole configuration database and stores it in
* caches /src/caches/...
*
* so if you change things in the database it will become active if you run
* buildCache()
* 
* if you change things here dont forget the locks / lock_guard
*******************************************************************************/
class ConfigFileReader
{
    inline static const bool debug = false;
    

  private:
    inline static string year = "19\\d\\d|20\\d\\d";

    chrono::system_clock::time_point begin;
    chrono::system_clock::time_point end;
    
    static std::atomic<ConfigFileReader*> instance;
    static std::mutex configFileReaderMutex;

    
    DbConnection* dbConnection;
    
  public:
    inline static const string configTable = "config2";

    //string configFileExtension = ".cfg"; 

    //XXX: COMMENT
    inline static vector<string> verb = { "add", "del", "set", "append", "remove", "mod" };  //TODO: mod
    
    //TODO: use here the combinedCommands vector
    //inline static vector<string> submodifier = { "language", "tvid", "network", "group" };

    //XXX MAINLY FOR THE INTERFACE
    
    inline static vector<string> availabeCommands = { "test", "login", "imdb", "tvmaze", "help", "debug", "whoami", "who", "sync", "user", 
                                                      "site", ".site", "contains", "section", "history", "ll", "tl", "exit", "autocomplete" };

    inline static vector<string> userWithModifier = { "blacklist", "whitelist", "groupblacklist", "groupwhitelist",
                                                      "sectionblacklist", "sectionwhitelist", "networkblacklist", "networkwhitelist", "active" /*TODO: change cfg form site to active"*/ };
    inline static vector<string> userWithoutModifier = { "password", "cbpassword", "port", "host", "setpassword" };

    inline static vector<string> siteWithModifier = { "rule", "dir", "blacklist", "whitelist", "groupblacklist", "groupwhitelist",
                                                      "sectionblacklist", "sectionwhitelist", "networkblacklist", "networkwhitelist" };
                                                              
    inline static vector<string> siteWithoutModifier = { "name", "location", "country", "size", "isp", "control", /*TODO "sitebans",*/ "status", "affils", /* "contains", "section" */ };
    
    inline static vector<string> allCommands = {};

    //XXX MAINLY FOR THE CONFIGFILE    
    vector<string> generalDicts = { "order", "feature", "topic", "keyword", "ruletemplate", "replacetvmaze", "replaceimdb", "separator", "screenoverridegroups", 
                                    "blacklist" ,"whitelist", "sectionblacklist", "sectionwhitelist", "networkblacklist", "networkwhitelist", "admin"  };


    vector<string> userDicts = { "blacklist" ,"whitelist", "groupblacklist" ,"groupwhitelist", "sectionblacklist", "sectionwhitelist", "networkblacklist",
                                 "networkwhitelist", "host", "port", "password", "cbpassword", "site" };    // "screenoverridegroups" 
    
    vector<string> siteDicts = { "dir", "rule", "affils", "name", "status", "comments", "blacklist", "whitelist", "groupblacklist", "groupwhitelist",
                                 "sectionblacklist", "sectionwhitelist", "networkblacklist", "location", "country", "size",
                                 "isp", "control", "sitebans",  "networkwhitelist" };
                                 
    vector<string> validCommands = { };

    //all the combined commands like airdate:2018-03-01 or tvid:(63,1535) or network:(amazon,netflix)
    vector<string> combinedCommands = { "group", "airdate", "episodeage", "tvid", "language", "network", "premiere", 
                                        "airdate", "episodeAge", "lrating", "lvotes", "rating", "votes", "year", "filter" };
    
    vector<string> users = { };
    vector<string> activeUsers = { };
    vector<string> sites = { };

    GeneralCache generalCache;
    unordered_map<string, UserCache*> userCaches = {};
    unordered_map<string, SiteCache*> siteCaches = {};

    static ConfigFileReader* getInstance(){
      ConfigFileReader* sin = instance.load();
      if ( !sin ){
        std::lock_guard<std::mutex> myLock(configFileReaderMutex);
        sin= instance.load();
        if( !sin ){
          sin= new ConfigFileReader();
          instance.store(sin);
        }
      }
      return sin;
    }
  

    ConfigFileReader()
    {
      this->begin = std::chrono::high_resolution_clock::now();
      this->dbConnection = DbConnection::getInstance();
      buildCache();
      buildCommandList();
    }

    //Destructor        
    ~ConfigFileReader()
    { 
      this->end = std::chrono::high_resolution_clock::now();
      string text = "~ConfigFileReader()";
      cout << HelperFunctions::sprintTime(text, begin, end);
    }  


    void getUsers()
    {
      string sqlQuery = "SELECT distinct(configFile) from " + ConfigFileReader::configTable + " where parameter like \"host\";";
      MYSQL_RES* result = this->dbConnection->fetch(sqlQuery);
      ARRAY_WITH_SIZE array = this->dbConnection->toArray(result);

      //cout << "userCount: " << array.rows << " " << array.cols << endl;
      for ( int i = 0; i < array.rows; i++)
      {
        //cout << array.array[i][0] << endl;
        this->users.push_back(array.array[i][0]);
      }
      mysql_free_result(result);
      this->dbConnection->freeArray(array.array, array.rows, array.cols);
    }
    
    void getSites()
    {
      string sqlQuery = "SELECT distinct(configFile) from " + ConfigFileReader::configTable + " where parameter like \"rule\";";
      MYSQL_RES* result = this->dbConnection->fetch(sqlQuery);
      ARRAY_WITH_SIZE array = this->dbConnection->toArray(result);

      for ( int i = 0; i < array.rows; i++)
      {
        //cout << array.array[i][0] << endl;
        this->sites.push_back(array.array[i][0]);
      }
      mysql_free_result(result);
      this->dbConnection->freeArray(array.array, array.rows, array.cols);
    }
    
    
    void loadConfig(string configFile, vector<string> configItems, unordered_map<string, unordered_map<string, string>> &content)
    {
      if (ConfigFileReader::debug) cout << "loadConfig " << configFile << endl;
      for (auto element : configItems)
      {
        if (ConfigFileReader::debug && configFile == "general.cfg") cout << "element " << element << endl;
        string sqlQuery = "SELECT modifier, term from " + ConfigFileReader::configTable + " where parameter like \"" + element + "\" and configFile like \"" + configFile + "\";";
        MYSQL_RES* result = this->dbConnection->fetch(sqlQuery);
        ARRAY_WITH_SIZE array = this->dbConnection->toArray(result);
        for ( int i = 0; i < array.rows; i++)
        {
          if (ConfigFileReader::debug) cout << element << " -> " << array.array[i][0] << ": " << array.array[i][1] << endl;
          if (array.array[i][0][0] == '\0')
          {
            content[element]["*"] = RegexFunctions::replace("[ ]+", " ", array.array[i][1]);  //XXX: remove doublespaces  //TODO: upper / lower case
          }
          else
          {
            content[element][array.array[i][0]] = RegexFunctions::replace("[ ]+", " ", array.array[i][1]);
          }
        }
        mysql_free_result(result);
        this->dbConnection->freeArray(array.array, array.rows, array.cols);
      }
    }
    
    
    //TODO: comment
    void getValidCommands()
    {
      vector<string> topicsVector = {};
      string orderLine = this->generalCache.content["order"]["all"];

      vector<string> features = RegexFunctions::split(' ', orderLine);
      for ( auto feature : features )
      {
        string topicsLine = "";
        if ( feature.substr(0,1) == "*" )
          topicsLine = this->generalCache.content["feature"][feature.substr(1)];
        else
          topicsLine = this->generalCache.content["feature"][feature];
        vector<string> topics = RegexFunctions::split(' ', topicsLine);
        for ( auto topic : topics )
        {
          if ( topic.substr(0,1) == "$" )
          {
            if ( ConfigFileReader::debug) cout << topic.substr(1) << endl;
            topicsVector.push_back(topic.substr(1));
          }
          else
          {
            if ( ConfigFileReader::debug) cout << topic << endl;
            topicsVector.push_back(topic);
          }
        }
      }
      for ( auto rt : this->generalCache.content["ruletemplate"] )
        topicsVector.push_back(rt.first);
      this->validCommands = topicsVector;
      
      this->validCommands.push_back("currentseason");                           //TODO: ugly
    }



       
    void loadGeneral()
    {
      loadConfig("general.cfg", generalDicts, this->generalCache.content);
    }


    void loadUsers()
    {
      for(auto user : this->users)
      {
        this->userCaches.insert(make_pair(user, new UserCache(user)));
        loadConfig(user, userDicts, this->userCaches[user]->content);
      }
    }

    void loadSites()
    {
      for(auto site : this->sites)
      {
        this->siteCaches[site] = new SiteCache(site);  //TODO: Multithreading
        loadConfig(site, siteDicts, this->siteCaches[site]->content);
      }
    }

    string buildCacheAsync()
    {
      thread buildCache;
      return "rebuilding cache async";
    }

    string buildCache()
    {

      //reset all
      this->users = { };
      this->sites = { };
      this->generalCache = GeneralCache();
      this->userCaches = {};
      this->siteCaches = {};
      this->validCommands = {};
    
      //set all
      getUsers();
      getSites();
      loadGeneral();
      loadUsers();
      loadSites();
      getValidCommands();
      
      return "cache rebuilt\n";
    }
    
    
    void buildCommandList()
    {
      this->allCommands = { };
      
      this->allCommands.reserve( ConfigFileReader::availabeCommands.size() + 
                                 ConfigFileReader::userWithModifier.size() +
                                 ConfigFileReader::userWithoutModifier.size() +
                                 ConfigFileReader::siteWithModifier.size() +
                                 ConfigFileReader::siteWithoutModifier.size() +
                                 ConfigFileReader::verb.size()  ); // preallocate memory
                   
      this->allCommands.insert( this->allCommands.end(), ConfigFileReader::availabeCommands.begin(), ConfigFileReader::availabeCommands.end() );
      this->allCommands.insert( this->allCommands.end(), ConfigFileReader::userWithModifier.begin(), ConfigFileReader::userWithModifier.end() );
      this->allCommands.insert( this->allCommands.end(), ConfigFileReader::userWithoutModifier.begin(), ConfigFileReader::userWithoutModifier.end() );
      this->allCommands.insert( this->allCommands.end(), ConfigFileReader::siteWithModifier.begin(), ConfigFileReader::siteWithModifier.end() );
      this->allCommands.insert( this->allCommands.end(), ConfigFileReader::siteWithoutModifier.begin(), ConfigFileReader::siteWithoutModifier.end() );
      this->allCommands.insert( this->allCommands.end(), ConfigFileReader::verb.begin(), ConfigFileReader::verb.end() );
    
    }
    

};

std::atomic<ConfigFileReader*> ConfigFileReader::instance;
std::mutex ConfigFileReader::configFileReaderMutex;

#endif
