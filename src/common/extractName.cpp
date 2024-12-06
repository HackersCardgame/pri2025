
#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>
#include <unordered_map>



#include "../common/regexFunctions.cpp"
#include "../common/regexParser.cpp"
#include "../common/configFileReader.cpp"

#include "../results/release.cpp"

using namespace std;

#ifndef EXTRACT_NAME
#define EXTRACT_NAME

/*******************************************************************************
* This module extracts the name of a release, for example years (in movies)
* or TV-Episode like S01E01 are the points where it separates the name
* from the info. the name maybe used then later for imdb or tvmaze lookup
*******************************************************************************/
class ExtractName
{
    inline static const bool debug = false;

  private:
    inline static string year = "19\\d\\d|20\\d\\d";  //TODO
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();
    inline static RegexParser* rp = RegexParser::getInstance();
    
  public:

    static string extractName(string release)
    {
      int occurancesYear = RegexFunctions::occurances(year, release);

      string result = "";
//      bool first = true;


      vector<string> parts = RegexFunctions::split('.', release);
      
      int size = parts.size();
      
      int pointer = 0;
      
      for (auto part : parts)
      {
        if ( ExtractName::debug ) cout << cfr->generalCache.content["separator"]["*"] << endl;
        if ( rp->parse(cfr->generalCache.content["separator"]["*"], part, false) )
        {
          occurancesYear -= 1;
          if(occurancesYear < 1)
          {
            if ( pointer > 0)
              break;
          }
        }

        result += part + ".";
        pointer += 1;
        if (pointer == size-1)
          break;
      }

      result = result.substr(0, result.size()-1);

      
      //cout << result << " (content)" << endl;
      
      return result;
    }
    
    static string extractInfo(string release, string content)
    {
      try
      {
        return release.substr(content.size()+1, release.size());
      }
      catch (...)
      {
        return release;
      }
    }
    
    
    static string extractYear(string release)
    {
      vector<string> years = RegexFunctions::findAll(year, release);
      if (years.size() > 0)
        return years.back();
      else
      {
        time_t t = time(NULL);
        tm* timePtr = localtime(&t);
        return to_string(timePtr->tm_year + 1900);
      }
    }
    
    static string extractGroup(string release)
    {
      vector<string> parts = RegexFunctions::split('-', release);
      if ( parts.size() > 1 )
        return parts.back();
        
      parts = RegexFunctions::split('_', release);
      if ( parts.size() > 1 )
        return parts.back();
        
      parts = RegexFunctions::split('.', release);
      if ( parts.size() > 1 )
        return parts.back();
        
      return "GROUP-ERROR";
    }
    
    static string removeGroup(string release, string group)
    {
      try
      {
        return release.substr(0, release.size() - group.size() - 1);
      }
      catch (...)
      {
        return release;
      }
    }
    

};


#endif
