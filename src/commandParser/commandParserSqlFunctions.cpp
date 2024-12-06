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

#include "../commandParser/commandParserTools.cpp"


#ifndef COMMAND_PARSER_SQL_FUNCTIONS
#define COMMAND_PARSER_SQL_FUNCTIONS


using namespace std;

/*******************************************************************************
* does what it says: 
* parses the commands that come from IRC or phaseClient, this is maybe also
* something you need to change over time since you maybe will have new commands
*******************************************************************************/
class CommandParserSqlFunctions
{

  public:
    inline static const bool debug = true;



    inline static DbConnection* dbc = DbConnection::getInstance();
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();
    inline static ParseRelease pr = ParseRelease();

    //TODO: maybe better user parameter and site parameter
    

//description above  //TODO: command history (existing)? changelog table per tuple? like config2 but additional user and datetime stamp, primary key datetime
    static vector<vector<string>> select( string what,  //"distinct(configFile)", "distinct(parameter)", "distinct(modifier)", "configFile, parameter, modifier, term, notes, lineNumber"
				  	  				                	  string configFile, string parameter, string modifier, string value, string user )  //TODO: order by
    {
      what = dbc->sanitize(what);
      configFile = dbc->sanitize(configFile);
      parameter = dbc->sanitize(parameter);
      modifier = dbc->sanitize(modifier);
      value = dbc->sanitize(value);
      user = dbc->sanitize(user);

      string sqlQuery = "SELECT " + what + " FROM " + ConfigFileReader::configTable + " WHERE " + "configFile LIKE '" + configFile + ".cfg'" +
                                                                                             " AND parameter LIKE '" + parameter + "'" +
                                                                                             " AND modifier LIKE '" + modifier + "'" +
                                                                                             " AND term LIKE '%" + value + "%'" +
                                                                                             " AND notes LIKE '%" + user + "%';";
      MYSQL_RES* result = dbc->fetch(sqlQuery);
	    vector<vector<string>> resultVec = DbConnection::getInstance()->toVector(result);
      dbc->freeResult(result);

      return resultVec;
    }


		static string replace( string configFile, string parameter,	string modifier, string value, string user, string lineNumber, bool force )
    {
      configFile = dbc->sanitize(configFile);
      parameter = dbc->sanitize(parameter);
      modifier = dbc->sanitize(modifier);
      value = dbc->sanitize(value);
      user = dbc->sanitize(user);

      vector<vector<string>> result = select( "count(*)", configFile, parameter, modifier, value, user);

      string message = "";
    
	    if ( result.size() > 0 && !force)
	    {
        message += "YOU WILL MODIFY " + color::rize(to_string(result.size()), "Yellow") + " line(s), to do so append " + color::rize("-f", "Red") + "\n";
        return message;
	    }


      string sqlQuery = "REPLACE INTO " + ConfigFileReader::configTable + " (configFile, parameter, modifier, term, notes, lineNumber) VALUES " +
                                                                     " ('" + configFile + "', '" + parameter + "', '" + modifier + "', '" + value + "', '" + user + "', '" + lineNumber + "');";

      if ( CommandParserSqlFunctions::debug ) message += color::rize(sqlQuery, "Red") + "\n";

      try
      {
        dbc->update(sqlQuery);
        message += color::rize("updated successfully", "Green") + "\n";
      }
      catch (...)
      {
        message += "could not update: " + color::rize(sqlQuery, "Red") + "\n";
      }

      message +=cfr->buildCache();

      return message;   
    }

