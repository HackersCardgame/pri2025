
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


#include "../lib/colorize/color.hpp"

#include "../common/featureExtractor.cpp"
#include "../common/configFileReader.cpp"

#include "../common/regexFunctions.cpp"

#include "../common/configFileReader.cpp"
#include "../processRules/checkRule.cpp"
#include "../processRules/checkBlacklist.cpp"

#include "../results/release.cpp"

#include "../common/logger.cpp"


using namespace std;

#ifndef PARSE_SITE
#define PARSE_SITE
static std::mutex parseSiteMutex;

/*******************************************************************************
* does what it says: 
* parses a site (config file) and stores the result in "siteResult"
* you can find the template for the results in /src/results/site.cpp
* this is stored because maybe multiple users have the same sites, so we
* dont have to calculate the site multiple times, only the first time
*******************************************************************************/
class ParseSite
{
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();
    inline static Logger* log = Logger::getInstance();


  private:
    inline static const bool debug = false;
    
    inline static string separator = "(S\\d\\dE\\d\\d|20\\d\\d|19\\d\\d|XXX|part[\\d]|[\\.]part[\\.]|LiMiTED|SWESUB|PDTV|1080p|720p|2160p|bluray|US|UK)";
    
  public:
    static int parseSite(string siteName, Release &rls)
    {
      if ( ParseSite::debug ) cout << "parseSite()" << endl;
      log->info("Parsing Site " + siteName, debugInfo);
      //2ms
      Site siteResult = Site(siteName);

      if ( ParseSite::debug ) cout << "parseSite() pre evaluateBlacklist" << endl;
      
      if ( ParseSite::debug ) cout << rls.releaseNameOriginal << endl;

      if ( ParseSite::debug ) cout << rls.tvmazeNetwork << endl;

      if ( ParseSite::debug ) cout << rls.tvmazeNetworkWeb << endl;

      if ( ParseSite::debug ) cout << rls.section << endl;

      bool siteFound = false;
      for ( auto sc : cfr->siteCaches )
        if ( sc.first == siteName )
          siteFound = true;
      
      if ( siteFound )
      {
        siteResult.blacklist = CheckBlacklist::evaluate(rls.releaseNameOriginal, rls.group, rls.tvmazeNetwork, rls.tvmazeNetworkWeb, rls.section, cfr->siteCaches[siteName]->content);

        if ( ParseSite::debug ) cout << "parseSite() post evaluateBlacklist" << endl;

        if ( ParseSite::debug ) cout << siteName << " siteBlaklist: " << siteResult.blacklist[0] << " " << siteResult.blacklist[1] << " " << siteResult.blacklist[2] << endl;
        
        vector<std::string> affils = RegexFunctions::split(' ', cfr->siteCaches[siteName]->content["affils"]["*"]);

        if ( ParseSite::debug ) cout << "parseSite() pre affils" << endl;
        
        for ( auto affil : affils )
          if ( affil == rls.group )
          {
            siteResult.matchedRule = "affils";
            break;
          }

        if ( ParseSite::debug ) cout << "parseSite() post affils" << endl;

        siteResult.matchedRule = checkSite(siteName, rls);
      }
      else
        siteResult.matchedRule = "*";

      auto pair = make_pair(siteName, siteResult);
      
      std::lock_guard<std::mutex> myLock(parseSiteMutex);

      rls.siteResults.insert(pair);
      
      return 0;

    }
    
    static string checkSite(string siteName, Release &rls)
    {
      unordered_map<string, string> rules = cfr->siteCaches[siteName]->content["rule"];

      for ( int i = 10; i >= 0; i--)
      {
		    for ( auto rule : rules )      //begin with the rule with the most . points to make it more deterministic
		    {
		      if ( i == RegexFunctions::occurances(".", rule.first) )
		      {
				    string result = CheckRule::checkRule(siteName, rule.first, rule.second, rls);
				    if ( result != "*" )
				    {
				      return result;
				    }
				  }
		    }
		  }
		    return "*";
    }

    static void parseAllSites(Release &rls, vector<string> &users)
    {
      int i=0;
  
      thread threads[1000];

      vector<string> allSites = { };
      
      for ( auto user : users )
      {

        for ( auto siteLine : cfr->userCaches[user]->content["site"] )
        {
          vector<string> siteLineElements = RegexFunctions::split(' ', siteLine.second);
          for ( auto site : siteLineElements )
            allSites.push_back(site+".cfg");
        }
        
      }

      if ( ParseSite::debug ) cout << "MARK1.0" << endl;

      for ( auto site : allSites )
      {
        if ( ParseSite::debug )
        {
          ParseSite::parseSite(site, rls);  //TODO: reenable threads
        }
        else
        {
          threads[i] = thread(parseSite, site, ref(rls));
          i++;
        }
      }

      if ( !ParseSite::debug )
      {
        for ( int j = 0; j < i; j++ )
        {
          threads[j].join();
        }
      }
    }
};

#endif
