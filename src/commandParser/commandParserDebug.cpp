
#include "../common/configFileReader.cpp"


#include "../commandParser/commandParserSqlFunctions.cpp"
#include "../commandParser/commandParserTools.cpp"


#ifndef COMMAND_PARSER_DEBUG
#define COMMAND_PARSER_DEBUG


using namespace std;

/*******************************************************************************
* does what it says: 
* parses the commands that come from IRC or phaseClient, this is maybe also
* something you need to change over time since you maybe will have new commands
*******************************************************************************/
class CommandParserDebug
{

  public:
    inline static const bool debug = false;
    inline static DbConnection* dbc = DbConnection::getInstance();
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();
    
    inline static vector<string> debugCommands = { "dbstatus", "checkall", "restart", "updatecache", "general", "generalcache", "usercache", "sitecache" };

    static string parseCommand(vector<string> commands, string user, bool force, bool debug, bool brief)
    {
      if ( CommandParserDebug::debug ) cout << "CommandParserDebug::parseCommand()" << endl;
      string message = "";
      
      string p0 = CommandParserTools::at(commands, 0);  //debug
      string p1 = CommandParserTools::at(commands, 1);  //command here:

      if ( CommandParserDebug::debug ) cout << "peeked" << endl;
      
      if ( p1 == "dbstatus" )
        message += checkTables();

      if ( p1 == "checkall" )
        message += checkAll();

      if ( p1 == "restart" )  //TODO; maybe try catch in tcpserver.cpp
        message += p1.substr(100);

      if ( p1 == "updatecache" )
        message += "updating Cache " + cfr->buildCacheAsync();  //TODO: ugly

      if ( p1 == "general" )
        message += CommandParserSqlFunctions::list("general", "%", "%", "%");

      if ( CommandParserDebug::debug ) cout << "halve" << endl;

//DEBUG INTERFACE
      if ( p1 == "generalcache" )
      {
        if ( CommandParserDebug::debug ) cout << "generalcache" << endl;

        string c00 = CommandParserTools::consume(commands);  //TODO: ugly
        string c0 = CommandParserTools::consume(commands);
        string c1 = CommandParserTools::consume(commands);
        string c2 = CommandParserTools::consume(commands);

        if ( c1 == "" && c2 == "" )
        {
          cout << "no parameter" << endl;
          for ( auto item : cfr->generalCache.content)
            message += item.first + "\n";
          return "DBG:\n" + message;
        }
        if ( c2 == "" )
        {
          cout << "no second parameter" << endl;

          for ( auto item : cfr->generalCache.content[c1])
            message += item.first + "\n";
          return "DBG:\n" + message;
        }
        message += cfr->generalCache.content[c1][c2];
        return "DBG:\n" + message;
      }


      if ( p1 == "usercache" )
      {
        if ( CommandParserDebug::debug ) cout << "usercache" << endl;

        string c00 = CommandParserTools::consume(commands);  //TODO: ugly
        string c0 = CommandParserTools::consume(commands);
        string c1 = CommandParserTools::consume(commands);
        string c2 = CommandParserTools::consume(commands);
        string c3 = CommandParserTools::consume(commands);

        if ( CommandParserDebug::debug ) cout << "usercache consumed" << endl;

        if ( c1 == "" && c2 == "" && c3 == "" )
        {
          if ( CommandParserDebug::debug ) cout << "no param" << endl;

          for ( auto item : cfr->userCaches)
          {
            cout << item.first << " " << endl;
            message += item.first + "\n";
            
          }
          return "DBG:\n" + message;
        }
        if ( c2 == "" && c3 == "" )
        {
          if ( CommandParserDebug::debug ) cout << "1 param" << endl;

          for ( auto item : cfr->userCaches[c1]->content)
          {
            cout << item.first << " " << endl;
            message += item.first + "\n";
          }
          return "DBG:\n" + message;
        }
        if ( c3 == "" )
        {
          if ( CommandParserDebug::debug ) cout << "2 param" << endl;

          for ( auto item : cfr->userCaches[c1]->content[c2])
            message += item.first + "\n";
          return "DBG:\n" + message;
        }
        
        message += cfr->userCaches[c1]->content[c2][c3];
        return "DBG:\n" + message;
      }

      if ( p1 == "sitecache" )
      {
        if ( CommandParserDebug::debug ) cout << "sitechace" << endl;

        string c00 = CommandParserTools::consume(commands);
        string c0 = CommandParserTools::consume(commands);
        string c1 = CommandParserTools::consume(commands);
        string c2 = CommandParserTools::consume(commands);
        string c3 = CommandParserTools::consume(commands);

        if ( c1 == "" && c2 == "" && c3 == "" )
        {
          for ( auto item : cfr->siteCaches)
            message += item.first + "\n";
          return "DBG:\n" + message;
        }
        if ( c2 == "" && c3 == "" )
        {
          for ( auto item : cfr->siteCaches[c1]->content)
            message += item.first + "\n";
          return "DBG:\n" + message;
        }
        if ( c3 == "" )
        {
          for ( auto item : cfr->siteCaches[c1]->content[c2])
            message += item.first + "\n";
          return "DBG:\n" + message;
        }
        message += cfr->siteCaches[c1]->content[c2][c3];
        return "DBG:\n" + message;
      }

    return message;
    }
    

