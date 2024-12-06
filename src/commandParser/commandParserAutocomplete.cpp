
#include "../commandParser/commandParserSqlFunctions.cpp"
#include "../commandParser/commandParserTools.cpp"


#ifndef COMMAND_PARSER_AUTOCOMPLETE
#define COMMAND_PARSER_AUTOCOMPLETE


using namespace std;

/*******************************************************************************
* does what it says: 
* parses the commands that come from IRC or phaseClient, this is maybe also
* something you need to change over time since you maybe will have new commands
*******************************************************************************/
class CommandParserAutocomplete
{

  public:
    inline static const bool debug = false;
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();


    static string parseCommand(vector<string> commands, string user, bool force, bool debug, bool brief)
    {
      if ( CommandParserAutocomplete::debug ) cout << "CommandParserAutocomplete::parseCommand()" << endl;
      string message = "";

      string p0 = CommandParserTools::at(commands, 0);
      string p1 = CommandParserTools::at(commands, 1);
      string p2 = CommandParserTools::at(commands, 2);
      string p3 = CommandParserTools::at(commands, 3);
      string p4 = CommandParserTools::at(commands, 4);
      string p5 = CommandParserTools::at(commands, 5);


      if ( p0 == "autocomplete" )
      {
        string defaultCommands = "";
        for ( auto c : cfr->allCommands )
          defaultCommands += "," + c;

        if ( p1 == "" )
          message += "autocomplete:"+defaultCommands.substr(1)+":";


        if ( p1 == "debug" )
          message += "autocomplete:"+defaultCommands+",debug checkall,debug general,debug generalcache:";

        if ( p1 == "site" && p2 == "")
        {
          string sCommands = "";
          for ( auto siteCache : cfr->siteCaches )
          {
            vector<string> s = RegexFunctions::split('.', siteCache.first);
            sCommands += ",site " + s.at(0);  //remove .cfg
          }
          message += "autocomplete:" + sCommands + ":";
        }

        if ( p1 == "site" && CommandParserTools::isSite(p2) && p3 == "" )
        {
          string sCommands = "";
          vector<string> thirdCommands = {};
          for ( auto v : cfr->verb )
            thirdCommands.push_back(v);
          for ( auto v : cfr->siteWithModifier )
            thirdCommands.push_back(v);
          for ( auto v : cfr->siteWithoutModifier )
            thirdCommands.push_back(v);
          for ( auto t : thirdCommands )
            sCommands += ",site " + p2 + " " + t; 
          
          message += "autocomplete:" + sCommands + ":";
        }

        if ( p1 == "site" && CommandParserTools::isSite(p2) && CommandParserTools::isInVector(p3, cfr->siteWithModifier) )
        {
          string sCommands = "";
          string modifier = CommandParserSqlFunctions::distinct("modifier", p2, p3, "%", "", 0);
          string colorStriped = RegexFunctions::replace("\\x1b\[[0-9;]*[a-zA-Z]", "", modifier);  //XXX: REMOVE THE COLOR INFO for IRC
          string noSpaces = RegexFunctions::replace(", ", ",", colorStriped);
          vector<string> elements = RegexFunctions::split(',', noSpaces);

          if ( p4 == "" )
          {
            for ( auto e : elements )
              sCommands += ",site " + p2 + " " + p3 + " " + e; 
          
            message += "autocomplete:" + sCommands.substr(1) + ":";
          }
          
          if ( CommandParserTools::isInVector(p4, elements) && p5 == "remove" )
          {
            string line = CommandParserSqlFunctions::list(p2, p3, p4, "");
            string colorStriped = RegexFunctions::replace("\\x1b\[[0-9;]*[a-zA-Z]", "", line);  //XXX: REMOVE THE COLOR INFO for IRC
            string newlineStriped = RegexFunctions::replace("\n", "", colorStriped);  //XXX: REMOVE THE COLOR INFO for IRC
            cout << "L: " << newlineStriped << endl;
            vector<string> lineElements = RegexFunctions::split('=', newlineStriped);
            string term = RegexFunctions::trim(lineElements.at(1));
            cout << "T: " << term << endl;
            vector<string> items = RegexFunctions::split(' ', term);
            
            for ( auto i : items )
            {
              if ( i != "" )
              {
                sCommands += ",site " + p2 + " " + p3 + " " + p4 + " " + p5 + " " + i; 
              }
            }
            message += "autocomplete:" + sCommands.substr(1) + ":";
            cout << message << endl;
          }
          
        }

      }

      return message;
    }
};
#endif

