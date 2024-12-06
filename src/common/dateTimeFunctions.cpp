#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>

#include "../common/regexFunctions.cpp"

using namespace std;

#ifndef DATE_TIME_FUNCTIONS
#define DATE_TIME_FUNCTIONS

/*******************************************************************************
* since c++ does not have build-in functions to handle date and time we need
* to have our own functions to eg. add days to date and such things
*******************************************************************************/
class DateTimeFunctions
{

  inline static const bool debug = false;


  public:
  // Adjust date by a number of days +/-
  static void datePlusDays( struct tm* date, int days )
  {
      const time_t ONE_DAY = 24 * 60 * 60 ;

      // Seconds since start of epoch
      time_t date_seconds = mktime( date ) + (days * ONE_DAY) ;

      // Update caller's date
      // Use localtime because mktime converts to UTC so may change date
      *date = *localtime( &date_seconds ) ; ;
  }

  static tm dateFromString(string dateString)
  {
    vector<string> parts = RegexFunctions::split('-', dateString);
    
    struct tm date = { 0, 0, 0 } ;  // nominal time midday (arbitrary).
    
    if (parts.size() == 3)
    {
      int year;
      try { year = stoi(parts[0]); } catch (...) { year = 1900; }
      int month;
      try { month = stoi(parts[1]); } catch (...) { month = 1; }
      int day;
      try { day = stoi(parts[2]); } catch (...) { day = 0; }
    
      // Set up the date structure
      date.tm_year = year - 1900;
      date.tm_mon = month - 1 ;  // note: zero indexed
      date.tm_mday = day;       // note: not zero indexed
    }
    
    if (parts.size() == 2)
    {
      int year;
      try { year = stoi(parts[0]); } catch (...) { year = 1900; }
      int month;
      try { month = stoi(parts[1]); } catch (...) { month = 1; }
      int day = 1;
    
      // Set up the date structure
      date.tm_year = year - 1900 ;
      date.tm_mon = month - 1 ;  // note: zero indexed
      date.tm_mday = day;       // note: not zero indexed
    }
    
    if (parts.size() == 1)
    {
      if ( parts[0] == "now" )
      {
        time_t t = std::time(NULL);
        struct tm now = *localtime(&t);
        return now;
      }
      else
      {
        int year = 1900;
        try { year = stoi(parts[0]); } catch (...)
        {
          cout << " Date function falied, taking 1900" << endl;                 //TODO: what default date
        }
        int month = 0;
        int day = 1;
    
        // Set up the date structure
        date.tm_year = year - 1900 ;
        date.tm_mon = month - 1 ;  // note: zero indexed
        date.tm_mday = day;       // note: not zero indexed
      }
    }
    
    /*
    {
      if ( DateTimeFunctions::debug) cout << asctime( &date ) << std::endl;
      return date;
    }*/
    
    if ( DateTimeFunctions::debug) cout << asctime( &date ) << std::endl;
    return date;
  }
  
  static string toString(tm date)
  {
    return asctime(&date);
  }
  
  static string toDateString(tm date)
  {
    int month = date.tm_mon+1;
    char monthStr[3];
    snprintf (monthStr, sizeof(monthStr), "%'.2d", month);
    char dayStr[3];
    snprintf (dayStr, sizeof(dayStr), "%'.2d", date.tm_mday);
    string result = to_string(date.tm_year+1900) + "-" + monthStr + "-" + dayStr;
    return result;
  }


  static string getTimeStamp()
  {
    string timeStamp = "";
    
    struct tm *stampNow;
    time_t tNow;
    time(&tNow);
    stampNow = localtime(&tNow);
    
    timeStamp += to_string(stampNow->tm_year + 1900) + "-";
    
    char mon[13];
    snprintf (mon, 13, "%.2d", stampNow->tm_mon + 1);
    timeStamp += string(mon) + '-';
    
    
    char day[13];
    snprintf (day, 13, "%.2d", stampNow->tm_mday);
    timeStamp += string(day) + ' ';
    
    char hour[13];
    snprintf (hour, 13, "%.2d", stampNow->tm_hour);
    timeStamp += string(hour) + ':';
    
    char min[13];
    snprintf (min, 13, "%.2d", stampNow->tm_min);
    timeStamp += string(min) + ':';
    
    char sec[13];
    snprintf (sec, 13, "%.2d", stampNow->tm_sec);
    timeStamp += string(sec);
  
    return timeStamp;
  }



};

#endif