		static string erase(string configFile, string parameter, string modifier, string value, string user, string lineNumber )
    {
      configFile = dbc->sanitize(configFile);
      parameter = dbc->sanitize(parameter);
      modifier = dbc->sanitize(modifier);
      value = dbc->sanitize(value);
      user = dbc->sanitize(user);

      string message = "";

      string sqlQuery = "DELETE FROM " + ConfigFileReader::configTable + " WHERE " + "configFile LIKE '" + configFile + ".cfg'" +
                                                                                " AND parameter LIKE '" + parameter + "'" +
                                                                                " AND modifier LIKE '" + modifier + "'" +
                                                                                " AND term like '%" + value + "%'" +
                                                                                " AND notes like '%" + user + "%';";
                                                                                
      if ( CommandParserSqlFunctions::debug ) message += color::rize(sqlQuery, "Red") + "\n";

      try
      {
        dbc->update(sqlQuery);
        message += "erased successfully\n";
        message += cfr->buildCache();
      }
      catch (...)
      {
        message = "could not delete: " + color::rize(sqlQuery, "Red") + "\n";
      }

      return message;
    }

    //makes a single vector from vector<vector, just takes the column with colNumber x
    vector<string> aggregate(vector<vector<string>> source, int colNumber)
    {
      vector<string> result;
      
      for ( auto row : source )
        result.push_back(row.at(colNumber));
        
      return result;
    }


    static string tab(string value, int width)
    {
      string message = value;
      int spaces = width - value.length();
      for ( int i = 0; i < spaces; i++ )
        message += " ";
        
      return message;
    }


    //TODO: maybe move to other file
    static string colorizeWord(string part, string text)
    {
      string term = "";
      size_t startPos = text.find(part);
      if ( startPos == string::npos )
        return text;
      if ( startPos == 0 )
      {
        term += color::rize(text.substr(0, part.length()), "Red");
        term += color::rize(text.substr(part.length()), "Default");
      }
      else
      {
        term += color::rize(text.substr(0, startPos), "Default");
        term += color::rize(text.substr(startPos, part.length()), "Red");
        term += color::rize(text.substr(startPos + part.length()), "Default");
      }
      
      return term;
    }


    //only one column as vector (column No, usage: distinct("%", "host", "%", "%", 0, 4) should you give all users
    static string distinct(string field, string configFile, string parameter, string modifier, string value, int column, int truncate=0 )  //use turncate = 4 for removing .cfg
    {
      vector<vector<string>> result = select( "distinct("+field+")", configFile, parameter, modifier, value, "%" );  //TODO: order by

      string message = "";
      
      for ( auto row : result )
      {
        string cfg;
        try
        {
          cfg = string(row.at(column));  //TODO: exception if column > column_with
          message +=  " " + color::rize(cfg.substr(0,cfg.length()-truncate), "Cyan");
        }
        catch (...)
        {
          message += "VECTOR OUT OF BOUNDS EXCEPTON, FIX THIS BUG AND HAVE FUN!";
        }
      }
      message += "\n";

      return message.substr(1);
    }


    //list(site, paramter, modifier, value)
    static string list(string configFile, string parameter, string modifier, string value)
    {
      vector<vector<string>> result = select( "configFile, parameter, modifier, term", configFile, parameter, modifier, value, "%");
      cout << " list() " << result.size() << endl;
      string message = "";

      for ( auto row : result )  //TODO: ask shrikey a specific order is required, a) order like in the database OR order alphabetic OR order other, rule, dir OR order alphabetic, rule, dir ...
      {
        vector<string> elements = RegexFunctions::split(' ', row.at(3));
        string term = "";
        for ( auto e : elements )                 //TODO: maybe split on space and then tab / indentation below the term
        {
          if ( RegexFunctions::match(value, e) )  //colorize the part of the word matching red
          {
			      term += " ";
            term += colorizeWord(value, e);
          }
          else
            term += " " + color::rize(e, "Default");
        }
        
        string modifierText = colorizeWord(modifier, row.at(2));
        
        if ( row.at(1).substr(0,1) != "#" )  //Dont show the comment lines
          message += color::rize(tab(row.at(0), 15), "Green") + ": " + color::rize(tab(row.at(1), 15), "Light Green") + " " + color::rize(tab(modifierText, 15), "Cyan") + " = " + color::rize(term, "Default") + "\n";
      }
      
      return message;
    }
      

