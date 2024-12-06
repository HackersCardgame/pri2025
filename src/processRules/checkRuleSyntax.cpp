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
//#include <sys/types.h>

#include <algorithm>


#include <thread>

#include <signal.h>

#include "../lib/colorize/color.hpp"

#include "../common/regexFunctions.cpp"
#include "../common/configFileReader.cpp"

#ifndef SYNTAX_CHECKER
#define SYNTAX_CHECKER


//TODO: check duplicated items

using namespace std;

/*******************************************************************************
* does what it says: checks the syntax of a rule and displays the error
* in red so that noone accidentially enters VT instead of TV
*******************************************************************************/
class CheckRuleSyntax
{

  public:
    inline static const bool debug = false;

    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();



    static string checkSyntax(string term)
    {
      if ( term == "" )
        return "";
      string removedDoubleSpaces = RegexFunctions::replace("[ ]+", " ", term);
      if ( term != removedDoubleSpaces )
        return "dont add doublespaces";
        
      string trimmedTerm = RegexFunctions::trim(term);
      if ( term != trimmedTerm )
        return "dont add leading or trailing spaces";
      
      string result = "";
      vector<string> parts = RegexFunctions::split(' ', removedDoubleSpaces);

      for ( auto part : parts )
      {
        if(std::find(cfr->validCommands.begin(), cfr->validCommands.end(), part) != cfr->validCommands.end())
        {
          result += " " + part;
          continue;
        }
        vector<string> subparts = RegexFunctions::split(':', part);           //TODO, what if airdate:5:sadf    MAYBE BETTER USE CASE
        if ( subparts.size() != 2 )
        {
          if ( CheckRuleSyntax::debug ) cout << "SUBPART" << endl;
          result += " >" + color::rize(part, "Red") + "<";
          continue;
        }
        else
        {
          bool found = false;
          for ( auto combinedCommand : cfr->combinedCommands )
          {
            if ( combinedCommand == subparts[0] )
            {
              if ( CheckRuleSyntax::debug ) cout << "SUBPART found " << subparts[0] << endl;
              result += " " + part;
              found = true;
              continue;
            }
          }
          if ( found ) continue;
        }
        result += " >" + color::rize(part, "Red") + "<";
      }
      return result.substr(1);
        
    }
        
      //replace "|" with " " ???
};


#endif
