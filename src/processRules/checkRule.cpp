
#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>
#include <unordered_map>




#include "../common/configFileReader.cpp"
#include "../common/dateTimeFunctions.cpp"


#include "../results/release.cpp"

using namespace std;

#ifndef CHECK_RULE
#define CHECK_RULE

/*******************************************************************************
* does what it says: 
* checks the Rule from a site, there is a unittest for this function that
* helps to understand how it works
*******************************************************************************/
class CheckRule
{
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();

  private:
    inline static const bool debug = false;
    
  public:

    static string checkRule(string site, string rule, string term, Release &rls)
    {
      if ( CheckRule::debug ) cout << "checkRule()" << endl;
      for ( auto rt : cfr->generalCache.content["ruletemplate"] )               //std = 1080p external new
      {
        //cout << " " + rt.first +  " " + term + " " + rt.second << endl;
        if ( RegexFunctions::match(" " + rt.first + " ", " " + term + " ") )
        {
          //cout << "original: " << term << endl;
          term = RegexFunctions::replace(" " + rt.first + " ", " " + rt.second + " ", " " + term + " ");            //TV std 720p airdate:2019-03-03
          term = RegexFunctions::trim(term);
          //cout << "replaced: " << term << endl;

        }                                                                       //TV 1080p external new 720p airdate:2019-03-03
      }
      if (!checkFeatures(site, rule, term, rls) )
      {
        return "*";
      }

      if ( !checkCombinedItems(site, rule, term, rls) )
        return "*";

      return rule;      
    }
    
    
    //loads the feature list (order) from config (sqq) and calls checkItems()
    static bool checkFeatures(string site, string rule, string term, Release &rls)
    {
      if ( CheckRule::debug ) cout << "checkFeature()" << endl;

      vector<std::string> termList = RegexFunctions::split(' ', term);
      
      string order = cfr->generalCache.content["order"][rls.features["section"]];
      
      if ( CheckRule::debug ) cout << color::rize(order, "Yellow") << endl;

      vector<std::string> orderList = RegexFunctions::split(' ', order);

      for ( auto feature : orderList )
      {
        if ( CheckRule::debug ) cout << feature << endl;
        if ( feature.substr(0,1) == "*" )
          feature = feature.substr(1);

        if ( !checkItems(site, rule, term, feature, termList, rls) )
        {
          if ( CheckRule::debug ) cout << site << " " << rule << " " << term << " " << color::rize(feature, "Yellow") << " NOT FOUND" << endl;
          return false;
        }

      }
      return true;
    }
      

    //checks all items from a given feature (features and items are laoded from config (sql)
    static bool checkItems(string site, string rule, string term, string feature, vector<string> termList, Release &rls)
    {
      if ( CheckRule::debug ) cout << "checkFeature()" << endl;

      string featureLine = cfr->generalCache.content["feature"][feature];
      vector<std::string> featureList = RegexFunctions::split(' ', featureLine);

      int topicsInRuleTerm = 0;
      
      for ( auto topic : featureList )
      {
        if ( topic.substr(0,1) == "$" )
          topic = topic.substr(1);

        for ( auto termItem : termList )
        {
          if ( topic == termItem )
          {
            topicsInRuleTerm++;                                               //topics from the same feature in the rule

            if ( rls.features[feature] == termItem )
            {
              if ( CheckRule::debug ) cout << color::rize("FOUND ", "Green") << site << " " << rule << " " << term << " --> " << color::rize(termItem, "White", "Green") << endl;
              return true;
            }
          }
        }
      }
      
      if ( topicsInRuleTerm == 0 )                                            //if no topic of that feature in rule then we dont check
        return true;
      else
      {
        if ( CheckRule::debug ) cout << color::rize("FAILED ", "Red") << site << " " << rule << " " << term << color::rize(" Feature ", "Yellow") << feature << " failed: " << color::rize(rls.features[feature], "White", "Red") << endl;
        return false;
      }
    }
    
    //function that checks if eg year:2013 or episodeage:3
    static bool checkCombinedItems(string site, string rule, string term, Release &rls)
    {
      if ( CheckRule::debug ) cout << "checkCombinedItems()" << endl;

      bool itemFailed = false;
      vector<string> termList = RegexFunctions::split(' ', term);
      for ( auto termItem : termList )
      {
        vector<string> parts = RegexFunctions::split(':', termItem);
        if ( parts.size() == 2 )                                                //tvid:83,87,107,315,112,216,522,17078 language:English,German premiere:2010 
        {                                                                       //episodeAge:3 network:amazon,netflix,youtube,youtube-premium group:flt,codex
          vector<string> values = RegexFunctions::split(',', parts[1]);
          string command = parts[0];
          itemFailed = itemFailed || !checkCombinedCommand(command, term, values, rls);
          if ( itemFailed )
            return false;
        }
        if ( parts.size() == 1 )                                                //tvid:83,87,107,315,112,216,522,17078 language:English,German premiere:2010 
        {
          if ( parts[0] == "currentseason" )
          {
            if ( CheckRule::debug ) cout << parts[0] << " rls: " << rls.season << " TVMAZE: " << rls.tvmazeCurerntSeason << endl;

            int rlsSeason = 1;
            int tvmazeSeason = 1;
            
            try { rlsSeason = stoi(rls.season); } catch (...) { cout << "could not stoi rls.season"; }
            try { tvmazeSeason = stoi(rls.tvmazeCurerntSeason); } catch (...) { cout << "could not stoi rls.season"; }
            
            if ( rlsSeason >= tvmazeSeason )
            {
              if ( CheckRule::debug ) 
                cout << "TvMAZE: Correct Season" << endl;
            }
            else
              return false;
          }
        }
      }
      return true;
    }

