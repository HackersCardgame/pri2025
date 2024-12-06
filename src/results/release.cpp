
#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "../lib/colorize/color.hpp"

#include "../common/helperFunctions.cpp"
#include "../common/dateTimeFunctions.cpp"

#include "site.cpp"
#include "user.cpp"

using namespace std;

#ifndef RELEASE
#define RELEASE

/*******************************************************************************
* is what it says: 
* the central datastorage for one incoming release (rls)
* also the site results and user results are attached here, since the results
* are specific for each release it would not make sense to cache this things
* also the logfile for a specific release is stored here, and here you can also
* adjust if a release logs in or not, just do a rls.login = true to login
*******************************************************************************/
class Release
{

  private:
    inline static bool debug = false;
    inline static string arrowLeft = " \033[0;36m- \033[0;37m>\033[0;36m>\033[1;36m>\033[0m ";
    inline static string arrowRight = " \033[1;36m<\033[0;36m<\033[0;37m<\033[0;36m -";
    
  public: 
  
  
    chrono::system_clock::time_point begin;
    chrono::system_clock::time_point end;
  
    inline static int tabspace = 30;
    
    bool login = false;
    bool overrideDupeCheck = false;
    bool log = false;
    bool allErrors = false;
    
    string logfile = "";  //TODO

    
    string releaseName = "";
    string releaseNameOriginal = "";
    string content = "";
    string releaseInfo = "";
    string group = "";
    string withoutGroup = "";
    string releaseYear = "0";
    string section = "";

    unordered_map<string, string> features = {};                                // unordered is faster   eg. feature SECTION = $GAMES $SPORTS XXX...

    string imdbTconst = "";                                                     //the tconst id from IMDB Basic / Akas Table
    string titleType = "";
    string imdbPrimaryTitle = "";                                               //the Primary  Title from IMDB Basic Table
    string imdbOriginalTitle = "";                                              //the Original Title from IDMB Basic Table
    string imdbYear = "";                                                       //the movie Year concerning IMDB
    string imdbGenre = "";                                                      //the genre like Action, Documentary...
    float imdbRating = 0.0;                                                     //the rating from imdb
    int imdbVotes = 0;                                                          //How many people voted
    string tmdbLanguage = "";                                                   //originalLanguage from TMDB

    int mojoUsScreens = 0;                                                      //the max number of cinemas running the movie at the same time in US
    int mojoUkScreens = 0;                                                      //the max number of cinemas running the movie at the same time in UK

    string episode = "";                                                        //SEASON:EPISODE, eg. S01E06
    string season = "";

            
    string tvmazeID = "";                                                       //the show ID from TVMAZE, eg 66 for The Big Bang Theory
    string tvmazeTitle = "";                                                    //the title from TVMAZE
    string tvmazeYear = "1900";

    string tvmazeType = "";
    string tvmazeLanguage = "";
    string tvmazeGenre = "";
    string tvmazeStatus = "";
    string tvmazeCountry = "";
    string tvmazeCurerntSeason = "";

    tm tvmazeAirdatePremier;                                                    //time_t VS time_point???
    string tvmazeNetwork = "";
    string tvmazeNetworkWeb = "";
    tm tvmazeAirdateEpisode;


    vector<string> dupes;
    
    vector<string> sites;                                                       // all available sites
    
    vector<string> users;                                                       // all available users

    unordered_map<string, Site> siteResults = {};                               // all processed siteResults

    unordered_map<string, User> userResults = {};                               // all processed userResults

          
    Release(string releaseName)
    {
      if ( Release::debug ) cout << "DEBUG ENABLED, makes it slower" << endl;
      this->begin = std::chrono::high_resolution_clock::now();
      this->releaseName = releaseName;
    }
    
    ~Release()
    {
      this->end = std::chrono::high_resolution_clock::now();
      string text = "~Release("+ this->releaseName+") -> " + this->section;
      cout << HelperFunctions::sprintTime(text, begin, end);
    }
          

    string sprint()
    {
      return sprint("all");
    }
                                                   
