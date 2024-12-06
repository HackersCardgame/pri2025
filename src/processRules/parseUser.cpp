
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

#include "../cbftp/cbftpInterface.cpp"
#include "../commandParser/history.cpp"


using namespace std;

#ifndef PARSE_USER
#define PARSE_USER
static std::mutex parseUserMutex;

/*******************************************************************************
* does what it says: 
* parses a user and after that sending the command to its cbftp that is
* identified with a specific port and has a password
* it does it with parallel processing / multi-threading so the more
* CPUs a server has the faster is this part and the faster is the parsing of
* the sites
*******************************************************************************/
class ParseUser
{
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();

  private:
    inline static const bool debug = false;

    
  public:
    static int parseUser(string userName, Release &rls, array<BlacklistItem,4> &generalBlacklist)
    {
      //1ms
      if ( ParseUser::debug ) cout << "parseUser() " << userName << endl;
      User userResult = User(userName);

      if ( rls.login ) History::writeHistory(userName, "login", "login", rls.releaseNameOriginal);

      userResult.blacklist = CheckBlacklist::evaluate(rls.releaseNameOriginal, rls.group, rls.tvmazeNetwork, rls.tvmazeNetworkWeb, rls.section, cfr->userCaches[userName]->content);

      if ( ParseUser::debug ) cout << "userBlaklist: " << userResult.blacklist[0] << " " << userResult.blacklist[1] << " " << userResult.blacklist[2] << " " << userResult.blacklist[3] << endl;

      vector<std::string> sites = { };
      
      vector<string> goodForCbftp = { };
      
      for ( auto siteItem : cfr->userCaches[userName]->content["site"] )
      {
        vector<std::string> line = RegexFunctions::split(' ', siteItem.second);  //  TODO add all lines
        sites.insert(sites.end(), line.begin(), line.end());
      }
      
      for ( auto site : sites )
      {
        if ( ParseUser::debug ) cout << "--> " << userName << " " << site << endl;
        auto it = rls.siteResults.find(site + ".cfg" );  //TODO: is a bit ugly
        if ( it != rls.siteResults.end() )
        {
          userResult.allSites.push_back(site);
          if ( it->second.matchedRule != "*" )
          {
            
            if ( ParseUser::debug ) cout << "siteblacklist: " << it->second.blacklist[0] << " " << it->second.blacklist[1] << " " << it->second.blacklist[2] << " " << it->second.blacklist[3] << endl;

            array<BlacklistItem,4> generalSiteBlacklist = CheckBlacklist::merge(generalBlacklist, it->second.blacklist);
            
            if ( ParseUser::debug ) cout << "generalSiteBL: " << generalSiteBlacklist[0] << " " << generalSiteBlacklist[1] << " " << generalSiteBlacklist[2] << " " << generalSiteBlacklist[3] << endl;
            
            array<BlacklistItem,4> generaSiteUserBlacklist = CheckBlacklist::merge(generalSiteBlacklist, userResult.blacklist);
            
            if ( CheckBlacklist::consolidate(generaSiteUserBlacklist) < 1 )  //0 = blacklisted, 1 = neutral, 2 = whitelisted
            {
              if ( ParseUser::debug ) cout << generaSiteUserBlacklist[0] << " " << generaSiteUserBlacklist[1] << " " << generaSiteUserBlacklist[2] << " " << generaSiteUserBlacklist[3] << " blacklistConsolidated: RELEASE BLACKLISTED" << endl;
              userResult.skippedSites.push_back(site+"/blacklist");
              continue;
              
            }
            
            if ( ParseUser::debug ) cout << "Found RULE " << it->second.matchedRule << endl; 
            if ( it->second.matchedRule == "affils" )
            {
              
              userResult.preSites.push_back(site + "/affils");
              goodForCbftp.push_back(site);
            }
            else
            {
              goodForCbftp.push_back(site);
              userResult.allowedSites.push_back(site + "/" + it->second.matchedRule);
            }
          }
          else
          {
            if ( ParseUser::debug ) cout << "skipped RULE " << it->second.matchedRule << endl;
            userResult.skippedSites.push_back(site);
          }
        }
        else
        {
          cout << color::rize("ERROR ", "White", "Red") << " SITE " << site << " NOT FOUND IN SITE CACHE!!!!!!!!!!!!!!" << endl;
        }
      }

      if (rls.login)
        if ( goodForCbftp.size() > 1 )
        {
          CbftpInterface::sendRelease(cfr->userCaches[userName]->content["host"]["*"],
                                      cfr->userCaches[userName]->content["port"]["*"],
                                      cfr->userCaches[userName]->content["cbpassword"]["*"],
                                      "prepare",rls.section,rls.releaseNameOriginal, goodForCbftp);

        }

      auto pair = make_pair(userName, userResult);
            
      std::lock_guard<std::mutex> myLock(parseUserMutex);
      rls.userResults.insert(pair);
      
      return 0;

    }
    

    static void parseAllUsers(Release &rls, vector<string> &users)
    {
      array<BlacklistItem,4> generalBlacklist = CheckBlacklist::evaluate(rls.releaseNameOriginal, rls.group, rls.tvmazeNetwork, rls.tvmazeNetworkWeb, rls.section, cfr->generalCache.content);

      if ( ParseUser::debug ) cout << "generalBlaklist: " << generalBlacklist[0] << " " << generalBlacklist[1] << " " << generalBlacklist[2] << " " << generalBlacklist[3] << endl;

      int i=0;
  
      thread threads[1000];

      for ( auto user : users )
      {
        if ( ParseUser::debug )
        {
          ParseUser::parseUser(user, rls, generalBlacklist);
        }
        else
        {
          threads[i] = thread(parseUser, user, ref(rls), ref(generalBlacklist));  //TODO reenable threads
          i++;
        }
      }

      if ( !ParseUser::debug )
      {
        for ( int j = 0; j < i; j++ )
        {
          threads[j].join();
        }
      }
    }
};

#endif