    //check a combined command like year: group: ...
    static bool checkCombinedCommand(string command, string term, vector<string> values, Release &rls)
    {
      //GENERAL STUFF
      if ( command == "group" )
      {
        for ( auto value : values )
        {
          if ( value == rls.group )
            return true;
        }
        if ( CheckRule::debug ) cout << "group not found " << endl;
        return false;
      }


      if ( command == "filter" )                   
      {
        for ( auto value : values )
        if ( RegexFunctions::match(value, rls.releaseNameOriginal) )
          return false;
        return true;
      }


      //TVMAZE STUFF
      if ( command == "tvid" )
      {
        for ( auto value : values )
        {
          if ( value == rls.tvmazeID )
          {
            if ( CheckRule::debug ) cout << "tvid found " << endl;
            return true;
          }
        }
        if ( CheckRule::debug ) cout << "tvid not found " << endl;
        return false;
      }
      
      if ( command == "language" )
      {
        HelperFunctions::toLower(rls.tvmazeLanguage);
        for ( auto value : values )
        {
          if ( value == rls.tvmazeLanguage )
          {
            if ( CheckRule::debug ) cout << "language found " << endl;
            return true;
          }
        }
        if ( CheckRule::debug ) cout << "language not found " << endl;
        return false;
      }
      
      if ( command == "network" )
      {
        for ( auto value : values )
        {
          if ( value == rls.tvmazeNetwork || value == rls.tvmazeNetworkWeb )
          {
            if ( CheckRule::debug ) cout << "Network found " << endl;
            return true;
          }
        }
        if ( CheckRule::debug ) cout << "network not found " << endl;
        return false;
      }
      if ( command == "premiere" )                                              //zB. premier:2010 TVMAZE
      {
        for ( auto value : values)
        {
          tm date1 = DateTimeFunctions::dateFromString(value);
          tm date2 = rls.tvmazeAirdatePremier;
          if ( CheckRule::debug ) cout << DateTimeFunctions::toString(date1) << " < " << DateTimeFunctions::toString(date2) << endl;
          if ( mktime(&date1) < mktime(&date2) )
          {
            if ( CheckRule::debug ) cout << color::rize("premiere accepted ", "Green") << term << endl;
            return true;
          }
          else
          {
            if ( CheckRule::debug ) cout << color::rize("premiere rejected ", "Red") << term << endl;
            return false;
          }
        }
        return false;
      }
      if ( command == "episodeage" )                                            //z.B. episodeage:3 (3 months) TVMAZE
      {
        for ( auto value : values)
        {
          int months = 0;
          try { months = stoi(value); } catch (...) { cout << "could not stoi month"; }
          tm episodeDate = rls.tvmazeAirdateEpisode;
          episodeDate.tm_mon = episodeDate.tm_mon + months;
          time_t temp = mktime(&episodeDate);
          struct tm episodeDateDelta = *localtime(&temp);
          tm now = DateTimeFunctions::dateFromString("now");
          if ( mktime(&now) < mktime(&episodeDateDelta) )
          {
            if ( CheckRule::debug ) cout << color::rize("episodeage accepted ", "Green") << term << endl;
            return true;            
          }
          else
          {
            if ( CheckRule::debug ) cout << color::rize("episodeage rejected ", "Red") << term << endl;
            return false;
          }
          return false;
        }
        return true;
      }
      if ( command == "airdate" )                                               //z.B. airdate:2012 TVMAZE
      {
        for ( auto value : values)
        {
          tm date1 = DateTimeFunctions::dateFromString(value);
          tm date2 = rls.tvmazeAirdatePremier;
          if ( CheckRule::debug ) cout << DateTimeFunctions::toString(date1) << " < " << DateTimeFunctions::toString(date2) << endl;
          if ( mktime(&date1) < mktime(&date2) )
          {
            if ( CheckRule::debug ) cout << color::rize("airdate accepted ", "Green") << term << endl;
            return true;
          }
          else
          {
            if ( CheckRule::debug ) cout << color::rize("airdate rejected ", "Red") << term << endl;
            return false;
          }
        }
        return false;
      }      
      //IMDB Stuff
      if ( command == "lrating" )                                               //z.B. lrating:5.3
      {
        if ( rls.features["screens"] == "limited" )
          for ( auto value : values)
          {
            float rating = stof(value);
            if ( rls.imdbRating > rating )
              return true;
            else
              return false;
          }
        return false;
      }
      if ( command == "lvotes" )                                               //z.B. lvotes:13464 (limited)
      {
        if ( rls.features["screens"] == "limited" )
          for ( auto value : values)
          {
            int votes = 0;
            try { votes = stoi(value); } catch (...) { cout << "could not stoi votes"; }
            if ( rls.imdbVotes  > votes )
              return true;
            else
              return false;
          }
        return false;
      }
      if ( command == "rating" )                                               //z.B. rating:7.8 (nicht limited)
      {
        if ( rls.features["screens"] != "limited" )
          for ( auto value : values)
          {
            float rating = stof(value);
            if ( rls.imdbRating > rating )
              return true;
            else
              return false;
          }
        return false;
      }
      if ( command == "votes" )
      {
        if ( rls.features["screens"] == "limited" )
          for ( auto value : values)
          {
            int votes = 0;
            try { votes = stoi(value); } catch (...) { cout << "could not stoi votes"; }
            if ( rls.imdbVotes  > votes )
              return true;
            else
              return false;
          }
        return false;
      }
      if ( command == "year" )
      {
        for ( auto value : values)
        {
          int year = stoi(value);
          try { year = stoi(value); } catch (...) { cout << "could not stoi year"; }
          if ( rls.imdbVotes  > year )
            return true;
          else
            return false;
        }
        return false;
      }
      return true;
    }
};

#endif

/*
 * "filter" };
                                        
                      */                  
