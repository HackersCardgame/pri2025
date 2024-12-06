
#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>
#include <unordered_map>

#include <mysql.h>

#include "../common/helperFunctions.cpp"
#include "../common/dbConnection.cpp"

using namespace std;


#ifndef LOOKUP_DUPE
#define LOOKUP_DUPE

/*******************************************************************************
* does what it says: it looks up the dupes in the database TODO: there need to bee more things like dupesPAL dupesNTSC...
*******************************************************************************/
class LookupDupe
{
  private:
  
    inline static const bool debug = false;
    inline static const string dupeTable = "dupes2";
    inline static DbConnection* dbc = DbConnection::getInstance();
    
  public:
  
    static string get(Release &rls)
    {
      if ( rls.overrideDupeCheck )
        return "nodupe";
    
      if ( isReleaseDupe(rls, 365) )
        return "releasedupe";
    
      if ( isGroupDupe(rls, 365) )
        return "groupdupe";
    
      insert(rls);
      return "nodupe";
    }
    
    
    static bool isReleaseDupe(Release &rls, int daysBack)
    {
      string sqlQuery = "SELECT * FROM " + LookupDupe::dupeTable + " where releaseName like '" + rls.releaseName + "';";
      MYSQL_RES* result = dbc->fetch(sqlQuery);
      if ( LookupDupe::debug ) dbc->printResult(result);
      if (mysql_num_rows(result) > 0)
      {
        mysql_free_result(result);
        return true;
      }
      else
      {
        mysql_free_result(result);
        return false;
      }
    }
        
    static bool isGroupDupe(Release &rls, int daysBack)
    {
      string sqlQuery = "SELECT * FROM " + LookupDupe::dupeTable + " where withoutGroup like '" + rls.withoutGroup + "';";   //maybe add timeDelta but telling the db it shall evaluate date slows down
      MYSQL_RES* result = dbc->fetch(sqlQuery);
      if ( LookupDupe::debug ) dbc->printResult(result);
      if (mysql_num_rows(result) > 0)
      {
        mysql_free_result(result);
        return true;
      }
      else
      {
        mysql_free_result(result);
        return false;
      }
    }
    
    static void insert(Release &rls)
    {
       
      string sqlQuery = "INSERT INTO " + LookupDupe::dupeTable 
          + " (releaseName, content, rlsGroup, withoutGroup, year, videosource, resolution, episode, tconst, tvmazeID, timestamp, consoleRegion, override) VALUES " +
          "('" + rls.releaseName + "', '" + rls.content + "', '" + rls.group +  "', '" + rls.withoutGroup + "', '" + rls.releaseYear +
          "', '" + rls.features["videosource"] + "', '" + rls.features["resolution"] + "', '" + rls.episode + 
          "', '" + rls.imdbTconst + "', '" + rls.tvmazeID + "', '" + addDays(0) + "', '" + rls.features["consoleregion"] +
          "', '" + rls.features["dupecheckoverride"] + "');";
       if ( LookupDupe::debug ) cout << "sql: " << sqlQuery << endl;
       if (rls.login) dbc->update(sqlQuery);
    }


    
    static string addDays(int deltaDays)
    {
      time_t now;
      struct tm* currentTime;
      time(&now);
      currentTime = localtime(&now);

      currentTime->tm_mday += deltaDays;
      
      time_t later = mktime(currentTime);
      struct tm* timeLater = localtime(&later);

      const int TIME_STRING_LENGTH = 20;
      char buffer [TIME_STRING_LENGTH];

      strftime(buffer, TIME_STRING_LENGTH, "%Y-%m-%d %H:%M:%S", timeLater);
      return buffer;
    }

    

};

      
      
/*
    
// ===================================================================================================
// Create the dupefile
// ===================================================================================================
  static void insert(rls):
    now = datetime.datetime.now()

    #releaseUnified = DupeChecker.unify(rls.releaseName)
    releaseUnified = rls.releaseName

    query = "INSERT INTO " + self.table \
          + " (releaseName, section, content, rlsGroup, year, videosource, resolution, episode, tconst, tvmazeID, timestamp, consoleRegion, override) VALUES (" \
          + repr(releaseUnified) + ", " + repr(rls.section) + ", " \
          + repr(rls.content) + ", " + repr(rls.group) + ", " \
          + repr(rls.releaseYear) + ", " + repr(rls.features["VIDEOSOURCE"]) \
          + ", " + repr(rls.features["RESOLUTION"]) \
          + ", " + repr(rls.episode) + ", " + repr(rls.imdbTconst) + ", " \
          + repr(rls.tvmazeID) + ", " + repr(str(now)) + ", " \
          + repr(rls.features["CONSOLEREGION"]) + ", " \
          + repr(rls.features["DUPECHECKEROVERRIDE"]) + ");"
                                      
    sql = self.translateNone(query)


    try:
      self.dbConn.modify(sql)
    except:
      logging.error("Could not modify Database " + sql)
      traceback.print_exc(file=sys.stdout)
      
  */    
      
      
#endif
