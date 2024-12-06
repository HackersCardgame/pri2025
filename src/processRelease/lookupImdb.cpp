
#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "../lib/Jaro-Winkler/cpp/jwdistance.h"
#include "../common/dbConnection.cpp"
#include <mysql.h>

#include "../common/helperFunctions.cpp"


using namespace std;


#ifndef LOOKUP_IMDB
#define LOOKUP_IMDB

/*******************************************************************************
* does what it says: looks up the imdb movie database from the local databas
* - needs to be built and updated weekly, the tools to do this you will find
*   in /tools/install
* - it has some sort of fault tolerance so you can write
*   - Matrix or The Matrix (uses wildchars)
*   - Starwars or Krieg der Sterne (will use movies akas table)
*   - ...
* the main entry point is:
*   - get(string title, int year)
*******************************************************************************/
class LookupImdb
{


  private:
    inline static string akas_small = "akas_small";
    inline static string basics_small = "basics_small";
    inline static string ratings_small = "ratings_small";
    
    inline static string akas = "akas";
    inline static string basics = "basics";
    inline static string ratings = "ratings";

    inline static string tmdb = "tmdb";
    inline static string mojo = "mojo";
    
    inline static const bool debug = false;
    
    inline static DbConnection* dbc = DbConnection::getInstance();
    
    
    //TODO: maybe HD tables for short movies -> shrikey soll das sagen


    static bool checkTables()
    {
      string sql = "SELECT tconst FROM " + LookupImdb::basics + " LIMIT 5";
      return false;
    }
    


//XXX und ne index tabelle zu machen bringt auch nochmals 100-1000x mehr speed...
        
  public:
  
    inline static int imdbTconst = 0;
    inline static int titleType = 1;
    inline static int imdbPrimaryTitle = 2;
    inline static int imdbOriginalTitle = 3;
    inline static int imdbYear = 4;
    inline static int imdbGenre = 5;
    inline static int imdbRating = 6;
    inline static int imdbVotes = 7;
    inline static int tmdbLanguage = 8;
    inline static int mojoUsScreens = 9;
    inline static int mojoUkScreens = 10;

    static MYSQL_RES* getBasics(string title, int year, string basic)
    {
      string preparedTitle = DbConnection::prepareSql(title);
      
      string sqlQuery = "";

      if ( year != 0 )
        sqlQuery = "SELECT tconst, titleType, primaryTitle, originalTitle, startYear, genres FROM " + basic +
                          //  " USE INDEX (bs_pt)" + 
                            " WHERE primaryTitle like '" + preparedTitle  + "' AND startYear >= " +
                            to_string(year - 1) + " AND startYear <= " + to_string(year + 1) + ";";
      else
        sqlQuery = "SELECT tconst, titleType, primaryTitle, originalTitle, startYear, genres FROM " + basic +
                          //  " USE INDEX (bs_pt)" + 
                            " WHERE primaryTitle like '" + preparedTitle  + "';";
      
      if ( LookupImdb::debug ) cout << sqlQuery << endl;
      MYSQL_RES* result = dbc->fetch(sqlQuery);
      if ( LookupImdb::debug ) dbc->printResult(result);
      return result;
    }

/*
    static MYSQL_RES* getAkas(string title, int year)
    {
      string preparedTitle = DbConnection::prepareSql(title);
      std::string sqlQuery ("SELECT titleId, title from " + LookupImdb::akas + " USE INDEX (as_ot)" + " WHERE title like '" +  preparedTitle  + "'" + ";");
      MYSQL_RES* result = dbc->fetch(sqlQuery);
      if (LookupImdb::debug) dbc->printResult(result);
      return result;
    }*/

    static MYSQL_RES* getBasicsAkas(string title, int year, string basic, string akas)
    {
      string preparedTitle = DbConnection::prepareSql(title);

      string sqlQuery = "";

      if ( year != 0 )
        sqlQuery = "SELECT titleId, titleType, title, originalTitle, startYear, genres FROM " +
                             akas  + /* " USE INDEX (as_ot)" +*/ " LEFT JOIN " + basic+" ON " + akas + ".titleId = " + 
                             basic + ".tconst " + 
                             " WHERE title LIKE '" + preparedTitle + "' AND " + basic + 
                             ".startYear >= " + to_string(year - 1) + " AND "+basic+".startYear <= " + to_string(year + 1) +  ";";
      else
        sqlQuery = "SELECT titleId, titleType, title, originalTitle, startYear, genres FROM " +
                             akas  + /* " USE INDEX (as_ot)" +*/ " LEFT JOIN " + basic+" ON " + akas + ".titleId = " + 
                             basic + ".tconst " + 
                             " WHERE title LIKE '" + preparedTitle + "';";


      if ( LookupImdb::debug ) cout << sqlQuery << endl;
      MYSQL_RES* result = dbc->fetch(sqlQuery);
      return result;
    }
    
