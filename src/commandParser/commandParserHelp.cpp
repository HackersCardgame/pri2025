#include "../commandParser/commandParserSqlFunctions.cpp"
#include "../commandParser/commandParserTools.cpp"
#include "../common/helperFunctions.cpp"



#ifndef COMMAND_PARSER_HELP
#define COMMAND_PARSER_HELP


using namespace std;

/*******************************************************************************
* does what it says: 
* parses the commands that come from IRC or phaseClient, this is maybe also
* something you need to change over time since you maybe will have new commands
*******************************************************************************/
class CommandParserHelp
{

  public:
    inline static const bool debug = false;
    
    inline static vector<string> availabeCommands = { "main", "general", "help", "debug", "import", "export", "affils" };
    
    static string parseCommand(vector<string> commands, string user, bool force, bool debug, bool brief)
    {
      if ( CommandParserHelp::debug ) cout << "CommandParserHelp::parseCommand()" << endl;

      string message = "";
      string c0 = CommandParserTools::consume(commands);
      string c1 = CommandParserTools::consume(commands);
      string c2 = CommandParserTools::consume(commands);

      message += CommandParserTools::typo(c0, availabeCommands);
      message += CommandParserTools::typo(c1, availabeCommands);
      message += CommandParserTools::typo(c2, availabeCommands);

//CASE: help
//CASE: help main
      if ( c1 == "" or c1 == "main" )
      {
        message += "Main Help, TODO" + color::rize(" shrikey ", "Yellow" ) + "said he will do that on 2019-07-19\n\n";
        message += color::rize("help ", "Blue") + color::rize("import", "Yellow" ) + " explains how to import\n";
        message += color::rize("help ", "Blue") + color::rize("export", "Yellow" ) + " explains how to export\n";
      }


      if ( c1 == "import" || c1 == "export" )
      {
        message += "you can export config files with the python tools: \n" +
                  color::rize("user: ~/pri42/tools/install$", "Blue") + color::rize(" python3 dbConfigFiles.py --export user01.cfg", "Green") + "\n";
        message += "you can import config files with the python tools: \n" +
                  color::rize("user: ~/pri42/tools/install$", "Blue") + color::rize(" python3 dbConfigFiles.py --import user01.cfg", "Green") + "\n";
        message += "you can export config files with the python tools: \n" + 
                  color::rize("user: ~/pri42/tools/install$", "Blue") + color::rize(" python3 dbConfigFiles.py --export all", "Green") + "\n";
        message += "you can import config files with the python tools: \n" +
                  color::rize("user: ~/pri42/tools/install$", "Blue") + color::rize(" python3 dbConfigFiles.py --import all", "Green") + "\n";

      }
      
//CASE: help debug
      if ( c1 == "debug" )
      {

        if ( c2 == "" )
          message = "Debug Help, TODO" + color::rize(" shrikey ", "Yellow" ) + "said he will do that on 2019-07-19";
          
        if ( c2 == "general" )
          message = "Debug Help (genera.cfg), shows general.cfg, TODO" + color::rize(" shrikey ", "Yellow" ) + "said he will do that on 2019-07-19";
      }
      return message;
    }


};

#endif
