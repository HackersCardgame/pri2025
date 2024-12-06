
#include <time.h>
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <mysql.h>

#include "../common/dbConnection.cpp"
#include "../common/helperFunctions.cpp"
#include "../common/regexFunctions.cpp"

using namespace std;


#ifndef LOOKUP_TVMAZE
#define LOOKUP_TVMAZE

/*******************************************************************************
* does what it says: looks up the tvmaze database from the local database
* - needs to be built and updated weekly, the tools to do this you will find
*   in /tools/install
* - it has some sort of fault tolerance so you can write
*   - Big Bang Theory or The Big Bang Theory (uses wildchars)
*   - ...
* the main entry point is:
*   - get(title, seasonEpisode, year, country)
*******************************************************************************/
class LookupTvmaze
{


  private:
    inline static string shows_table = "tvmaze";
    inline static string episodes_table = "episodes";
    
    inline static const bool debug = false;
    
    inline static DbConnection* dbc = DbConnection::getInstance();
    
    
    static bool checkTables()
    {
      string sql = "SELECT tconst FROM " + LookupTvmaze::shows_table + " LIMIT 5";
      return false;
    }

        
  public:
  
    inline static int TVMAZE_ID = 0;
    inline static int TVMAZE_TITLE = 1;
    inline static int TVMAZE_TYPE = 2;
    inline static int TVMAZE_LANGUAGE = 3;
    inline static int TVMAZE_GENRE = 4;
    inline static int TVMAZE_STATUS = 5;
    inline static int TVMAZE_PREMIERE = 6;
    inline static int TVMAZE_NETWORK = 7;
    inline static int TVMAZE_NETWORK_WEB = 8;
    inline static int TVMAZE_COUTRY_CODE = 9;
    inline static int TVMAZE_CURRENT_SEASON = 10;
    inline static int TVMAZE_AIRDATE_EPISODE = 11;

    static MYSQL_RES* getTvmaze(string title, string seasonEpisode, int year, string country)
    {
      string sqlQuery;
//          string preparedTitle = DbConnection::prepareSql(title);
    
      if ( country == "UK" )
        country = "GB";

    if ( year != 0 )
      sqlQuery = "SELECT id, title, type, language, genre, status, premiere, network, networkWeb, countryCode from " + LookupTvmaze::shows_table + " where title like '%" + title + "%' AND YEAR(premiere) like " + to_string(year) + ";";

    if ( country != "" )
      sqlQuery = "SELECT id, title, type, language, genre, status, premiere, network, networkWeb, countryCode from " + LookupTvmaze::shows_table + " where title like '%" + title + "%' AND countryCode like '" + country + "';";

    if ( country == "" && year == 0 )
      sqlQuery = "SELECT id, title, type, language, genre, status, premiere, network, networkWeb, countryCode from " + LookupTvmaze::shows_table + " where title like '%" + title + "%';";

    if (LookupTvmaze::debug) cout << sqlQuery << endl;

    MYSQL_RES* result = dbc->fetch(sqlQuery);
    
    return result;
    }



    static ARRAY_WITH_SIZE getEpisode(string tvmazeID, string seasonEpisode)
    {
      string seasonEpisodeUppercase = "";

      for (auto & c: seasonEpisode) seasonEpisodeUppercase += c;
      
      vector<string> parts = RegexFunctions::split('e', seasonEpisodeUppercase);  //TODO: bad, maybe better int in &rls
      
      int season = 0;
      int episode = 0;

      if (LookupTvmaze::debug) cout << "psize " << parts.size() << endl;
      
      if ( parts.size() == 2 )
      {

        try { season = stoi(parts[0].substr(1)); } catch (...) { cout << "could not get season"; }
        
        try { episode = stoi(parts[1]); } catch (...) { cout << "could not get episode"; }
      }
      
      string sqlQuery = "SELECT airdate from " + LookupTvmaze::episodes_table + " where showId = " + tvmazeID + " AND season = " + to_string(season) + " AND episode = " + to_string(episode) +";";

      if (LookupTvmaze::debug) cout << sqlQuery << endl;

      MYSQL_RES* result = dbc->fetch(sqlQuery);
      ARRAY_WITH_SIZE array = dbc->toArray(result);
      mysql_free_result(result);
      return array;
    
    }


    static vector<string> mysqlRes2vector(MYSQL_RES* result, string title)
    {
    
      vector<string> vec;
      ARRAY_WITH_SIZE array = dbc->toArray(result);
      if (array.rows)
      {
        char** result = HelperFunctions::findBest(array.array, array.rows, array.cols, title, 1);  //TODO: if none best?

        
        for ( int i = 0; i < array.cols; i++ )
        {
          if (LookupTvmaze::debug) cout << result[i] << endl;
          vec.push_back(string(result[i]));
          
        }

      }
      dbc->freeArray(array.array, array.rows, array.cols);

      return vec;
    }
    
    static string getLastSeason(string tvmazeID)
    {
      string season = "0";
      string sqlQuery = "SELECT max(season) from " + LookupTvmaze::episodes_table + " where showId = '" + tvmazeID + "';";

      if (LookupTvmaze::debug) cout << sqlQuery << endl;

      MYSQL_RES* result = dbc->fetch(sqlQuery);
      ARRAY_WITH_SIZE array = dbc->toArray(result);
      if (array.cols > 0 && array.rows > 0)
      {
        season = array.array[0][0];
      }
      mysql_free_result(result);
      dbc->freeArray(array.array, array.rows, array.cols);

      return season;
    }
    
    static vector<string> get(string title, string seasonEpisode, int year, string country)
    {
      MYSQL_RES* shows = LookupTvmaze::getTvmaze(title, seasonEpisode, year, country);
      vector<string> show;
      vector<string> episode;
      vector<string> combined;

      if (LookupTvmaze::debug) cout << "numlines " << mysql_num_rows(shows) << endl;
      
      if (mysql_num_rows(shows) > 0)
      {
        show = LookupTvmaze::mysqlRes2vector(shows, title);

        mysql_free_result(shows);

        if ( show[0] == "NONE" )
        {
          cout << "TVMAZe not found: " << title << " " << seasonEpisode << " " << year << " " << country << endl;
        }
        else
        {
          string lastEpisode = getLastSeason(show[0]);
          show.push_back(lastEpisode);
          
          ARRAY_WITH_SIZE array  = LookupTvmaze::getEpisode(show[0], seasonEpisode);
          if (array.cols > 0 && array.rows > 0)
          {
            if (LookupTvmaze::debug) cout << array.array[0][0] << endl;
            show.push_back(array.array[0][0]);
          }
          else
          {
            if (LookupTvmaze::debug) cout << "Episodes not found" << endl;
            show.push_back("NONE");
          }
          dbc->freeArray(array.array, array.rows, array.cols);
        }
      }
      else
      {
        mysql_free_result(shows);
        show = { "NONE", "NONE", "NONE", "NONE", "NONE", "NONE", "NONE", "NONE", "NONE", "NONE", "NONE", "NONE", "NONE" };
      }

      return show;
    }
};

#endif

