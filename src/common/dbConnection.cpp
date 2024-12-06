
#include <assert.h>

#include <chrono>
#include <iostream>
#include <future>
#include <string>
#include <cstring>

#include <thread>
#include <chrono>

#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <mysql.h>

#include "../common/regexFunctions.cpp"

#include "../lib/colorize/color.hpp"

using namespace std;

#ifndef DB_CONNECTION
#define DB_CONNECTION

struct ARRAY_WITH_SIZE {  //TODO: not here
  int rows;
  int cols;
  char*** array;
};


/*******************************************************************************
* does all the database related stuff, this is the only module that directly
* communicates with the database, so if you would like to use another type
* of database, then you only have to change things here
*
* if you change things remember the locks / lock_guards and if you changed
* thigns remember to run simultaneously multithreads to check if you have
* deadlocks or chrashes when simultaneously accessing data
*******************************************************************************/

// singletonMeyers.....cpp
class DbConnection{
  public:
    inline static const bool debug = false;

    static DbConnection* getInstance(){
      DbConnection* sin= instance.load();
      if ( !sin ){
        std::lock_guard<std::mutex> myLock(myMutex);
        sin= instance.load();
        if( !sin ){
          sin= new DbConnection();
          instance.store(sin);
        }
      }
      // volatile int dummy{};
      return sin;
    }

    string sanitize(string value)
    {
      //prevent sql-injection  CASE:  \'; DROP TABLE users; - ?? TODO
      value = RegexFunctions::replace("'", "", value);
      value = RegexFunctions::replace("\"", "", value);

      return value;
    }

    void reset()
    {
      if (DbConnection::debug) cout << "reset()" << endl;
      std::lock_guard<std::mutex> myLock(myMutex);
      if (DbConnection::debug) cout << color::rize("reset() mutex", "Yellow") << endl;
      mysql_close(this->con);
      if (DbConnection::debug) cout << color::rize("DB Connection closed", "Yellow") << endl;
      this->con = mysql_init(NULL);
      if (DbConnection::debug) cout << color::rize("mysql_init()", "Yellow") << endl;

      if (mysql_real_connect(this->con, this->dbHost, this->dbUser, this->dbPassword, this->dbDatabase, 0, NULL, 0) == NULL)
      {
          finish_with_error(con);
          //TODO: sleep 1, then retry to reconnect, may 100 tries
      }
      else
        if (DbConnection::debug) cout << color::rize("DatabaseConnection successfully initialized", "Green") << endl;
    }


    int testLocks(int number)
    {
      if (DbConnection::debug) cout << "testLocks()" << endl;
      string a = string("SELECT ");                                             // WITH "SELECT 1" we get 1 from the Database, so the database connection will be used so we can test without table
      string b = to_string(number);
      string sql = a + b;

      MYSQL_RES* res = this->fetch(sql);
      if (mysql_num_rows(res)>0)
      {
        MYSQL_ROW row = this->getFirstRow(res);
        unsigned long *lengths = mysql_fetch_lengths(res);
        int len = lengths[0]+1;
        char cstr[len];
        sprintf(cstr, "%s", row[0] ? row[0] : "NULL");   //TODO drei mal, was wenn len=nulll udn der cstr mit 1 initialisiert wurde aber da vier buchstaben (NULL) eingetragen wird?
        //string text(cstr);
        mysql_free_result(res);
        return std::stoi(cstr);
      }

      return 0;
    }
    

    MYSQL_RES* fetchAll(string sqlQuery)
    {
      if (DbConnection::debug) cout << "fetchAll()" << endl;
      std::lock_guard<std::mutex> myLock(myMutex);

      if (mysql_query(con, sqlQuery.c_str()))
      {
        if (DbConnection::debug) cout << "ERROR in mysql_query()" << endl;
        finish_with_error(con);
        throw string("could not execute query: " + sqlQuery);
      }

      MYSQL_RES *result = mysql_store_result(con);

      if (result == NULL)
      {
        if (DbConnection::debug) cout << "result == NULL" << endl;
        finish_with_error(con);
        throw string("could not execute query: " + sqlQuery);
      }
        
      //if (mysql_num_rows(result)<1)   this does not work because empty results would be repeated eternally
        //throw string("\"" + sqlQuery + "\"" + " (ResultSet empty)");
        
      return result;
    }

