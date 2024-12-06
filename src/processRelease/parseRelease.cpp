
#include <assert.h>

#include <chrono>
#include <iostream>
#include <future>
#include <string>
#include <cstring>

#include <thread>
#include <chrono>

#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <mysql.h>

#include "../lib/colorize/color.hpp"

#include "../results/release.cpp"

#include "../common/regexFunctions.cpp"
#include "../common/configFileReader.cpp"
#include "../common/dateTimeFunctions.cpp"

#include "../common/extractName.cpp"
#include "../common/featureExtractor.cpp"
#include "../processRelease/lookupImdb.cpp"
#include "../processRelease/lookupTvmaze.cpp"
#include "../processRelease/lookupDupe.cpp"

#include "../processRules/parseSite.cpp"
#include "../processRules/parseUser.cpp"

#include "../common/logger.cpp"

  

using namespace std;

#ifndef PARSE_RELEASE
#define PARSE_RELEASE

/*******************************************************************************
* does what it says: it parses a release
* - creates the Release Class for all results
* - extracts the name
* - extracts the features
* - looks up the extracted name in imdb or tvmaze
* - processes the sites and users and notifies the users cbftp
*
********************************************************************************
********************************************************************************
*           --->>>  it's a good idea to start here  <<<---
********************************************************************************
*******************************************************************************/
class ParseRelease
{
  public:
    inline static const bool debug = false;
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();  
    
