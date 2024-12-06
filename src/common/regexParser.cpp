
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
//#define total_hours_wasted_here = 1328
//
//                Please update this counter if you wasted time here
//
// ==============================================================================


#pragma once

#include <time.h>
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unordered_map>

//#include "../common/regexFunctions.cpp"
#include "../common/helperFunctions.cpp"
#include "../common/configFileReader.cpp"
#include "../common/regexFunctions.cpp"

using namespace std;

#ifndef REGEX_PARSER
#define REGEX_PARSER

/*******************************************************************************
* like described above, this is one of the most complicated part of this project
* it is very discurraged to change things here. mostly you would be to stupid
* to understand what happens here. but if you really would like to give you
* a try then i recommend doing first this tutorial:
*
* https://craftinginterpreters.com/ or if you would like to do it offline
* https://github.com/munificent/craftinginterpreters
*
* if noone deleted it it should be in the /doc/craftinginterpreters directory
*
* WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
*
* IF YOU DONT HAVE AT LEAST /SOME DEGREE/ in CS (Computer Science)
*
* then the chances will be very high that you brick the whole prject
* with just a single mistake in here
*
* but if you need to maybe first have a look at "craftinginterpreters"
*******************************************************************************/
class RegexParser
{
    inline static const bool debug = false;
    

  private:
    inline static string year = "19\\d\\d|20\\d\\d";
    chrono::system_clock::time_point begin;
    chrono::system_clock::time_point end;
    
    static std::atomic<RegexParser*> instance;
    static std::mutex regexParserMutex;
    
    ConfigFileReader* cfr;
    
  public:

    static RegexParser* getInstance(){
      RegexParser* sin = instance.load();
      if ( !sin ){
        std::lock_guard<std::mutex> myLock(regexParserMutex);
        sin= instance.load();
        if( !sin ){
          sin= new RegexParser();
          instance.store(sin);
        }
      }
      // volatile int dummy{};
      return sin;
    }
  

    RegexParser()
    {
      this->begin = std::chrono::high_resolution_clock::now();
      this->cfr = ConfigFileReader::getInstance();
    }

    //Destructor        
    ~RegexParser()
    { 
      this->end = std::chrono::high_resolution_clock::now();
      string text = "~RegexParser()";
      cout << HelperFunctions::sprintTime(text, begin, end);
    }  


  
    //go through rule splitted by " " and evaluet in AND or OR logic  (this is the entrypoint for parsing a rule in general.cfg or site cfg
    bool parse(string rule, string release, bool add)
    {
      vector<std::string> ruleVector = RegexFunctions::split(' ', rule);
      if(add) //and OR-Cunjunction
      {
        for(auto item : ruleVector)
        {
          if (!select(item, release))
            return false;
        }
        return true;
      }
      else  //case OR-Cunjunction
      {
        for(auto item : ruleVector)
        {
          if (select(item, release))
            return true;
        }
        return false;
      }
    }
    
    bool select(string item, string release)
    {
      if(hasNonBracedOrPipes(item, release))
      {
        return parsePipes(item, release);
      }
      else if (item.substr(0,1) == "$")     //"^" will be done from regex
      {
        return parse(this->cfr->generalCache.content["keyword"][item.substr(1)], release, false);
      }
      else if (item.substr(0,2) == "!$")
      {
        return !parse(this->cfr->generalCache.content["keyword"][item.substr(2)], release, false);
      }
      else if (item.substr(0,1) == "!")
      {
        return !search(item, release);
      }
      else
      {
        return search(item, release);
      }
    }
    
    //this function is called many times and uses regex, if it slows down then here
    bool search(string word, string release)
    {
      bool match;
      string first = word.substr(0,1);
      if ( first == "^" || first == "-" || first == "." || first == "[" )       //XXX special case for the ^ regex command and words that have already . or -
        match = RegexFunctions::match(word, release);
      else
      {
        if (RegexParser::debug) cout << "ASDF" << "."+word+"." << endl;
        match = RegexFunctions::match("[._-]"+word+"[._-]", "."+release+".");
      }
      if (RegexParser::debug) cout << word << " --> " << release << " matches? " << match << endl;
      return match;
    }
    
    
    //to find out if there are pipes that are not in braces
    bool hasNonBracedOrPipes(string item, string release)  //TODO TESTCASE ([._]|SUB|TRUE)FRENCH[._-])  TESTCASE2 topic MOVIES = !$sport !$TVEP !$DATE $progressiveScan|$source|$codec
    {
      int openBrackets = 0;
      for(auto c : item)
      {
        if (c=='(')
          openBrackets++;
        if (c==')')
          openBrackets--;
        if (c=='|')
          if (openBrackets == 0)
            return true;
      }
      return false;
    }

    //parses the parts that are conjuncted with a | but not in braces individual
    bool parsePipes(string item, string release)  // was parseBrackets()     //XXX: this is needet to parse $codec|$resolution since variables are not in the regex standard
    {                                                                               //XXX: but for example ($codec|$resolution) we would leave to regex, TODO: shall we?
      int i = 0;
      int openBrackets = 0;
      vector<int> splitPoints = {0};
      for(auto c : item)
      {
        if (c=='(')
          openBrackets++;
        if (c==')')
          openBrackets--;
        if (c=='|')
          if (openBrackets == 0)
            splitPoints.push_back(i);
        i++;
      }
      
      splitPoints.push_back(i);   //XXX: append the end to the list
      string parts = item.substr(splitPoints[0],splitPoints[1]) + " ";

      int n = splitPoints.size();
      for(int m = 0; m < n-2; m++)
        parts += item.substr(splitPoints[m+1]+1,(splitPoints[m+2]-splitPoints[m+1]-1)) + " ";

      return parse(parts.substr(0,parts.length()-1), release, false);
      
    }
};

std::atomic<RegexParser*> RegexParser::instance;
std::mutex RegexParser::regexParserMutex;

#endif
