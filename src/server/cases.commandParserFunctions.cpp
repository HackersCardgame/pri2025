
//description above
    static string select( string what,  //"distinct(configFile)", "distinct(parameter)", "distinct(modifier)", "configFile, parameter, modifier, term, notes, lineNumber"
				  	  					  string configFile, 
			  			  				  string parameter
							  		  	  string modifier
								  	  	  string value, 
	  				  					  string user )
    {
      configFile = dbc->sanitize(configFile);
      parameter = dbc->sanitize(parameter);
      modifier = dbc->sanitize(modifier);
      value = dbc->sanitize(value);
      user = dbc->sanitize(user);

      sqlQuery = "SELECT " + what + " FROM " + ConfigFileReader::configTable + " WHERE " + "configFile like '" + configFile + ".cfg' " +
                                                                                      " and parameter like '" + parameter + "' " +
                                                                                      " and modifier like '" + modifier + "' " +
                                                                                      " and term like '%" + value + "%' " +
                                                                                      " and notes like '%" + user + "%';";
      MYSQL_RES* result = dbc->fetch(sqlQuery);
	    vector<vector<string>> resultVec = DbConnection::getInstance()->toVector(result);
      dbc->freeResult(result);

      return resultVec;
    }





static string replace(
				  	  					string site, 
			  			  				string parameter
							  		  	string modifier
								  	  	string value, 
	  				  					string user,
	  				  					string lineNumber,

-> if "" or '' input sanitation for site, parameter, modifier, value, user, lineNumber

aggregate configFiles -> as vector
aggregate parameters -> as vector
aggregate modifier -> as vector
aggregate term -> as vector

contents: affils, blacklist, rule, group, => site1, site2,...
configFile, term  <- configFile, parameter, modifier

configs: rule = site; host = user => site1,site2,
distinct(configFile) <- configFile, parameter, modifier

modifier: tv tv.720 movies games, rule dir
distinct(modifier) <- configFile, parameter, modifier, %value%


list:
configFile, parameter, modifier, term, notes <- configFile, parameter, modifier, %value%

add: (like set)
1. max(lineNumber) <- configFile
2. insert: configFile, parameter, modifier, term, notes, lineNumber  <- configFile, parameter, modifier, value, notes(user), lineNumber //TODO: Notes update not replace) 

del: 
delete: * <- configFile, parameter, modifier //TODO: undelete function / mark inactive / move to other table, changelog table

set: (like add)
1. existing(lineNumber) <- configFile
replace: configFile, parameter, modifier, term, notes, lineNumber <- configFile, parameter, modifier, value, notes(user), lineNumber

append:
1. configFile, parameter, modifier, term, notes, lineNumber <- configFile, parameter, modifier    (ORDER BY lineNumber)
2. replace: (configFile, parameter, modifier, term, notes, lineNumber) <- configFile, parameter, modifier, newTerm, notes(user), lineNumber

remove:
1. configFile, parameter, modifier, term, notes, lineNumber <- configFile, parameter, modifier
2. replace: configFile, parameter, modifier, term, notes, lineNumber <- configFile, parameter, modifier, newTerm, notes(user), lineNumber

mod:












      string sqlQuery = "SELECT configFile, term from " + ConfigFileReader::configTable + " where configFile like \"" + site + ".cfg\" and parameter like \"" + parameter + "\" and modifier like \"" + modifier + "\" and term like \"%"+value+"%\";";


----------------------------------

      string sqlQuery = "SELECT distinct(modifier) from " + ConfigFileReader::configTable + " where configFile like \"" + site + ".cfg\" and parameter like \"" + parameter + "\" and modifier like \"" + modifier + "\" and term like \"%"+value+"%\";";


----------------------------------

      string sqlQuery = "SELECT distinct(configFile) from " + ConfigFileReader::configTable + " where configFile like \"" + site + ".cfg\" and parameter like \"" + parameter + "\" and modifier like \"" + modifier + "\";";

----------------------------------

      string sqlQuery = "SELECT configFile, parameter, modifier, term, notes from " + ConfigFileReader::configTable + " WHERE configFile LIKE \"" + site + ".cfg\" AND parameter LIKE \"" + parameter + "\" AND modifier LIKE \"" + modifier + "\" AND term like \"%"+value+"%\" ORDER BY lineNumber;";

----------------------------------

      string sqlQuery1 = "SELECT max(lineNumber) from " + ConfigFileReader::configTable + " WHERE configFile LIKE '" + site + ".cfg';";

      string sqlQuery2 = "INSERT INTO " + ConfigFileReader::configTable + " (configFile, parameter, modifier, term, notes, lineNumber) VALUES ('" + site + ".cfg', '" + parameter + "', '" + modifier + "', '" + value + "', '" + user + "', " + ln  + ");";

----------------------------------

      string sqlQuery = "DELETE FROM " + ConfigFileReader::configTable + " WHERE configFile LIKE '" + site + ".cfg' AND parameter LIKE '" + parameter + "' AND modifier LIKE '" + modifier + "' ;";


----------------------------------

      string sqlQuery = "REPLACE INTO " + ConfigFileReader::configTable + " (configFile, parameter, modifier, term, notes, lineNumber) VALUES ('" + site + ".cfg', '" + parameter + "', '" + modifier + "', '" + value + "', '" + user + "', 0);";  //TODO: line 0 ?


----------------------------------

       string sqlQuery = "SELECT configFile, parameter, modifier, term, notes, lineNumber from " + ConfigFileReader::configTable + " WHERE configFile LIKE \"" + site + ".cfg\" AND parameter LIKE \"" + parameter + "\" AND modifier LIKE \"" + modifier + "\" ORDER BY lineNumber;";


          sqlQuery = "REPLACE INTO " + ConfigFileReader::configTable + " (configFile, parameter, modifier, term, notes, lineNumber) VALUES ('" + configFile + "', '" + parameter + "', '" + modifier + "', '" + newTerm + "', '" + user + "', '" + lineNumber + "');";


----------------------------------

remove:

      string sqlQuery = "SELECT configFile, parameter, modifier, term, notes, lineNumber from " + ConfigFileReader::configTable + " WHERE configFile LIKE \"" + site + ".cfg\" AND parameter LIKE \"" + parameter + "\" AND modifier LIKE \"" + modifier + "\" ORDER BY lineNumber;";

          sqlQuery = "REPLACE INTO " + ConfigFileReader::configTable + " (configFile, parameter, modifier, term, notes, lineNumber) VALUES ('" + configFile + "', '" + parameter + "', '" + modifier + "', '" + newTerm + "', '" + user + "', '" + lineNumber + "');";

----------------------------------