    void parse(Release &rls, bool allUsers)
    {
      Logger* log = Logger::getInstance();
  
      log->info("start: " + rls.releaseName, debugInfo);
  
      rls.releaseName = RegexFunctions::unify(rls.releaseName);

      log->info("unified: " + rls.releaseName, debugInfo);
      
      rls.releaseNameOriginal = rls.releaseName;
      
      HelperFunctions::toLower(rls.releaseName);                                //to mitigate the problem that some things are upper and lower case, so we dont have to test with time intensive regex
      
      if ( ParseRelease::debug ) cout << "Start" << endl;
      string content = ExtractName::extractName(rls.releaseName);

      if ( ParseRelease::debug ) cout << "rls.content" << endl;
      rls.content = RegexFunctions::replace("\\.", " ", content);

      if ( ParseRelease::debug ) cout << "rls.releaseInfo" << endl;
      rls.releaseInfo = ExtractName::extractInfo(rls.releaseName, content);

      if ( ParseRelease::debug ) cout << "rls.releaseYear" << endl;
      rls.releaseYear = ExtractName::extractYear(rls.releaseName);
      
      if ( ParseRelease::debug ) cout << "rls.releaseGroup" << endl;
      rls.group = ExtractName::extractGroup(rls.releaseName);      

      if ( ParseRelease::debug ) cout << "rls.withoutGroup" << endl;
      rls.withoutGroup = ExtractName::removeGroup(rls.releaseName, rls.group);

      if ( ParseRelease::debug ) cout << "featureExtractor::getFeatures" << endl;
      FeatureExtractor::getFeatures(rls);
      

/*   //TODO: maybe differ between stripped features and full features
      try:
        if rls.featuresFull["SECTION"] in ["SPORTS"]:
          raise
      
        if rls.featuresStripped["SECTION"] in ["*", "GAMES", "DOX"]:
*/

      if (rls.features["section"] == "movies")  //TODO: subsitute STV
      {
        if ( ParseRelease::debug ) cout << "imdb analysis" << endl;
        int yearInt = 1900;
        try { yearInt = stoi(rls.releaseYear); } catch (...) { cout << "could not stoi year"; }
        vector<string> movie = LookupImdb::get(rls.content, yearInt);

        rls.imdbTconst        = movie[LookupImdb::imdbTconst];
        rls.titleType        = movie[LookupImdb::titleType];
        rls.imdbPrimaryTitle  = movie[LookupImdb::imdbPrimaryTitle];
        rls.imdbOriginalTitle = movie[LookupImdb::imdbOriginalTitle];
        rls.imdbYear          = movie[LookupImdb::imdbYear];
        rls.imdbGenre         = movie[LookupImdb::imdbGenre];
        rls.features["MOVIEGENRE"] = movie[LookupImdb::imdbGenre];
        try { rls.imdbRating  = stof(movie[LookupImdb::imdbRating]); } catch (...) { if (ParseRelease::debug) cout << "could not stof() rating"; }
        try { rls.imdbVotes = stoi(movie[LookupImdb::imdbVotes]); } catch (...) { if (ParseRelease::debug) cout << "could not stoi() ratings"; }
        rls.tmdbLanguage      = movie[LookupImdb::tmdbLanguage];  //TODO: Wed 24 Jul 2019 11:12:11 PM CEST TOLD SHRIKEY THAT THIS SUORCE IS MISSING
        try { rls.mojoUsScreens     = stoi(movie[LookupImdb::mojoUsScreens]); } catch (...) { rls.mojoUsScreens = 0; if (ParseRelease::debug) cout << "could not stoi() usMojo"; }
        try { rls.mojoUkScreens     = stoi(movie[LookupImdb::mojoUkScreens]); } catch (...) { rls.mojoUkScreens = 0; if (ParseRelease::debug) cout << "could not stoi() ukMojo"; }
        
        if ( rls.mojoUsScreens > 500 || rls.mojoUkScreens > 250 )
          rls.features["screens"] = "wide";

        if ( rls.imdbTconst.substr(0,2) == "tt" )
          rls.features["imdb"] = "nfo";
        else
          rls.features["imdb"] = "nonfo";           //TODO: STV / WIDE ....
        
        if ( ParseRelease::debug ) cout << "IMDB: " << rls.imdbPrimaryTitle << endl;
      }

      if (rls.features["section"] == "tv")
      {
        if ( ParseRelease::debug ) cout << "tvmaze analysis" << endl;
        vector<string> episode = RegexFunctions::findAll("s\\d+e\\d+", rls.releaseName);   //TODO: noch untoll wegen gross kelinbuchstaben. was wenn jemand grossbuchstaben in die DB einträgt
        
        rls.episode = episode.back();
        
        vector<string> separts = RegexFunctions::split('e', rls.episode);
        
        if ( separts.size() > 1 )
          rls.season = separts[0].substr(1);
        else
          rls.season = "0"; 
        
        vector<string> show = LookupTvmaze::get(rls.content, rls.episode, 0, "");  //TODO: 1999 and US from releaseName

        rls.tvmazeID = show[LookupTvmaze::TVMAZE_ID];
        
        rls.tvmazeTitle = show[LookupTvmaze::TVMAZE_TITLE];

        string tvtype = show[LookupTvmaze::TVMAZE_TYPE];
        HelperFunctions::toLower(tvtype);
        rls.tvmazeType = tvtype;
        rls.features["tvtype"] = tvtype;
        
        string language = show[LookupTvmaze::TVMAZE_LANGUAGE];
        HelperFunctions::toLower(language);
        rls.tvmazeLanguage = language;
        rls.features["tvlanguage"] = language;

        string genre = show[LookupTvmaze::TVMAZE_GENRE];
        HelperFunctions::toLower(genre);
        rls.tvmazeGenre = genre;
        rls.features["tvgenre"] = genre;



        rls.tvmazeStatus = show[LookupTvmaze::TVMAZE_STATUS];
        
        rls.tvmazeAirdatePremier = DateTimeFunctions::dateFromString(show[LookupTvmaze::TVMAZE_PREMIERE]);
        
        vector<string> parts = RegexFunctions::split('-', show[LookupTvmaze::TVMAZE_PREMIERE]);
        rls.tvmazeYear = parts[0];
        
        rls.tvmazeNetwork = show[LookupTvmaze::TVMAZE_NETWORK];
        HelperFunctions::toLower(rls.tvmazeNetwork);
        rls.features["tvnetwork"] = rls.tvmazeNetwork;
        
        rls.tvmazeNetworkWeb = show[LookupTvmaze::TVMAZE_NETWORK_WEB];
        HelperFunctions::toLower(rls.tvmazeNetworkWeb);
        rls.features["tvnetworkweb"] = rls.tvmazeNetworkWeb;

        string coutryCode = "nw_" + show[LookupTvmaze::TVMAZE_COUTRY_CODE];
        HelperFunctions::toLower(coutryCode);
        rls.tvmazeCountry = coutryCode;
        rls.features["tvcountry"] = coutryCode;
        
        rls.tvmazeCurerntSeason = show[LookupTvmaze::TVMAZE_CURRENT_SEASON];  //bor1s failed here in the very begining of the project since the tvamze database does not have this item

        tm tvmazeAirdateEpisode = DateTimeFunctions::dateFromString(show[LookupTvmaze::TVMAZE_AIRDATE_EPISODE]);
        
        
        rls.tvmazeAirdateEpisode = tvmazeAirdateEpisode;
      }

      if ( ParseRelease::debug ) cout << rls.sprint() << endl;
      

      rls.features["dupe"] = LookupDupe::get(rls);

      FeatureExtractor::getSection(rls);

      if ( allUsers )
      {
        ParseSite::parseAllSites(rls, cfr->users);
        ParseUser::parseAllUsers(rls, cfr->users);
      }
      else
      {
        ParseSite::parseAllSites(rls, cfr->activeUsers);
        ParseUser::parseAllUsers(rls, cfr->activeUsers);
      }
      if ( ParseRelease::debug ) cout << "rls.sprint" << endl;            

      if ( rls.log )
       rls.logfile = log->getLogContent(rls.allErrors);
      
    }
};

#endif