    int update(string sqlQuery)
    {
      if (DbConnection::debug) cout << "update()" << endl;
      std::lock_guard<std::mutex> myLock(myMutex);

      if (mysql_query(con, sqlQuery.c_str()))
      {
        if (DbConnection::debug) cout << "ERROR in mysql_query()" << endl;
        finish_with_error(con);
        throw string("could not execute query: " + sqlQuery);
      }
      return 0;
    }


    MYSQL_RES* fetch(string sqlQuery)
    {
      if (DbConnection::debug) cout << "fetch()" << " -> SQL: " << sqlQuery << endl;
      MYSQL_RES* result = nullptr;
      bool gotResult = false;
      int counter = 0;
      while (!gotResult)
      {
        try
        {
          result = this->fetchAll(sqlQuery);
          gotResult = true;
        }
        catch(string e)
        {
          counter++;
          cout << "EE: " << e << endl;
          if (DbConnection::debug) cout << "RESETTING NoW" << endl;
          this_thread::sleep_for(std::chrono::milliseconds(500));
          this->reset();
          if ( counter > 10 ) break;
        }
      }
      return result;
    }


    MYSQL_ROW getFirstRow(MYSQL_RES* result)    //TODO: maybe not needed
    {
      //int num_fields = mysql_num_fields(result);                                //the number of columns
      MYSQL_ROW row;
      row = mysql_fetch_row(result);
      return row;
    }  


    void printResult(MYSQL_RES *result)
    {
      mysql_data_seek(result, 0);  //zurückspulen falls nicht zurück gespult ist, bzw. den pointer auf 0 setzen

      int num_fields = mysql_num_fields(result);
      int numRows = mysql_num_rows(result);

      if (DbConnection::debug) cout << "ROWS: " << numRows << endl;

      MYSQL_ROW row;

      while ((row = mysql_fetch_row(result))) 
      { 
          for(int i = 0; i < num_fields; i++) 
          { 
              printf("%s ", row[i] ? row[i] : "NULL"); 
          } 
          printf("\n"); 
      }
    }


//TODO: FREE / DESTROY THE ARRAY https://stackoverflow.com/questions/8617683/return-a-2d-array-from-a-function
    ARRAY_WITH_SIZE toArray(MYSQL_RES *result)
    {
      int num_cols = mysql_num_fields(result);
      int num_rows = mysql_num_rows(result);
      int current_row = 0;

      char*** field_array = 0;
      field_array = new char**[num_rows];

      MYSQL_ROW row;
      unsigned long *lengths;

      mysql_data_seek(result, 0);

      while ((row = mysql_fetch_row(result)))
      {
        field_array[current_row] = new char*[num_cols];
        lengths = mysql_fetch_lengths(result);
        for(int current_col = 0; current_col < num_cols; current_col++) 
        { 
          int len = lengths[current_col]+1;
          field_array[current_row][current_col] = new char[len];
          sprintf(field_array[current_row][current_col], "%s", row[current_col] ? row[current_col] : "NULL");
        }
        current_row++;
      }
      
      return ARRAY_WITH_SIZE { num_rows, num_cols, field_array };
    }


//XXX: much slower but much more elegant, dont use it for things that must be fast
    vector<vector<string>> toVector(MYSQL_RES *result)
    {
      int num_cols = mysql_num_fields(result);

      vector<vector<string>> vec;

      MYSQL_ROW row;
      unsigned long *lengths;

      mysql_data_seek(result, 0);

      while ((row = mysql_fetch_row(result)))
      {
        vector<string> rowVec;
        lengths = mysql_fetch_lengths(result);
        for(int current_col = 0; current_col < num_cols; current_col++) 
        { 
          int len = lengths[current_col]+1;
          char* field = new char[len];
          sprintf(field, "%s", row[current_col] ? row[current_col] : "NULL");
          string f = string(field);
          rowVec.push_back(f);
        }
        vec.push_back(rowVec);
      }
      
      return vec;
    }