    static string checkTables()
    {
      string message = "";
      message += checkTable(ConfigFileReader::configTable, "general");
      message += checkTable("title_basics_tsv", "");
      message += checkTable("title_akas_tsv", "");
      message += checkTable("title_ratings_tsv", "");
      message += checkTable("basics", "");
      message += checkTable("akas", "");
      message += checkTable("ratings", "");
      message += checkTable("basics_small", "");
      message += checkTable("akas_small", "");
      message += checkTable("ratings_small", "");
      message += checkTable("tmdb", "");
      message += checkTable("mojo", "");
      message += checkTable("tvmaze", "");
      message += checkTable("episodes", "");
      message += checkTable("dupes2", "");
      
      return message;
      
    }


    static string checkTable(string table, string configFile)
    {
      string message = "";
      string sqlQuery = "";
      if ( configFile == "" )
        sqlQuery = "SELECT count(*) from " + table + " ;";
      else
        sqlQuery = "SELECT count(*) from " + table + " where configFile like \""+configFile+".cfg\" ;";
        
      MYSQL_RES* result = dbc->fetch(sqlQuery);      //TODO WTF!!!!
      ARRAY_WITH_SIZE array = dbc->toArray(result);
      int count = 0;
      try
      {
        count = stoi(array.array[0][0]);
      }
      catch (...)
      {
        cout << "Table Error " << table << " " << configFile << endl;
      }
        
      if ( count > 10 )
        message += color::rize(table + ": " + configFile + " " + to_string(count), "White", "Green");
      else
        message += color::rize(table + ": " + configFile + " " + to_string(count), "White", "Red") + " you'll find the tools in <GITREPO>/tools/install/";

      message += "\n";
      dbc->freeResult(result);
      dbc->freeArray(array.array, array.rows, array.cols);

      //if ( CommandParserDebug::debug ) message += color::rize(sqlQuery, "Red") + "\n";
      
      return message;
    }


    //TODO: maybe use commandParserSqlFunctions - functions //TODO: description, or maybe chackAllRules()
    static string checkAll()
    {
      string message = "";
      string sqlQuery = "SELECT configFile, parameter, modifier, term, notes, lineNumber from " + ConfigFileReader::configTable + " where parameter='rule';";        
      MYSQL_RES* result = dbc->fetch(sqlQuery);
      ARRAY_WITH_SIZE array = dbc->toArray(result);
      
      if ( CommandParserDebug::debug ) cout << "S: " << array.rows << " " << array.cols << endl;
      
      for ( int i = 0; i < array.rows; i++)
      {
        string term = array.array[i][3];

        if ( term == "" )  //TODO: ugly
        {
          message += color::rize(array.array[i][0], "Cyan") + " " + color::rize(array.array[i][1], "Yellow") + " " +  color::rize(array.array[i][2], "Cyan") + "\n" + ">EMPTY RULE<" + "\n\n";
          continue;
        }
        string checkedTerm = CheckRuleSyntax::checkSyntax(term);

        if ( term != checkedTerm )
          message += color::rize(array.array[i][0], "Cyan") + " " + color::rize(array.array[i][1], "Yellow") + " " +  color::rize(array.array[i][2], "Cyan") + "\n" + checkedTerm + "\n\n";
      }

      dbc->freeResult(result);
      dbc->freeArray(array.array, array.rows, array.cols);

      return message;
    }



};
  
#endif



