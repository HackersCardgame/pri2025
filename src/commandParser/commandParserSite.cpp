#include "../commandParser/commandParserSqlFunctions.cpp"
#include "../commandParser/commandParserTools.cpp"


#ifndef COMMAND_PARSER_SITE
#define COMMAND_PARSER_SITE


//TODO: maybe keyword like >> allblacklist << to finde things better

using namespace std;

/*******************************************************************************
* does what it says: 
* parses the commands that come from IRC or phaseClient, this is maybe also
* something you need to change over time since you maybe will have new commands
*******************************************************************************/
class CommandParserSite
{

  //TODO: maybe use quotes to denote the values  +could ignore order, additional key to enter, append and remove would be stupid to write ... append "tv"

  public:
    inline static const bool debug = false;
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();


    static string parseCommand(vector<string> commands, string user, bool force, bool debug, bool brief)
    {
      if ( CommandParserSite::debug ) cout << "CommandParserSite::parseCommand()" << endl;
      string message = "";

      for ( auto &c : commands )
      {
        if ( c == "+" ) c = "add";
        if ( c == "-" ) c = "del";
        if ( c == "=" ) c = "set";
        if ( c == "+=" ) c = "append";
        if ( c == "-=" ) c = "remove";
      }

      string c0 = "";

      if ( CommandParserTools::at(commands, 0) == "site" || CommandParserTools::at(commands, 0) == ".site" )
        c0 = CommandParserTools::consume(commands);

      string p0 = CommandParserTools::at(commands, 0);
      string p1 = CommandParserTools::at(commands, 1);
      string p2 = CommandParserTools::at(commands, 2);
      string p3 = CommandParserTools::at(commands, 3);
      string p4 = CommandParserTools::at(commands, 4);

      //TODO: affils with no parameter shows all affils rows?
      //TODO: limit output in irc or ask for -f to display all, or automatically add dot like .command .site or automatically set biref 
      
//affils, name, location...
      if ( CommandParserTools::isInVector(p0, cfr->siteWithoutModifier) && p1 == "" )
      {
        if ( CommandParserSite::debug ) cout << "affils, name, location..." << endl;
        message += CommandParserSqlFunctions::list("%", p0, "%", "%");
      }

      if ( CommandParserTools::isInVector(p0, cfr->siteWithoutModifier) && p1 != "" )
      {
        if ( CommandParserSite::debug ) cout << "1 both, rule, dir, name, affils..." << endl;
        message += CommandParserSqlFunctions::list("%", p0, "%", p1);
      }


      if ( CommandParserTools::isInVector(p0, cfr->siteWithModifier) && p1 == "" )
      {
        if ( CommandParserSite::debug ) cout << "both, rule, dir, name, affils..." << endl;
        message += CommandParserSqlFunctions::distinct("modifier", "%", p0, "%", "%", 0);

      }

      if ( CommandParserTools::isInVector(p0, cfr->siteWithModifier) && p1 != "" )
      {
        if ( CommandParserSite::debug ) cout << "2 single, rule, dir" << endl;
        if ( p2 == "" )
          message += CommandParserSqlFunctions::list("%", p0, "%"+p1+"%", "%");
        else
          message += CommandParserSqlFunctions::list("%", p0, "%"+p1+"%", p2);
      }

      if ( CommandParserTools::isInVector(p0, cfr->siteWithModifier) && p1 == "*" && p2 != "")
      {
        message += CommandParserSqlFunctions::list("%", p0, "%", "%"+p2+"%");
      }

//site
      if ( c0 == "site" && p0 == "" )
        message += CommandParserSqlFunctions::distinct("configFile", "%", "rule", "%", "%", 0, 4);  //TODO: this way sites where one deleted all rules are inaccessable

//XYZ
//site XYZ
      if ( CommandParserTools::isSite(p0) && p1 == "" )
      {
        message += CommandParserSqlFunctions::list(p0, "%", "%", "%");
      }

      if ( c0 == ".site" && CommandParserTools::isSite(p0) && p1 == "" )
      {
        message += CommandParserSqlFunctions::list(p0, "name", "%", "%");
        message += CommandParserSqlFunctions::list(p0, "location", "%", "%");
        message += CommandParserSqlFunctions::list(p0, "country", "%", "%");
        message += CommandParserSqlFunctions::list(p0, "size", "%", "%");
        message += CommandParserSqlFunctions::list(p0, "isp", "%", "%");
        message += CommandParserSqlFunctions::list(p0, "control", "%", "%");
        message += CommandParserSqlFunctions::list(p0, "sitebans", "%", "%");
        message += CommandParserSqlFunctions::list(p0, "status", "%", "%");
        message += CommandParserSqlFunctions::list(p0, "affils", "%", "%");
        message += CommandParserSqlFunctions::distinct("modifier", p0, "rule", "%", user, 0);
        message += CommandParserSqlFunctions::distinct("modifier", p0, "dir", "%", user, 0);
      }

//del XYZ
//site del xYZ
        if ( p0 == "del" && CommandParserTools::isSite(p1) && ( p2 == "" ) )
        {
          message += CommandParserSqlFunctions::del(p1, "%", "%", "%", "%", force );
        }
//add XYZ
//site add xYZ
        if ( p0 == "add" && p1 != "" && p2 == "" )
        {
          message += CommandParserSqlFunctions::add(p1, "name", "", "added by " + user, user, force );
          message += CommandParserSqlFunctions::add(p1, "rule", "games", "games std", user, force );
          message += CommandParserSqlFunctions::add(p1, "dir", "games", "/incoming/games", user, force );
          message += CommandParserSqlFunctions::list(p1, "%", "%", "%");
        }

//XYZ name
//XYZ rule
//XYZ courier
//site XYZ name
//site XYZ rule
//site XYZ courier
        bool p1isParameter = CommandParserTools::isInVector(
             p1, cfr->siteWithModifier) ||
             CommandParserTools::isInVector(
             p1, cfr->siteWithoutModifier);


        if ( CommandParserTools::isSite(p0) && p1isParameter && p2 == "" )
          message += CommandParserSqlFunctions::list(p0, p1, "%", "%");
          
      //TODO: Fucing bad word for this function, better meybe group
      if ( p0 == "contains" && p1 != "" )
      {
        message += CommandParserSqlFunctions::list("%", "affils", "%", p1);
        message += CommandParserSqlFunctions::list("%", "blacklist", "%", p1);
      }

      if ( p0 == "section" && p1 != "" )
      {
        message += CommandParserSqlFunctions::list("%", "rule", "%", p1);
      }



//XYZ rule RULE
//XYZ dir DIR
//site XYZ rule RULE
//site XYZ dir DIR
      if ( CommandParserTools::isSite(p0) && CommandParserTools::isInVector(p1, cfr->siteWithModifier) && p2 != "" && p3 == "" )  //TODO: maybe check if parameter with modifier, what with blacklist 
        message += CommandParserSqlFunctions::list(p0, p1, p2, "%");


//BBR ADD name (asdf asdf)
//site BBR ADD name (asdf asdf)
      if ( CommandParserTools::isSite(p0) && p1 == "add" && CommandParserTools::isInVector( p2, cfr->siteWithoutModifier ) )
      {
        string c1 = CommandParserTools::consume(commands);
        string c2 = CommandParserTools::consume(commands);
        string c3 = CommandParserTools::consume(commands);
        message += "c1: " + c1 + " c2: " + c2 + " c3: " + c3 + "\n";
        string value = CommandParserTools::restToString(commands);
        message += CommandParserSqlFunctions::add(c1, c3, "", value, user, force);
        message += CommandParserSqlFunctions::list(c1, c3, "%", "%"); //TODO: check
      }

//BBR DEL name
//site BBR DEL name
      if ( CommandParserTools::isSite(p0) && p1 == "del" && CommandParserTools::isInVector(p2, cfr->siteWithoutModifier) )
      {
        string value = CommandParserTools::restToString(commands);
        message += CommandParserSqlFunctions::del(p0, p2, "", value, user, force);
        message += CommandParserSqlFunctions::list(p0, p2, "", "%");
      }


//BBR name SET blablabla asdf
//site BBR name SET blablabla asdf
      if ( CommandParserTools::isSite(p0) && CommandParserTools::isInVector(p1, cfr->siteWithoutModifier) && p2 == "set" )
      {
        string c1 = CommandParserTools::consume(commands);
        string c2 = CommandParserTools::consume(commands);
        string c3 = CommandParserTools::consume(commands);
        string value = CommandParserTools::restToString(commands);
        message += CommandParserSqlFunctions::set(c1, c2, "", value, user, force, false);
        message += CommandParserSqlFunctions::list(c1, c2, "", "%");
      }


//XYZ rule ADD RULE
//site XYZ rule ADD RULE
      if ( CommandParserTools::isSite(p0) && CommandParserTools::isInVector(p1, cfr->siteWithoutModifier) && p2 == "add" && p3 != "" )
      {
        string c1 = CommandParserTools::consume(commands);
        string c2 = CommandParserTools::consume(commands);
        string c3 = CommandParserTools::consume(commands);
        string c4 = CommandParserTools::consume(commands);
        string value = CommandParserTools::restToString(commands);  //TODO: better an "already exists" message
        message += CommandParserSqlFunctions::add(c1, c2, c4, value, user, force);
        message += CommandParserSqlFunctions::list(c1, c2, "", "%");
      }

//XYZ rule DEL RULE      
//site XYZ rule DEL RULE      
      if ( CommandParserTools::isSite(p0) && CommandParserTools::isInVector(p1, cfr->siteWithoutModifier) && p2 == "del" && p3 != "" )
      {
//TODO: for ...
// site XYZ rule DEL RULE1 RULE2 RULE3
        message += CommandParserSqlFunctions::del(p0, p1, p3, "", user, force);
        message += CommandParserSqlFunctions::list(p0, p1, "", "%");
      }
      

//XYZ rule RULE SET x264...
//XYZ rule RULE DEL
//XYZ rule RULE ADD
//XYZ rule RULE APPEND
//XYZ rule RULE REMOVE
//site XYZ rule RULE SET x264...
//site XYZ rule RULE DEL
//site XYZ rule RULE ADD
//site XYZ rule RULE APPEND
//site XYZ rule RULE REMOVE


      if ( CommandParserTools::isSite(p0) && /* ( CommandParserTools::isInVector(p1, cfr->siteWithoutModifier) ||*/ CommandParserTools::isInVector(p1, cfr->siteWithModifier) )
      {
        if ( CommandParserTools::isInVector(p2, cfr->verb) )
        {
          string c1 = CommandParserTools::consume(commands);
          string c2 = CommandParserTools::consume(commands);
          string c3 = CommandParserTools::consume(commands);
          string c4 = CommandParserTools::consume(commands);
          string value = CommandParserTools::restToString(commands);

          message += "1: c1: " + c1 + " c2: " + c2 + " c3: " + c3 + " c4: " + c4 + "\n";

          if ( c3 == "add" )
            message += CommandParserSqlFunctions::add(c1, c2, c4, value, user, force);  //TODO: rule add TV.720, rule del TV.720 XXX BUT: site rule TV.720 append tv or remove
          if ( c3 == "del" )
            message += CommandParserSqlFunctions::del(c1, c2, c4, value, user, force);


          if ( c1 == "%" && force ) message += CommandParserSqlFunctions::list(c1, c2, "%", "%");
          if ( c1 != "%" ) message += CommandParserSqlFunctions::list(c1, c2, "%", "%");
        }
      }

      if ( CommandParserTools::isSite(p0) && /* ( CommandParserTools::isInVector(p1, cfr->siteWithoutModifier) || */ CommandParserTools::isInVector(p1, cfr->siteWithModifier) )
      {
        if ( CommandParserTools::isInVector(p3, cfr->verb) )
        {
          string c1 = CommandParserTools::consume(commands);
          string c2 = CommandParserTools::consume(commands);
          string c3 = CommandParserTools::consume(commands);
          string c4 = CommandParserTools::consume(commands);
          string value = CommandParserTools::restToString(commands);

          message += "2 c1: " + c1 + " c2: " + c2 + " c3: " + c3 + " c4: " + c4 + "\n";

          if ( c4 == "set" )
            message += CommandParserSqlFunctions::set(c1, c2, c3, value, user, force, true);
          if ( c4 == "append" )
            message += CommandParserSqlFunctions::append(c1, c2, c3, value, user, force, true);
          if ( c4 == "remove" )
            message += CommandParserSqlFunctions::remove(c1, c2, c3, value, user, force, true);
          //if ( c4 == "mod" )
            //message += CommandParserSqlFunctions::mod(c1, c2, c3, c4, value, "", user, force );


          if ( c1 == "%" && force ) message += CommandParserSqlFunctions::list(c1, c2, "%", "%");
          if ( c1 != "%" ) message += CommandParserSqlFunctions::list(c1, c2, "%", "%");
        }
      }

      return message;
    }
};
#endif

