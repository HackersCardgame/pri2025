
#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>

#include <mysql.h>

#include "../common/dbConnection.cpp"

using namespace std;


#ifndef HISTORY
#define HISTORY

/*******************************************************************************
* does what it says: writes history data into the databasse per user
*******************************************************************************/
class History
{
  private:
  
    inline static const bool debug = false;
    inline static const string historyTable = "history";
    inline static DbConnection* dbc = DbConnection::getInstance();
    
  public:
  
    static void writeHistory(string user, string action, string command, string release)
    {
      string sqlQuery = "INSERT IGNORE INTO " + History::historyTable 
          + " (configFile, action, command, releaseName, time) VALUES " +
          "('" + user + "', '" + action + "', '" + command + "', '" + release + "', NOW(3));";
       if ( History::debug ) cout << "sql: " << sqlQuery << endl;
       dbc->update(sqlQuery);
    }

    static string lastRelease(string user)
    {
      string sqlQuery = "SELECT releaseName FROM " + History::historyTable 
          + " WHERE action = 'login' AND configFile like '" + user + "%' ORDER BY time DESC LIMIT 1;";
       if ( History::debug ) cout << "sql: " << sqlQuery << endl;

      MYSQL_RES* result = dbc->fetch(sqlQuery);

      ARRAY_WITH_SIZE array = dbc->toArray(result);
      
      if (array.rows)
      {
        for ( int i = 0; i < array.rows; i++)
        {
          if ( History::debug ) cout << array.array[i][0] << endl;
          return array.array[i][0];
        } 
      }
      return "";
    }

    static string history(string user, int count)
    {
    
      string message = "";
      string sqlQuery = "SELECT * FROM " + History::historyTable 
          + " WHERE configFile like '" + user + "%' ORDER BY time DESC LIMIT " + to_string(count) + ";";
      if ( History::debug ) cout << "sql: " << sqlQuery << endl;

      MYSQL_RES* result = dbc->fetch(sqlQuery);

      ARRAY_WITH_SIZE array = dbc->toArray(result);
      
      cout << array.rows << " " << array.cols << endl;
      
      if (array.rows)
      {
        for ( int i = array.rows-1; i >= 0 ; i--)
        {
          cout << i << endl;
          for ( int j = 0; j < array.cols; j++ )
          {
            if ( History::debug ) cout << array.array[i][j] << " ";
            message += string(array.array[i][j]) + " ";
          }
          message += "\n";
        } 
      }
      return message;
    }



};

      
#endif
