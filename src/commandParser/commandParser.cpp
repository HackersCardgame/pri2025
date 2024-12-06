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


#include "../lib/colorize/color.hpp"

#include "../common/regexFunctions.cpp"
#include "../common/helperFunctions.cpp"

#include "../commandParser/commandParserHelp.cpp"
#include "../commandParser/commandParserSqlFunctions.cpp"
#include "../commandParser/commandParserTools.cpp"
#include "../commandParser/commandParserIndex.cpp"
#include "../commandParser/commandParserDebug.cpp"
#include "../commandParser/commandParserRelease.cpp"
#include "../commandParser/commandParserSite.cpp"
#include "../commandParser/commandParserUser.cpp"
#include "../commandParser/commandParserSync.cpp"

#include "../commandParser/history.cpp"


#ifndef COMMAND_PARSER
#define COMMAND_PARSER


using namespace std;

/*******************************************************************************
* does what it says: 
* parses the commands that come from IRC or phaseClient, this is maybe also
* something you need to change over time since you maybe will have new commands
*******************************************************************************/
class CommandParser
{

  public:
    inline static const bool debug = false;


    //splits sites commands with multiple sites like bbr,zh,gp to single commands for each site one
    static string execute(string commandLine, string user, bool brief)
    {
	    std::lock_guard<std::mutex> myLock(commandParserMutex);

      
      History::writeHistory(user, "command", commandLine, "--");

      HelperFunctions::toLower(commandLine);

      if ( RegexFunctions::match(",", commandLine) )
      {
        string message = "";
        vector<string> commands = CommandParserTools::commandToVector(commandLine);
        
        int partNo = 0;
        for ( auto command : commands )
        {
          if ( RegexFunctions::match(",", command) )
          {
            vector<string> sites = RegexFunctions::split(',', command);
            for ( auto site : sites )
            {
              string begin = "";
              string end = "";

              for ( auto it = commands.begin(); it !=commands.begin()+partNo; it++)
                begin += *it + " ";

              for ( auto it = commands.begin()+partNo+1; it !=commands.end(); it++)
                end += " " + *it;

              if ( CommandParserTools::isSite(site) )  //XXX site add asdf,qwer,dsfg will not work, but it probably does not make sense to mass add sites since users shall do this carefully
                message += executeSingle(begin + site + end, user, brief);
            }
            break;
          }
          partNo++;
        }
        return message;
      }
      else
        return executeSingle(commandLine, user, brief);

    }
    

    static string executeSingle(string commandLine, string user, bool brief)
    {
      bool force = false;
      bool debugRls = false;
      
      string message = "";

      //prevent sql-injection
      commandLine = RegexFunctions::replace("'", "", commandLine);
      commandLine = RegexFunctions::replace("\"", "", commandLine);
    
      if ( CommandParser::debug ) cout << "COMMAND: " << commandLine << endl;
      vector<string> commands = CommandParserTools::commandToVector(commandLine);

      if ( commands.back() == "-f" )
      {
        force = true;
        commands.erase(commands.end());
      }

      if ( commands.back() == "-d" )
      {
        debugRls = true;
        commands.erase(commands.end());
      }
      if ( CommandParser::debug ) cout << "pre call commandParserCommands() " << commandLine << endl;

      message += CommandParserIndex::commands(CommandParserTools::restToString(commands), user, force, debugRls, brief);

      if ( CommandParser::debug ) cout << "post call commandParserCommands() " << commandLine << endl;

      //message += color::rize("<EOF>", "Yellow");

     //TODO: > group codex => affils: xy, yz, ... blacklist: ab, cv, ef
      cout << message;
      return message;


     }

    static std::mutex commandParserMutex;

};
    
std::mutex CommandParser::commandParserMutex;
    
#endif
