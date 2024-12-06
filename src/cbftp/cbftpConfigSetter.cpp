
#include <chrono>
#include <iostream>
#include <future>
#include <string>
#include <cstring>
#include <vector>

#include <thread>
#include <chrono>

#include <cstdlib>
#include <cstdio>
#include <stdio.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>  //for gethostbyname


#include "../lib/colorize/color.hpp"

#include "../common/configFileReader.cpp"
#include "../cbftp/cbftpInterface.cpp"


using namespace std;

#ifndef CBFTP_CONFIG_SETTER
#define CBFTP_CONFIG_SETTER

/*******************************************************************************
* sets sites, affiliates, diretories of a users cbftp, this requires a modified
* cbftp that can accept these functions.
* the modified cbftp should also be in the same gitrepository of pHase
*******************************************************************************/
class CbftpConfigSetter
{
  private:
    inline static const bool debug = false;


  public:
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();


  // ============================================================================
  // function to call with parameter user: eg. user01 (whitouth .cfg)
  // ============================================================================
  static string sendAllSites(string userName)
  {
    bool userFound = false;
    
    for ( auto user : cfr->activeUsers )
      if ( user == userName )
        userFound = true;
    
    if ( !userFound )
      return "user not logged in on this shell or non existent";
    
    if ( CbftpConfigSetter::debug ) cout << "sendAllSites() " << userName << endl;
    vector<string> sites = { };
      
    for ( auto siteItem : cfr->userCaches[userName]->content["site"] )
    {
      vector<string> line = RegexFunctions::split(' ', siteItem.second);  //  TODO add all lines
      sites.insert(sites.end(), line.begin(), line.end());
    }

    for ( auto site : sites )
      sendSite(userName, site);
    
    string answer = "affils and dirs from all sites from user " + userName + " sent (hopefully cbftp was running)";
    return answer;  //TODO: maybe just true or false
  }

  //TODO site mit .cfg user ohne?

  // ============================================================================
  // function to call with parameter user: eg. user01 (whitouth .cfg) sitename
  // ============================================================================
  static string sendSite(string & userName, string & site)
  {
    if ( CbftpConfigSetter::debug ) cout << "sendSite() " << userName << endl;
    bool userFound = false;
    
    for ( auto user : cfr->activeUsers )
      if ( user == userName )
        userFound = true;
    
    if ( !userFound )
      return "user not logged in on this shell or non existent";

    string host = cfr->userCaches[userName]->content["host"]["*"];
    string port = cfr->userCaches[userName]->content["port"]["*"];
    string password = cfr->userCaches[userName]->content["cbpassword"]["*"];
    
    if ( CbftpConfigSetter::debug ) cout << host << ":" << port << " pw: " << password << endl;
    

    sendAffils(host, port, password, "setaffils", site);
    sendDirs(host, port, password, "setdir", site);

    string answer = "affils and dirs from " + site + " sent (hopefully cbftp was running)";
    return answer;
   }

  static string sendAffils(string host, string port, string password, string messageType, string siteName)
  {
    if ( CbftpConfigSetter::debug ) cout << "sendAffils()" << endl;
    
    vector<string> affils = RegexFunctions::split(' ', cfr->siteCaches[siteName+".cfg"]->content["affils"]["*"]);  //TODO: everything lowercase, check what if multiline affils
    
    string settings = "";
    
    for ( auto affil : affils )
    {
      settings += "," + affil;
    }
    
    if ( CbftpConfigSetter::debug ) cout << "a: " << settings << endl; 
    
    if ( settings != "" )
      CbftpInterface::sendSetting(host, port, password, messageType, siteName, settings.substr(1));
    
    return "";
  }


  static string sendDirs(string host, string port, string password, string messageType, string siteName)
  {
    if ( CbftpConfigSetter::debug ) cout << "site (dir): " << siteName << endl; 

    unordered_map<string, string> dirs = cfr->siteCaches[siteName+".cfg"]->content["dir"];
  
    string settings = "";
    
    for ( auto dir : dirs )
    {
      settings += ";" + dir.first+","+dir.second;
    }

    if ( CbftpConfigSetter::debug ) cout << "d: " << settings << endl; 

    if ( settings != "" )
      CbftpInterface::sendSetting(host, port, password, messageType, siteName, settings.substr(1));

    return "";
  }

};

#endif

