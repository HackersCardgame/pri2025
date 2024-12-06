
#include <numeric>

#include "../common/regexFunctions.cpp"


#include "../commandParser/commandParserSqlFunctions.cpp"
#include "../commandParser/commandParserTools.cpp"
#include "../commandParser/commandParserDebug.cpp"
#include "../commandParser/commandParserRelease.cpp"
#include "../commandParser/commandParserSite.cpp"
#include "../commandParser/commandParserUser.cpp"
#include "../commandParser/commandParserSync.cpp"
#include "../commandParser/commandParserAutocomplete.cpp"

#include "../commandParser/history.cpp"


#ifndef COMMAND_PARSER_COMMANDS
#define COMMAND_PARSER_COMMANDS


using namespace std;

/*******************************************************************************
* does what it says: 
* parses the commands that come from IRC or phaseClient, this is maybe also
* something you need to change over time since you maybe will have new commands
*******************************************************************************/
class CommandParserIndex
{

  //TODO: maybe use similarity() to identify typose?

  public:
    inline static const bool debug = false;
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();

    static string commands(string commandLine, string user, bool force, bool debugRls, bool brief)
    {
      if ( CommandParserIndex::debug ) cout << "CommandParserIndex::commands()" << endl;
      string message = "";

      if ( commandLine.substr(0,1) == "!" )
        commandLine = commandLine.substr(1);

      vector<string> commands = CommandParserTools::commandToVector(commandLine);

      string p0 = CommandParserTools::at(commands, 0);
      string p1 = CommandParserTools::at(commands, 1);

      bool isAdmin = false;
      vector<string> admins = RegexFunctions::split(' ', cfr->generalCache.content["admin"]["*"]);
      
      for ( auto admin : admins )
      {
        if ( admin == user )
        {
          isAdmin = true;
        }
      }


      bool foundFull = false;

      //TODO: desription
      for ( auto a : cfr->allCommands )
      {
        if ( a == p0 )
          foundFull = true;
      }


      //generate vector of abreviated commands
      vector<string> withAbbreviations = CommandParserTools::shorts(p0, cfr->allCommands);

      bool foundAbreviated = false;
      
      if ( !foundFull )
		    for ( auto a : withAbbreviations )
		    {
		      if ( a == p0 )
		        foundAbreviated = true;
		    }

      if ( foundAbreviated )
      {
        message += "replacing command " + color::rize(p0, "Red");
        p0 = CommandParserTools::getCommandFromAbbrev(p0, cfr->allCommands);
        message += " with " + color::rize(p0, "Green") + "\n";
        commands.at(0) = p0;
      }

      if ( !foundFull && !foundAbreviated && !CommandParserTools::isUser(p0) && !CommandParserTools::isSite(p0) )
      {
        message += "maybe TyPO; replacing command " + color::rize(p0, "Red");
        p0 = CommandParserTools::typo(p0, cfr->allCommands);
        message += " with " + color::rize(p0, "Green") + "\n";
        commands.at(0) = p0;
      }

      if ( p0 == "test" )
        message += CommandParserRelease::parseCommand(commands, user, force, debugRls, brief);

      if ( p0 == "tl" )
        message += CommandParserRelease::parseCommand(commands, user, force, debugRls, brief);

      if ( p0 == "login" )
        message += CommandParserRelease::parseCommand(commands, user, force, debugRls, brief);

      if ( p0 == "ll" )
        message += CommandParserRelease::parseCommand(commands, user, force, debugRls, brief);

      if ( p0 == "imdb" )
        message += CommandParserRelease::parseCommand(commands, user, force, debugRls, brief);

      if ( p0 == "tvmaze" )
        message += CommandParserRelease::parseCommand(commands, user, force, debugRls, brief);

      if ( CommandParserIndex::debug ) cout << "post tvmaze" << endl;

      int h_len = 0;

      try
      {
        h_len = stoi(p1);
      }
      catch (...)
      {
        if ( CommandParserIndex::debug ) cout << "history has no lenght";
      }

      if ( h_len > 20 )
        h_len = 20;

      if ( p0 == "history" && h_len == 0 )
        message += History::history(user, 10);

      if ( p0 == "history" && h_len > 0 )
        message += History::history(user, h_len);

    //TODO: function to test with statusbar how many worng things caused some change, maybe modify the caches first to test, then modify the database

      if ( p0 == "help" )
      {
        message += CommandParserHelp::parseCommand(commands, user, force, debugRls, brief);
      }

      if ( p0 == "debug" )
      {
        if ( isAdmin )
	          message += CommandParserDebug::parseCommand(commands, user, force, debugRls, brief);
        if ( !isAdmin )
          message += "you are NOT an ADMIN user, "+ color::rize("ACCESS DENIED", "Red");
      }

      if ( p0 == "autocomplete" )
	      message += CommandParserAutocomplete::parseCommand(commands, user, force, debugRls, brief);

      if ( p0 == "whoami" )
        message += "you are logged in as User " + user;

      if ( p0 == "who" )
        message += "NOT YET PROGRAMMED";

      if ( p0 == "sync" )
        message += CommandParserSync::parseCommand(commands, user, force, debugRls, brief);


      if ( p0 == "user" )
        message += CommandParserUser::parseCommand(commands, user, force, debugRls, brief);
        
      if ( CommandParserTools::isUser(p0) )
        message += CommandParserUser::parseCommand(commands, user, force, debugRls, brief);
      
      //TODO: identical commands with site, eg blacklist
      if ( CommandParserTools::isInVector(p0, cfr->userWithModifier) || CommandParserTools::isInVector(p0, cfr->userWithoutModifier)  )
        message += CommandParserUser::parseCommand(commands, user, force, debugRls, brief);

      //TODO: require admin rights, otherwise users can delete each other, or confirmation message	
      if ( CommandParserTools::isInVector(p0, cfr->verb) && CommandParserTools::isUser(p1) )
        message += CommandParserUser::parseCommand(commands, user, force, debugRls, brief);

        
      if ( p0 == "site" )
        message += CommandParserSite::parseCommand(commands, user, force, debugRls, brief);

      if ( CommandParserTools::isSite(p0) )
        message += CommandParserSite::parseCommand(commands, user, force, debugRls, brief);

      if ( CommandParserTools::isInVector(p0, cfr->verb) && CommandParserTools::isSite(p1) )
        message += CommandParserSite::parseCommand(commands, user, force, debugRls, brief);

      if ( p0 == ".site" )
        message += CommandParserSite::parseCommand(commands, user, force, debugRls, true);

      //affils, size, name ...
      if ( CommandParserTools::isInVector(p0, cfr->siteWithoutModifier) )
        message += CommandParserSite::parseCommand(commands, user, force, debugRls, false);

      //blacklist, rule etc suchen
      if ( CommandParserTools::isInVector(p0, cfr->siteWithModifier) )
        message += CommandParserSite::parseCommand(commands, user, force, debugRls, false);

      string c1 = CommandParserTools::consume(commands);
      string p2 = CommandParserTools::peek(commands);

     return message;
    }

};
#endif
