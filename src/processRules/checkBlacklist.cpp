#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstdio>
#include <iostream>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>


#include <thread>

#include <signal.h>

#include "../lib/colorize/color.hpp"


#include "../common/regexParser.cpp"
#include "../common/configFileReader.cpp"


#ifndef BLACKLIST_CHECKER
#define BLACKLIST_CHECKER


using namespace std;

enum BlacklistItem { black, none, white };  //white has always the bigger weight, but whitlisted Network does not mean that blacklisted section gets overwritten

/*******************************************************************************
* does what it says: checks the blacklist from general.cfg, user.cfg and sites.cfg   TODO: needs to be tested
* 
*******************************************************************************/
class CheckBlacklist
{

  public:
    inline static const bool debug = false;

    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();
    inline static RegexParser* rp = RegexParser::getInstance();
    
    static BlacklistItem evaluateBlacklist(string &releaseName, string &groupName, string &network, string &networkWeb, string &section, unordered_map<string, unordered_map<string, string>> &content)
    {
      if ( CheckBlacklist::debug ) cout << "evaluateBlacklist()" << endl;

      for ( auto line : content["whitelist"] )
      {

        if ( rp->parse(line.second, releaseName, false) )
          return white;
      }

      for ( auto line : content["blacklist"] )
      {
        if ( rp->parse(line.second, releaseName, false) )
          return black;
      }
      return none;
    } 

    static BlacklistItem evaluateGroupBlacklist(string &releaseName, string &groupName, string &network, string &networkWeb, string &section, unordered_map<string, unordered_map<string, string>> &content)
    {
      if ( CheckBlacklist::debug ) cout << "evaluateGroupBlacklist()" << endl;

      for ( auto line : content["groupwhitelist"] )
      {
        if ( rp->parse(line.second, groupName, false) )
        {
          cout << "groupwhitelist" << endl;
          return white;
        }
      }

      for ( auto line : content["groupblacklist"] )
      {
        if ( rp->parse(line.second, groupName, false) )
        {
          cout << "groupblacklist" << endl;
          return black;
        }
      }
      return none;
    } 


    static BlacklistItem evaluateSectionBlacklist(string &releaseName, string &groupName, string &network, string &networkWeb, string &section, unordered_map<string, unordered_map<string, string>> &content)
    {
      for ( auto line : content["sectionwhitelist"] )
      {
        if ( rp->parse(line.second, section, false) )
          return white;
      }

      for ( auto line : content["sectionblacklist"] )
      {
        if ( rp->parse(line.second, section, false) )
          return black;
      }
      
      return none;
    } 

    static BlacklistItem evaluateNetworkBlacklist(string &releaseName, string &groupName, string &network, string &networkWeb, string &section, unordered_map<string, unordered_map<string, string>> &content)
    {
      for ( auto line : content["networkwhitelist"] )
      {
        if ( rp->parse(line.second, network, false) || rp->parse(line.second, networkWeb, false) )
          return white;
      }


      for ( auto line : content["networkblacklist"] )
      {
        if ( rp->parse(line.second, network, false) || rp->parse(line.second, networkWeb, false) )
          return black;
      }
      
      return none;
    }


    static array<BlacklistItem,4> evaluate(string &releaseName, string &groupName, string &network, string &networkWeb, string &section, unordered_map<string, unordered_map<string, string>> &content)
    {
      if ( CheckBlacklist::debug ) cout << "evaluate()" << endl;

      array<BlacklistItem,4> result = {none, none, none};
      
      result[0] = evaluateGroupBlacklist(releaseName, groupName, network, networkWeb, section, content);

      if ( CheckBlacklist::debug ) cout << "evaluate() group bl" << endl;

      result[1] = evaluateBlacklist(releaseName, groupName, network, networkWeb, section, content);

      if ( CheckBlacklist::debug ) cout << "evaluate() bl" << endl;

      result[2] = evaluateSectionBlacklist(releaseName, groupName, network, networkWeb, section, content);

      if ( CheckBlacklist::debug ) cout << "evaluate() section bl" << endl;

      result[3] = evaluateNetworkBlacklist(releaseName, groupName, network, networkWeb, section, content);

      if ( CheckBlacklist::debug ) cout << "evaluate() network bl" << endl;

      return result;
    }
    
    

  static array<BlacklistItem,4> merge(array<BlacklistItem,4> lowerPriority, array<BlacklistItem,4> higherPriority)
  {  
    array<BlacklistItem,4> result = { none, none, none, none };

    for ( int i = 0; i < 4; i++ )
    {
      if ( lowerPriority[i] == black )
        if ( higherPriority[i] != white )
          result[i] = black;
      if ( higherPriority[i] == black )
        result[i] = black;
      if ( higherPriority[i] == white )
        result[i] = white;
    }
        
    return result;
  }

  static BlacklistItem consolidate(array<BlacklistItem,4> blacklist)
  {
    for ( int i = 0; i < 4; i++ )
      if ( blacklist[i] == black )
        return black;
    return none;
  }

};

#endif
