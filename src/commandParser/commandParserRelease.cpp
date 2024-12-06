
#include "../processRelease/lookupImdb.cpp"
#include "../processRelease/lookupTvmaze.cpp"
#include "../processRelease/lookupDupe.cpp"

#include "../commandParser/commandParserSqlFunctions.cpp"
#include "../commandParser/commandParserTools.cpp"


#include "../commandParser/history.cpp"


#ifndef COMMAND_PARSER_RELEASE
#define COMMAND_PARSER_RELEASE


using namespace std;

/*******************************************************************************
* does what it says: 
* parses the commands that come from IRC or phaseClient, this is maybe also
* something you need to change over time since you maybe will have new commands
*******************************************************************************/
class CommandParserRelease
{

  public:
    inline static const bool debug = false;
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();


    inline static ParseRelease pr = ParseRelease();  //TODO REMOVE OTHRE PLACES


    static string parseCommand(vector<string> commands, string user, bool force, bool debug, bool brief)
    {
      if ( CommandParserRelease::debug ) cout << "CommandParserRelease::parseCommand()" << endl;

      string message = "";

      string c1 = CommandParserTools::consume(commands);

      string p1 = CommandParserTools::peek(commands);

      if ( CommandParserRelease::debug ) cout << "consumed" << endl;
     
//test matrix.1999.x264.1080p.web-ASDF
      if ( p1 == "-d" )   //ugly
      {
        CommandParserTools::consume(commands);
        debug = true;
      }

      if ( CommandParserRelease::debug ) cout << "post debug flag" << endl;
      
//test matrix.1999.x264.1080p.web-ASDF
      if ( c1 == "test" )
      {
        Release rls = Release(CommandParserTools::restToString(commands));
        //cout << "going to parse release now" << endl;
        rls.log = debug;
        rls.allErrors = debug;
        pr.parse(rls, false);
        message +=  rls.sprint("all");
      }

      if ( CommandParserRelease::debug ) cout << "post test" << endl;

//test matrix.1999.x264.1080p.web-ASDF
      if ( c1 == "tl" )
      {
        Release rls = History::lastRelease(user);
        //cout << "going to parse release now" << endl;
        rls.log = debug;
        rls.allErrors = debug;
        pr.parse(rls, false);
        message +=  rls.sprint("all");
      }

      if ( CommandParserRelease::debug ) cout << "post tl" << endl;

//login matrix.1999.x264.1080p.web-ASDF
      if ( c1 == "login" )
      {
        Release rls = Release(CommandParserTools::restToString(commands));
        rls.login = true;
        rls.overrideDupeCheck = true;
        //cout << "going to parse release now" << endl;
        rls.log = debug;
        rls.allErrors = debug;
        pr.parse(rls, false);
        message +=  rls.sprint("all");
      }

      if ( CommandParserRelease::debug ) cout << "post login" << endl;

//login matrix.1999.x264.1080p.web-ASDF
      if ( c1 == "ll" )
      {
        Release rls = History::lastRelease(user);
        rls.login = true;
        rls.overrideDupeCheck = true;
        //cout << "going to parse release now" << endl;
        rls.log = debug;
        rls.allErrors = debug;
        pr.parse(rls, false);
        message +=  rls.sprint("all");
      }

      if ( CommandParserRelease::debug ) cout << "post ll" << endl;

//imdb the matrix      //TODO: year  
      if ( c1 == "imdb" )
      {          
        message += getImdb(commands);
      }
      
      if ( CommandParserRelease::debug ) cout << "post imdb" << endl;

//tvmaze big bang theory
      if ( c1 == "tvmaze" )
      {
        message += getTvmaze(commands);
      }

      if ( CommandParserRelease::debug ) cout << "post tvmaze" << endl;
      
      return message;
    }
    
    
    static string getImdb(vector<string> commands)
    {
      string message = "";

      int year = 0;
      try
      {
        year = stoi(CommandParserTools::peek(commands));
        CommandParserTools::consume(commands);
      }
      catch (...)
      {
        cout << "stoi, not a year" << endl;
      }
      
      try
      {
        year = stoi(commands.back());
        commands.erase(commands.end());
      }
      catch (...)
      {
        cout << "stoi, not a year" << endl;
      }

      vector<string> movie = LookupImdb::get(CommandParserTools::restToString(commands), year);
      for ( auto item : movie )
        message +=  ", " + item;
        
      return message.substr(2);
    }
    

    static string getTvmaze(vector<string> commands)
    {
      if ( CommandParserRelease::debug ) cout << "getTvmaze()" << endl;

      string message = "";

      vector<string> show;

      string episode = "s01e01";
      
      if ( commands.size() > 0 )
        episode = commands.back();
      else
        cout << "no valid season/episode" << endl;

      if ( CommandParserRelease::debug ) cout << "pre findall" << endl;

      vector<string> tvep = RegexFunctions::findAll("S\\d+e\\d+", episode);

      if ( CommandParserRelease::debug ) cout << "tvep" << endl;
      
      if ( tvep.size() > 0 )
      {
        if ( CommandParserRelease::debug ) cout << "tvep.size()>0" << endl;

        if ( commands.size() > 0 )
          commands.erase(commands.end());
          
        show = LookupTvmaze::get(CommandParserTools::restToString(commands), tvep[0], 0, "");
      }
      else
        show = LookupTvmaze::get(CommandParserTools::restToString(commands), "s01E01", 0, "");

      if ( CommandParserRelease::debug ) cout << "post if" << endl;
        
  //TODO: 1999 and US from releaseName
      for ( auto item : show )
        message +=  ", " + item;
      return message.substr(2);
    }


};
#endif