    static string add(string configFile, string parameter, string modifier, string value, string user, bool force)
    { 
      string message = "";

      int lineNumber = 0;

      vector<vector<string>> result = select( "max(lineNumber)", configFile, parameter, modifier, value, user);

      try
      {
		    if ( result.size() > 0 )
		      lineNumber = stoi(result.at(0).at(0))+1;
		  }
		  catch (...)
		  {
		  }
      
      bool replaced = false;  //TODO: warning if more than 0 lines changed
      
      result = select( "configFile, parameter, modifier, term", configFile, parameter, modifier, value, user);
      for ( auto r : result)
      {
        message += replace(r.at(0), r.at(1), r.at(2), value, user, to_string(lineNumber), force);
        replaced = true;
      }
      
      if ( !replaced )
        message += replace(configFile, parameter, modifier, value, user, to_string(lineNumber), force);
      
      return message;
    }
      
    static string set(string configFile, string parameter, string modifier, string value, string user, bool force, bool checkSyntax)
    {
      return add(configFile, parameter, modifier, value, user, force);
    }
    
    static string del(string configFile, string parameter, string modifier, string value, string user, bool force)
    {
      string message = "";

      vector<vector<string>> result = select( "configFile, parameter, modifier, term", configFile, parameter, modifier, value, "%");

      if ( result.size() > 1 && !force )
        message += color::rize("YOU WILL REMOVE more than 1 LINE", "Yellow") + ", to do so append " + color::rize("-f", "Red") + "\n";
      else
      {
        message += erase(configFile, parameter, modifier, value, user, "%");
        message += color::rize(to_string(result.size()), "Red") + " lines removed" + "\n";
      }
    
      return message;
    }
      
    
    static string append(string configFile, string parameter, string modifier, string value, string user, bool force, bool checkSyntax)
    {
      string message = "";

      vector<vector<string>> result = select( "configFile, parameter, modifier, term, notes, lineNumber", configFile, parameter, modifier, "%", "%");

      string term;

      for ( auto r : result )  //TODO: WARN -f with multiple lines
      {
        term = r.at(3) + " " + value;
        string checkedTerm = "";
        if ( checkSyntax )
          checkedTerm = CheckRuleSyntax::checkSyntax(term);
        if ( checkSyntax && term != checkedTerm )
          return color::rize("Error in your Syntax: ", "Red") + checkedTerm + "\n";
        message += replace(r.at(0), r.at(1), r.at(2), term, user, r.at(5), force);
      }

      return message;
    }
      
    static string remove(string configFile, string parameter, string modifier, string value, string user, bool force, bool checkSyntax)
    {
      string message = "";

      vector<vector<string>> result = select( "configFile, parameter, modifier, term, notes, lineNumber", configFile, parameter, modifier, "%", "%");

      string term;

      for ( auto r : result )  //TODO: WARN -f with multiple lines
      {
        term = r.at(3);
        vector<string> items = RegexFunctions::split(' ', term);
        vector<string> difference = RegexFunctions::split(' ', value);
        for ( auto & i : items )
          for ( auto d : difference )
            if ( d == i )
              i = "";
        string newTerm = CommandParserTools::restToString(items);
        string checkedTerm = "";
        
        if ( checkSyntax )
          checkedTerm = CheckRuleSyntax::checkSyntax(newTerm);
          
        if ( checkSyntax && newTerm != checkedTerm )
          return color::rize("Error in your Syntax: ", "Red") + checkedTerm + "\n";
        message += replace(r.at(0), r.at(1), r.at(2), newTerm, user, r.at(5), force);
      }

      return message;
    }

    static string mod(string configFile, string parameter, string modifier, string value, string user, bool force, bool checkSyntax)
    {
      string message = "";
      //if + add
      //if - del
      return message;
    }

};

#endif