    static MYSQL_RES* getRating(string tconst)
    {
      string sqlQuery ("SELECT * from " + LookupImdb::ratings + " where tconst = '"+ tconst + "';");
      MYSQL_RES* result = dbc->fetch(sqlQuery);
      return result;
    }
    

    static MYSQL_RES* getTmdb(string tconst)
    {
      string sqlQuery ("SELECT originalLanguage from " + LookupImdb::tmdb + " where tconst = '"+ tconst + "';");
      MYSQL_RES* result = dbc->fetch(sqlQuery);
      return result;
    }
    

    static MYSQL_RES* getMojo(string tconst)
    {
      string sqlQuery ("SELECT usScreens, ukScreens from " + LookupImdb::mojo + " where tconst = '"+ tconst + "';");
      MYSQL_RES* result = dbc->fetch(sqlQuery);
      return result;
    }
    

    static vector<string> mysqlRes2vector(MYSQL_RES* result, string title)
    {
      if ( LookupImdb::debug ) cout << "mysqlRes2vector()" << endl;
      vector<string> vec;
      ARRAY_WITH_SIZE array = dbc->toArray(result);
      if (array.rows)
      {
        char** result = HelperFunctions::findBest(array.array, array.rows, array.cols, title, 2);
        if ( string(result[0]) == "NONE" )
        {
          vec = { "NONE", "NONE", "NONE", "NONE", "NONE",  "NONE",  "NONE", "NONE", "NONE" , "NONE", "NONE" };
        }
        else
        {
          MYSQL_RES* mysql_rating = LookupImdb::getRating(result[0]);
          char** rating = dbc->to1dArray(mysql_rating);
          
          if (LookupImdb::debug) cout << "RETURN " << color::rize( result[0], "Cyan" ) << ": " + color::rize( result[1], "Yellow") << " =>" << color::rize( rating[1], "Green" ) << "(" + color::rize( rating[2], "Blue") +")" << endl;
          
          MYSQL_RES* mysql_tmdb = LookupImdb::getTmdb(result[0]);
          char** tmdb = dbc->to1dArray(mysql_tmdb);
          
          MYSQL_RES* mysql_mojo = LookupImdb::getMojo(result[0]);
          char** mojo = dbc->to1dArray(mysql_mojo);

          //Important: The order matters!!!
          vec.push_back(string(result[imdbTconst]));
          vec.push_back(string(result[titleType]));
          vec.push_back(string(result[imdbPrimaryTitle]));
          vec.push_back(string(result[imdbOriginalTitle]));
          vec.push_back(string(result[imdbYear]));
          vec.push_back(string(result[imdbGenre]));
          vec.push_back(string(rating[1]));
          vec.push_back(string(rating[2]));
          vec.push_back(string(tmdb[0]));
          vec.push_back(string(mojo[0]));
          vec.push_back(string(mojo[1]));
        }
      }
      else
      {
        vec = { "NONE", "NONE", "NONE", "NONE", "NONE",  "NONE",  "NONE", "NONE", "NONE" , "NONE", "NONE" };
      }
      dbc->freeArray(array.array, array.rows, array.cols);
      return vec;
    }



    static vector<string> get(string title, int year)
    {
      if ( LookupImdb::debug ) cout << "getBasics, small " << title << endl;
      MYSQL_RES* result = LookupImdb::getBasics(title, year, basics_small);

      vector<string> vec = LookupImdb::mysqlRes2vector(result, title);

      if ( vec[0] == "NONE" )
      {
        if ( LookupImdb::debug ) cout << "getAkas, small " << title << endl;
        mysql_free_result(result);
        result = LookupImdb::getBasicsAkas(title, year, basics_small, akas_small);

        vec = LookupImdb::mysqlRes2vector(result, title);

        if ( LookupImdb::debug ) dbc->printResult(result);
        
        if ( vec[0] == "NONE" )
        {
          if ( LookupImdb::debug ) cout << "getBasics, large " << title << endl;
          mysql_free_result(result);
          result = LookupImdb::getBasics(title, year, basics);

          vec = LookupImdb::mysqlRes2vector(result, title);

          if ( LookupImdb::debug ) dbc->printResult(result);
          if ( vec[0] == "NONE" )
          {
            if ( LookupImdb::debug ) cout << "getAkas, large " << title << endl;
            mysql_free_result(result);
            result = LookupImdb::getBasicsAkas(title, year, basics, akas);

            vec = LookupImdb::mysqlRes2vector(result, title);

          }
        }
      }// TODO: similarity anpassen 
      mysql_free_result(result);
      return vec;
    }
};

#endif

