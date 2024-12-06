#include "../commandParser/commandParserSqlFunctions.cpp"
#include "../commandParser/commandParserTools.cpp"


#ifndef COMMAND_PARSER_SYNC
#define COMMAND_PARSER_SYNC


using namespace std;

/*******************************************************************************
* does what it says: 
* parses the commands that come from IRC or phaseClient, this is maybe also
* something you need to change over time since you maybe will have new commands
*******************************************************************************/
class CommandParserSync
{

  public:
    inline static const bool debug = false;


    static string parseCommand(vector<string> commands, string user, bool force, bool debug, bool brief)
    {
      string message = "";

      string c1 = CommandParserTools::consume(commands);

      if ( c1 == "" || c1 == "all" )
      {
        string userConfig = user + ".cfg";
        message += CbftpConfigSetter::sendAllSites( userConfig );
      }
      else
      if ( CommandParserTools::isSite(c1) )
      {
        string userConfig = user + ".cfg";
        message += CbftpConfigSetter::sendSite(userConfig, c1);
      }
      else
        message += "site does not exist\n";
      return message;

    }
};
#endif