    string sprint(string userName)
    {
      
      string output = "";
      output += createLine("release", "\""+this->releaseNameOriginal+"\"", "Default", "Cyan");
      output += createLine("-->", this->withoutGroup+" -" + this->group , "Default", "yellow");
      
      output += createLine("content", "\""+this->content+"\"", "Default", "Default");
      output += createLine("info/year", this->releaseInfo + " / " + this->releaseYear, "Default", "Default");
      this->end = std::chrono::high_resolution_clock::now();
      string time = HelperFunctions::sprintTime("=>", begin, end);
      output += createLine("time", time, "Default", "Default");
      output += createLine("dupeType", this->features["dupe"], "Default", "Default");
      output += createLine("SECTION", this->section, "Default", "Green");
      
      if ( this->features["section"] == "movies" )
      {
        output += "\n";
        output += createLine("imdb-info tconst", this->imdbTconst + " ("+ this->imdbYear + ")", "Default", "Default");
        output += createLine("imdb-info title", this->imdbPrimaryTitle + " / " + this->imdbOriginalTitle , "Default", "Default");
        output += createLine("imdb-info titleType", this->titleType, "Default", "Default");
        stringstream ss;
        ss << std::fixed << setprecision(1) << this->imdbRating;
        string rating = ss.str();
        output += createLine("imdb-info r/v", rating + " / " + to_string(this->imdbVotes), "Default", "Default");
        output += createLine("imdb-info us/uk", to_string(this->mojoUsScreens) + " / " + to_string(this->mojoUkScreens), "Default", "Default");
        output += createLine("imdb-info language", this->tmdbLanguage, "Default", "Default");
        output += createLine("imdb-info genres", this->imdbGenre, "Default", "Default");
        output += createLine("tv-info", this->releaseName, "Red", "Default");
      }
      if ( this->features["section"] == "tv" )
      {
        output += createLine("TVMAZE", this->tvmazeTitle + " ("+ this->tvmazeID + ")", "Default", "Default");
        output += createLine("TVMAZE Year", this->tvmazeYear, "Default", "Default");
        output += createLine("TVMAZE tvmazeType", this->tvmazeType, "Default", "Default");
        output += createLine("TVMAZE tvmazeLanguage", this->tvmazeLanguage, "Default", "Default");
        output += createLine("TVMAZE tvmazeGenre", this->tvmazeGenre, "Default", "Default");
        output += createLine("TVMAZE tvmazeStatus", this->tvmazeStatus, "Default", "Default");
        output += createLine("TVMAZE tvmazeCountry", this->tvmazeCountry, "Default", "Default");
        output += createLine("TVMAZE tvmazeCurerntSeason", this->tvmazeCurerntSeason, "Default", "Default");
        output += createLine("TVMAZE tvmazeAirdate", DateTimeFunctions::toDateString(this->tvmazeAirdatePremier), "Default", "Default");
        output += createLine("TVMAZE tvmazeNetwork", this->tvmazeNetwork, "Default", "Default");
        output += createLine("TVMAZE tvmazeNetworkWeb", this->tvmazeNetworkWeb, "Default", "Default");
        output += createLine("TVMAZE tvmazeAirdateEpisode", DateTimeFunctions::toDateString(this->tvmazeAirdateEpisode), "Default", "Default");
      }            


      // USER INFO
      for ( auto user : this->userResults )
      {
        if ( userName != "all" && userName != user.second.userName )
          continue;
        output += "\n";
        string line = "";

        line += "All: " + to_string(user.second.allowedSites.size()) + " ";
        line += "Pre: " + to_string(user.second.preSites.size()) + " ";
        line += "Allowed: " + to_string(user.second.allowedSites.size()) + " ";
        line += "Skipped: " + to_string(user.second.allowedSites.size()) ;

        output += createLine(user.second.userName, line, "Cyan", "Default");
        
        line = "";
        for ( auto site : user.second.allSites )
        {
          line += site + " ";
        }

        output += createLine("All", line, "Yellow", "Default");

        line = "";
        for ( auto site : user.second.preSites )
        {
          line += site + " ";
        }

        output += createLine("Pre", line, "Green", "Green");

        line = "";
        for ( auto site : user.second.allowedSites )
        {
          line += site + " ";
        }

        output += createLine("Allowed", line, "Green", "Green");


        line = "";
        for ( auto site : user.second.skippedSites )
        {
          line += site + " ";
        }
        
        output += createLine("Skipped", line, "Red", "Red");
        
      }
      

      // LOG FILE
      
      output += color::rize("LOG:", "Cyan") + "\n";
      output += logfile;


      return output;
    }
    
    
    string createLine(string key, string value, string keyColor, string valueColor)
    {
      int tabWidth = Release::tabspace;
      string result = "";
      result += Release::arrowLeft;
      int klen = key.length();
      //int vlen = value.length();
      result += color::rize(key, keyColor).c_str();
      for(int i = klen; i < tabWidth; i++)
        result += " ";
      result += ": ";
      result += color::rize(value, valueColor).c_str();
      result += "\n";
      return result;
    }
    
    
    
};

#endif