    char** to1dArray(MYSQL_RES *result)
    {
      int num_rows = mysql_num_rows(result);
      int num_cols = mysql_num_fields(result);

      if (num_rows < 1)
      {
        char** emptyRow = new char*[num_cols];
        
        for (int i = 0; i < num_cols; i++)
        {
          emptyRow[i] = (char*)"NONE";
        }
        return emptyRow;
      }

      char** array = new char*[num_cols];

      MYSQL_ROW row;
      unsigned long *lengths;

      mysql_data_seek(result, 0);

      row = mysql_fetch_row(result);
      {
        lengths = mysql_fetch_lengths(result);
        for(int current_col = 0; current_col < num_cols; current_col++) 
        { 
          int len = lengths[current_col]+1;
          array[current_col] = new char[len];
          sprintf(array[current_col], "%s", row[current_col] ? row[current_col] : "NULL");
        }
      }
      return array;
    }



// XXX: dont use vector<string> because to slow, about 5x slower than pointer array

    void freeArray(char*** field_array, int num_rows, int num_cols)
    {
      //int current_row = 0;
      for (int i = 0; i < num_rows; i++)
      {
        for (int j = 0; j < num_cols; j++)
        {
          //free(field_array[i][j]);
          delete[] field_array[i][j];
        }
        //free(field_array[i]);
        delete[] field_array[i];
      }
      //free(field_array);
      delete[] field_array;
    }

    void freeResult(MYSQL_RES* res)
    {    
      mysql_free_result(res);
    }
    

    static string prepareSql(string a)
    {
      int len = a.length();
      string b = "";
      for (int i = 0; i < len*2+1; ++i)
        if(i%2==0)
          b += '%';
        else
          b += a[(i-1)/2];
      return b;
    }

private:
    const char* dbHost = "localhost";                                           //TODO: in config file or as parameter
//    const char* dbHost = "127.0.0.1";       //TODO: just for the CI/CD Test, localhost would be faster because it does not use the tcp-ip stack, maybe add /etc/hosts
    const char* dbUser = "pci";   //TODO: was "pci", maybe debian and centos does this different, then would have to change per system
    const char* dbPassword = "123456";
    const char* dbDatabase = "pci";

    bool connected = false;


    MYSQL *con = mysql_init(NULL);

    DbConnection()
    { 
      if (DbConnection::debug) cout << color::rize("Constructor dbConnection", "Yellow") << endl;
      int connectionAttempts = 0;

      while (!this->connected && connectionAttempts < 10)
      {
        connectionAttempts++;
        try
        {
          if (DbConnection::debug) cout << color::rize("trying to connect do mysql/mariaDB", "Yellow") << endl;

          if (con == NULL)
          {
              fprintf(stderr, "%s\n", mysql_error(con));
              throw runtime_error("sql con == NULL");
              //exit(1);
          }

          if (DbConnection::debug) cout << color::rize("mysqsl/mariaDB Server present", "Yellow") << endl;

          if (mysql_real_connect(this->con, this->dbHost, this->dbUser, this->dbPassword, this->dbDatabase, 0, NULL, 0) == NULL) 
          {
              finish_with_error(con);
              throw runtime_error("sql_real_connect() failed");
          }

          if (DbConnection::debug) cout << color::rize("mysql user/password accepted", "Yellow") << endl;

          this->connected = true;
        }
        catch(...)
        {
          if (DbConnection::debug) cout << color::rize("DB Initializing failed, retrying", "Yellow") << endl;
          this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    
      }
      if (DbConnection::debug) cout << color::rize("DatabaseConnection successfully initialized", "Green") << endl;
    }

    //Destructor        
    ~DbConnection()
    { 
      mysql_close(this->con);
      if (DbConnection::debug) cout << color::rize("Destructor dbConnection", "Yellow") << endl;
    }  
  
    //Mysql things
    void finish_with_error(MYSQL *con)
    {
      fprintf(stderr, "%s\n", mysql_error(con));
    }
    
//TODO: implement this:
    //use this for all inputs to prevent SQL-Injection    
    std::string escapeSQL(const char* dataIn)   //könnte man auch string nehmen weil es keine so massenhaft vielen sql queries gibt
    {
       if (dataIn)
       {
          std::size_t dataInLen = strlen(dataIn);
          std::string to(((dataInLen * 2) + 1), '\0');
          mysql_real_escape_string(this->con, &to[0], dataIn, dataInLen);
          return (to);
       }
       else
       {
          return ("");
       }
    }

    //belongs to destructor, delete the object
    DbConnection(const DbConnection&)= delete;
    DbConnection& operator=(const DbConnection&)= delete;

    static std::atomic<DbConnection*> instance;
    static std::mutex myMutex;
};


std::atomic<DbConnection*> DbConnection::instance;
std::mutex DbConnection::myMutex;

#endif
