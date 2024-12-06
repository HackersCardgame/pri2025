#include "../server/hashPassword.cpp"
#include "../commandParser/commandParserSqlFunctions.cpp"
#include "../commandParser/commandParserTools.cpp"


#ifndef COMMAND_PARSER_USER
#define COMMAND_PARSER_USER


using namespace std;

/*******************************************************************************
* does what it says: 
* parses the commands that come from IRC or phaseClient, this is maybe also
* something you need to change over time since you maybe will have new commands
*******************************************************************************/
class CommandParserUser
{

  public:
    inline static const bool debug = false;
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();

    static string parseCommand(vector<string> commands, string user, bool force, bool debug, bool brief)
    {
      if ( CommandParserUser::debug ) cout << "CommandParserUser::parseCommand" << endl;
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


      if ( CommandParserTools::at(commands, 0) == "user" )
        c0 = CommandParserTools::consume(commands);

      string p0 = CommandParserTools::at(commands, 0);
      string p1 = CommandParserTools::at(commands, 1);
      string p2 = CommandParserTools::at(commands, 2);
      string p3 = CommandParserTools::at(commands, 3);
      string p4 = CommandParserTools::at(commands, 4);

//user     //TODO: additional check for block other passwords, shall users be able to access other users config file shrikey?
      if ( c0 == "user" && p0 == "" )
        message += CommandParserSqlFunctions::distinct("configFile", "%", "host", "%", "%", 0, 4);

//host
//user host
//user01 host
//user user01 host
//user user01 cbpassword.....

      string lastCommand = "";
      try
      {
        lastCommand = commands.back();
      }
      catch (...)
      {
        if ( CommandParserUser::debug ) cout << "no further commands" << endl;
      }


      bool lastIsParameter = CommandParserTools::isInVector(lastCommand, cfr->userWithModifier) || CommandParserTools::isInVector(lastCommand, cfr->userWithoutModifier);


      if ( lastIsParameter )
      {
        bool answered = false;
        for ( auto c : commands )
        {
          if ( CommandParserTools::isUser(c) )
          {
            answered = true;
            message += CommandParserSqlFunctions::list(c, commands.back(), "%", "%");
          }
        }
        if ( !answered )
        {
          message += CommandParserSqlFunctions::list(user, commands.back(), "%", "%");
        }
      }


//setpassword  //TODO: maybe in commandParserIndex
      if ( p0 == "setpassword" && p2 == "" )
      {
        CommandParserTools::consume(commands);
        message += CommandParserSqlFunctions::set(user, "password", "", sha256(CommandParserTools::restToString(commands)), user, force, false);
      }



//user01
      if ( CommandParserTools::isUser(p0) && p1 == "" )
        message += CommandParserSqlFunctions::list(p0, "%", "%", "%");

//user01 setpassword  //TODO: set password from other users?
      if ( CommandParserTools::isUser(p0) && p1 == "setpassword" )
      {
        string c0 = CommandParserTools::consume(commands);
        CommandParserTools::consume(commands);
        message += CommandParserSqlFunctions::set(c0, "password", "", sha256(CommandParserTools::restToString(commands)), user, force, false );
      }

//host SET git.0x8.ch
//blacklist SET disney mikey.mouse
      bool p0IsParameter = CommandParserTools::isInVector(p0, cfr->userWithModifier) || CommandParserTools::isInVector(p0, cfr->userWithoutModifier);
      if ( p0IsParameter && p1 == "set" )
      {
        string c0 = CommandParserTools::consume(commands);
        CommandParserTools::consume(commands);  //TODO: ugly
        string value = CommandParserTools::restToString(commands);
        message += CommandParserSqlFunctions::set(user, c0, "", value, user, force, false);
        message += CommandParserSqlFunctions::list(user, c0, "", "%");
      }

//host SET git.0x8.ch
//blacklist APPEND disney mikey.mouse
      if ( p0IsParameter && p1 == "append" )
      {
        string c0 = CommandParserTools::consume(commands);
        CommandParserTools::consume(commands);  //TODO: ugly
        string value = CommandParserTools::restToString(commands);
        message += CommandParserSqlFunctions::append(user, c0, "", value, user, force, false);
        message += CommandParserSqlFunctions::list(user, c0, "", "%");
      }

//host SET git.0x8.ch
//blacklist REMOVE disney mikey.mouse
      if ( p0IsParameter && p1 == "remove" )
      {
        string c0 = CommandParserTools::consume(commands);
        CommandParserTools::consume(commands);
        string value = CommandParserTools::restToString(commands);
        message += CommandParserSqlFunctions::remove(user, c0, "", value, user, force, false);
        message += CommandParserSqlFunctions::list(user, c0, "", "%");
      }

//user01 host SET git.0x8.ch
//user01 blacklist SET disney mikey.mouse
      bool p1IsParameter = CommandParserTools::isInVector(p0, cfr->userWithModifier) || CommandParserTools::isInVector(p0, cfr->userWithoutModifier);
      if ( CommandParserTools::isUser(p0) && p1IsParameter && p2 == "set" )
      {
        string c0 = CommandParserTools::consume(commands);
        string c1 = CommandParserTools::consume(commands);
        string value = CommandParserTools::restToString(commands);
        message += CommandParserSqlFunctions::set(c0, c1, "", value, user, force, false);
        message += CommandParserSqlFunctions::list(c0, c1, "", "%");
      }

//user01 host SET git.0x8.ch
//user01 blacklist SET disney mikey.mouse
      if ( CommandParserTools::isUser(p0) && p1IsParameter && p2 == "append" )
      {
        string c0 = CommandParserTools::consume(commands);
        string c1 = CommandParserTools::consume(commands);
        string value = CommandParserTools::restToString(commands);
        message += CommandParserSqlFunctions::append(c0, c1, "", value, user, force, false);
        message += CommandParserSqlFunctions::list(c0, c1, "", "%");
      }

//user01 host SET git.0x8.ch
//user01 blacklist SET disney mikey.mouse
      if ( CommandParserTools::isUser(p0) && p1IsParameter /*TODO: ugly*/ && p2 == "remove" )
      {
        string c0 = CommandParserTools::consume(commands);
        string c1 = CommandParserTools::consume(commands);
        string value = CommandParserTools::restToString(commands);
        message += CommandParserSqlFunctions::remove(c0, c1, "", value, user, force, false);
        message += CommandParserSqlFunctions::list(c0, c1, "", "%");
      }

//user user01 host SET git.0x8.ch
//user user01 blacklist SET disney mikey.mouse
      bool p2IsParameter = CommandParserTools::isInVector(p0, cfr->userWithModifier) || CommandParserTools::isInVector(p0, cfr->userWithoutModifier);
      if ( p0 == "user" && CommandParserTools::isUser(p1) && p2IsParameter && p3 == "set" )
      {
        CommandParserTools::consume(commands);
        string c1 = CommandParserTools::consume(commands);
        string c2 = CommandParserTools::consume(commands);
        string c3 = CommandParserTools::consume(commands);
        string value = CommandParserTools::restToString(commands);
        message += CommandParserSqlFunctions::set(c1, c2, "", value, user, force, false);
        message += CommandParserSqlFunctions::list(c1, c2, "", "%");
      }


//user01 rule RULE SET x264...
//user01 rule RULE DEL
//user01 rule RULE ADD
//user01 rule RULE APPEND
//user01 rule RULE REMOVE
//user user01 rule RULE SET x264...
//user user01 rule RULE DEL
//user user01 rule RULE ADD
//user user01 rule RULE APPEND
//user user01 rule RULE REMOVE

      string c1 = user;

      if ( CommandParserTools::isUser(p0) )
        string c1 = CommandParserTools::consume(commands);                  //user01

      string m1 = CommandParserTools::at(commands, 0);                      //rule, blacklist, site
      string m2 = CommandParserTools::at(commands, 1);                      //TV, -1, 
      string m3 = CommandParserTools::at(commands, 2);                      //set, del, append, remove

      if ( CommandParserTools::isInVector(m1, cfr->userWithModifier) )
      {
        if ( CommandParserTools::isInVector(m3, cfr->verb) )
        {
          string c2 = CommandParserTools::consume(commands);                //rule, blacklsit, site
          string c3 = CommandParserTools::consume(commands);                //-1, TV, GAMES...
          string c4 = CommandParserTools::consume(commands);                //set, del, append, remove
          
          string value = CommandParserTools::restToString(commands);
          if ( c4 == "add" )
            message += CommandParserSqlFunctions::add(c1, c2, c3, value, user, force);
          if ( c4 == "del" )
            message += CommandParserSqlFunctions::del(c1, c2, c3, value, user, force);
          if ( c4 == "set" )
            message += CommandParserSqlFunctions::set(c1, c2, c3, value, user, force, false);
          if ( c4 == "append" )
            message += CommandParserSqlFunctions::append(c1, c2, c3, value, user, force, false);
          if ( c4 == "remove" )
            message += CommandParserSqlFunctions::remove(c1, c2, c3, value, user, force, false);
          //if ( c4 == "mod" )
            //message += CommandParserSqlFunctions::mod(c1, c2, c3, "", value, "", user, force );


          if ( c1 == "%" && force ) message += CommandParserSqlFunctions::list(c1, c2, c3, "%");
          if ( c1 != "%" ) message += CommandParserSqlFunctions::list(c1, c2, c3, "%");
        }
      }

      bool m1IsParameter = CommandParserTools::isInVector(m1, cfr->userWithModifier) || CommandParserTools::isInVector(m1, cfr->userWithoutModifier);

      if ( m1IsParameter )
      {
        if ( CommandParserTools::isInVector(m2, cfr->verb) )
        {
          string c2 = CommandParserTools::consume(commands);
          string c3 = CommandParserTools::consume(commands);
          
          string value = CommandParserTools::restToString(commands);
          if ( c3 == "add" )
            message += CommandParserSqlFunctions::add(c1, c2, "", value, user, force);
          if ( c3 == "del" )
            message += CommandParserSqlFunctions::del(c1, c2, "", value, user, force);
          if ( c3 == "set" )
            message += CommandParserSqlFunctions::set(c1, c2, "", value, user, force, false);
          if ( c3 == "append" )
            message += CommandParserSqlFunctions::append(c1, c2, "", value, user, force, false);
          if ( c3 == "remove" )
            message += CommandParserSqlFunctions::remove(c1, c2, "", value, user, force, false);
          if ( c3 == "mod" )
            message += CommandParserSqlFunctions::mod(c1, c2, c3, value, user, force, true);


          if ( c1 == "%" && force ) message += CommandParserSqlFunctions::list(c1, c2, "%", "%");
          if ( c1 != "%" ) message += CommandParserSqlFunctions::list(c1, c2, "%", "%");
        }
      }

      return message;
    }
};
#endif
