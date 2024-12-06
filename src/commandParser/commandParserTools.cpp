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

#include <thread>

#include <signal.h>


#include "../lib/colorize/color.hpp"

#include "../common/regexFunctions.cpp"
#include "../common/dbConnection.cpp"
#include "../common/configFileReader.cpp"
#include "../common/helperFunctions.cpp"

#include "../processRules/checkRuleSyntax.cpp"
#include "../processRelease/parseRelease.cpp"

#include "../cbftp/cbftpConfigSetter.cpp"

#include "../processRelease/lookupImdb.cpp"
#include "../processRelease/lookupTvmaze.cpp"
#include "../processRelease/lookupDupe.cpp"



#ifndef COMMAND_PARSER_TOOLS
#define COMMAND_PARSER_TOOLS


using namespace std;

/*******************************************************************************
* does what it says: 
* parses the commands that come from IRC or phaseClient, this is maybe also
* something you need to change over time since you maybe will have new commands
*******************************************************************************/
class CommandParserTools
{

  public:
    inline static const bool debug = false;



    inline static DbConnection* dbc = DbConnection::getInstance();
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();
    inline static ParseRelease pr = ParseRelease();

    //TODO: maybe better user parameter and site parameter
    
    static bool isSite(string site)
    {
      if ( site == "%" ) return true;  //TODO: maybe dangerous, needed for bulk modification
      return isInVector(site+".cfg", cfr->sites);
    }
    
    
    static bool isUser(string user)
    {
      return isInVector(user+".cfg", cfr->users);
    }
    
    static bool isInVector(string value, vector<string> &vec)
    {
      if (count(vec.begin(), vec.end(), value))
       return true;
      return false;
    }


    //TODO: description
    static vector<string> commandToVector(string command)
    {
      string removedDoubleSpaces = RegexFunctions::replace("[ ]+", " ", command);
      return RegexFunctions::split(' ', removedDoubleSpaces);
    }
    
    //TODO: description
    static string restToString(vector<string> rest)
    {
      string result = "";
      if ( rest.size() == 0 )
        return "";
        
      if ( rest.size() == 1 )
        return rest.front();
        
      if ( rest.size() > 1 )
      {
        result += rest.front();
        for ( auto it = rest.begin()+1; it !=rest.end(); it++)
          if ( *it != "" )
            result += " " + *it;
        return result;
      } 
      return "";
    }

    //TODO: discription
    static string consume(vector<string> &commands)
    {
      if ( commands.size() == 0 )
        return "";
        
      string command = commands.front();
      commands.erase(commands.begin());
      return command;
    }
    
    //TODO: description
    static string peek(vector<string> &commands)
    {
      if ( commands.size() == 0 )
        return "";
        
      string command = commands.front();
      return command;
    }
    
    //TODO: Description
    static string at(vector<string> &commands, int pos)
    {
//      if ( commands.size() == 0 )
//        return "";
      try
      {
        return commands.at(pos);
      }
      catch (...)
      {
        if ( CommandParserTools::debug ) cout << "you asked for an element higher than the lenght of the vector" << endl;
      }
      return "";
    }
    
    //TODO: uses jaro winkler to fix typos
    static string typo(string c1, vector<string> availabeCommands)
    {
      if ( CommandParserTools::debug ) cout << ">>" << c1 << "<<" << endl;

      float bestSimilarity = 0;
      string bestMatch = "";
      for ( auto c : availabeCommands )
      {
        float similarity = HelperFunctions::similar((char*)c.c_str(), (char*)c1.c_str());
        if ( CommandParserTools::debug ) cout << c << " " << similarity << endl;
        if ( bestSimilarity < similarity )
        {
          bestSimilarity = similarity;
          bestMatch = c;
        }
      }
      
      if ( bestSimilarity > 0.6 && c1 != bestMatch )
      {
        if ( CommandParserTools::debug ) cout << "best: " << bestSimilarity << endl;
        return bestMatch;
      }
      return c1;
    }
    

    //TODO: WTF!!!  <- Generates a list of abbreviation for each commands, the algorithm starts with first letter, first finds sync, then finds site, sees that s can not be used as abbreviation
    //                                                                                                                                                and marks then s with invalid
    //                                                                                   continues with the first 2 letters, since sy and si is not ambigious does not mark them as invalid
    static vector<string> shorts(string & c1, vector<string> availabeCommands)
    {
      //struct for the abbreviations, will be filled from available commands, and if you use sit it would find site and sitebans and so the count would be > 0 and it says that it is ambigious
      struct shorts
      {
        string abbreviation;
        int count;
        vector<string> commands;
      };

      vector<shorts> shs;

      int len = c1.length();
      
      for ( auto c : availabeCommands )
      {
        string s = c.substr(0,len);

        bool found = false;
        
        for ( auto & sh : shs )
        {
          if ( CommandParserTools::debug ) cout << sh.abbreviation << " " << s << endl;
          if ( sh.abbreviation == s )
          {
            for ( auto sc : sh.commands )
            {
              if ( sc != c )
              {
                sh.count++;
                sh.commands.push_back(c);
                found = true;
              }
            }
          }
        }
        if ( !found )
        {
          if ( CommandParserTools::debug ) cout << "new: " << s << endl;
          shorts newShort = { .abbreviation = s, .count = 0 };
          newShort.commands.push_back(c);
          shs.push_back(newShort);
        }
      }
      
      for ( auto sh : shs )
      {
        if ( sh.count == 0 )
          availabeCommands.push_back(sh.abbreviation);
      }
      
      return availabeCommands;
    }
    
    //TODO: Description
    static string getCommandFromAbbrev(string & c1, vector<string> availabeCommands)
    {

      int len = c1.length();
      
      for ( auto c : availabeCommands )
      {
        string s = c.substr(0,len);

        if ( c1 == s )
          return c;
      }
      
      return "";    
    }
    
    
};

#endif
