
#include <chrono>
#include <iostream>
#include <future>
#include <string>
#include <vector>

#include <thread>
#include <chrono>

#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <mysql.h>

#include "../lib/colorize/color.hpp"

#include "dbConnection.cpp"

using namespace std;

constexpr auto tenMill= 100000;                                               //call constructor 1000000 times to test

//std::atomic<DbConnection*> DbConnection::instance;
//std::mutex DbConnection::myMutex;


std::chrono::duration<double> getTime(int num, string test){

  auto begin= std::chrono::system_clock::now();
  for ( int i = 0; i < tenMill; ++i)
  {
    if ( i % 50000 == 0 )
    {
      cout << "Testing Reset: RESET SQL CONNECTION intentionally after 50000 queries" << endl;
      DbConnection::getInstance()->reset();
    }
    if (i==DbConnection::getInstance()->testLocks(i))
    {
      if ( i%99999 == 0 ) cout << i << " OK thread: " << num << endl;
    }
    else 
      cout << i << "ERROR thread: " << num << endl;
  }
  return std::chrono::system_clock::now() - begin;
}


//TEST SINGLETON WITH MULTITHREADING and SQL CONNECTION (SELECT 1, SELECT 2....)
void unitTests()
{
  try
  {
    cout << DbConnection::getInstance()->testLocks(1337);
  }
  catch (const std::exception&)
  {
    cout << "Database Query failed" << endl;
  }
  
  string sqlQuery[3] = { "SELECT configFile FROM config2 LIMIT 5",
                         "SELECT * FROM basics LIMIT 5",
                         "SELECT configFile FROM config2 LIMIT 5"  };
  for (int h = 0; h < 5; h++)
  {
    for (int i = 0; i < 3; i++)
    {
      try
      {
        cout << "QUERY: " << sqlQuery[i] << endl;
        
        MYSQL_RES* res = DbConnection::getInstance()->fetch(sqlQuery[i]);  

        if (mysql_num_rows(res)>0)
        {
          MYSQL_ROW row = DbConnection::getInstance()->getFirstRow(res);
          cout << row[0] << endl;
        }
        mysql_free_result(res);

      }
      catch (string e)
      {
        std::cout << e << "\n";
        std::cout << color::rize("(WW) Warning: " + e, "Black", "Yellow", "Bold") << endl;
      }
    }
  if ( h == 19 ) cout << "restarting loop" << endl<< endl<< endl;
  this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  
  cout << "Testing vector function" << endl;
  MYSQL_RES* res2 = DbConnection::getInstance()->fetch("SELECT configFile, parameter, modifier FROM config2 LIMIT 5;");  
  vector<vector<string>> all = DbConnection::getInstance()->toVector(res2);

  for ( auto l : all )
    for ( string c : l )
      cout << c << endl;

  std::cout << color::rize("Starting db locks thread test with 8 threads with 10'000 SQL Queries each", "White", "Cyan", "Bold") << endl;
  this_thread::sleep_for(std::chrono::milliseconds(1000));

  auto fut1= std::async(std::launch::async,getTime, 1, "TEST");  //launch = fire and forget
  auto fut2= std::async(std::launch::async,getTime, 2, "TEST");
  auto fut3= std::async(std::launch::async,getTime, 3, "TEST");
  auto fut4= std::async(std::launch::async,getTime, 4, "TEST");
  auto fut5= std::async(std::launch::async,getTime, 5, "TEST");
  auto fut6= std::async(std::launch::async,getTime, 6, "TEST");
  auto fut7= std::async(std::launch::async,getTime, 7, "TEST");
  auto fut8= std::async(std::launch::async,getTime, 8, "TEST");
  
  auto total= fut1.get() + fut2.get() + fut3.get() + fut4.get() + fut5.get() + fut6.get() + fut7.get() + fut8.get();

  std::cout << total.count() << std::endl;
}
  
/*
  if (mysql_query(con, "DROP TABLE IF EXISTS Cars")) {
      finish_with_error(con);
  }
*/

int main(int argc, char **argv)
{
  try
  {
    unitTests();
  }
  catch (const std::exception&)
  {
    cout << "COULD NOT RUN unitTests for dbConnection.cpp" << endl;
    return 1;
  }
  return 0;
}




