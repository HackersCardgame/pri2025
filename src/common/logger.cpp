
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

#include <vector>

#include "../common/regexFunctions.cpp"
#include "../common/dateTimeFunctions.cpp"

#include "../lib/colorize/color.hpp"



using namespace std;

#ifdef __clang__
#define debugInfo __FILE__, __LINE__, "compiled-with-clang-6-no-data"
#endif
#ifndef __clang__
#define debugInfo __FILE__, __LINE__, __builtin_FUNCTION()
#endif

#ifndef LOGGER
#define LOGGER

/*******************************************************************************
* does what it says, class for logging, just call eg
* Logger log = Logger::getInstance();
* log.info("MESSAGE", "FILE THAT CALLED IT", "Linenumber", "calling function")
*
* or
*
* Logger.getInstance().info("MESSAGE", "FILE THAT CALLED IT", "Linenumber", "calling function")
* that does the same but calls "getInstance". if you only want to log ince
* in a complete file this is ok, but if you want to log multiple things from
* one file, then it is not good to call it multiple times
*******************************************************************************/
class Logger{
  public:
  
    Logger* logger;
  
    inline static const bool debug = false;
    string logContent = "";

    static Logger* getInstance(){
      Logger* sin = instance.load();
      if ( !sin ){
        std::lock_guard<std::mutex> myLock(loggerMutex);
        sin = instance.load();
        if( !sin ){
          sin = new Logger();
          instance.store(sin);
        }
      }
      return sin;
    }

  string fileOnly(string fileName)
  {
    const char* cFileName = fileName.c_str();
    int len = strlen(cFileName);
    int pos = 0;
    for (int i = 0; i < len; ++i)
      if ( cFileName[i] == '/' )
        pos = i+1;
        
    return fileName.substr(pos, len);
  }

  void info(string message, string file, int line, string function)
  {
    string fileName = fileOnly(file);
    std::lock_guard<std::mutex> myLock(loggerMutex);
    this->logContent += "["+DateTimeFunctions::getTimeStamp() +"] "+ "INFO: " + fileName + ":" + to_string(line) + ":" + function + "() " + message + "\n";
  }
  
  void warning(string message, string file, int line, string function)
  {
    string fileName = fileOnly(file);
    std::lock_guard<std::mutex> myLock(loggerMutex);
    this->logContent += "["+DateTimeFunctions::getTimeStamp() +"] "+ "WARNING: " + fileName + ":" + to_string(line) + ":" + function + "() " + message + "\n";
  }

  void error(string message, string file, int line, string function)
  {
    string fileName = fileOnly(file);
    std::lock_guard<std::mutex> myLock(loggerMutex);
    this->logContent += "["+DateTimeFunctions::getTimeStamp() +"] "+ "ERROR: " + fileName + ":" + to_string(line) + ":" + function + "() " + message + "\n";
  }

  string getLogContent(bool allErrors)
  {
    vector<string> lines = RegexFunctions::split('\n', this->logContent);
    string result = "";
    for ( auto line : lines )
    {
      if ( line.substr(0, 5) == "ERROR" )
      {
        result += color::rize(line, "White", "Red") + "\n";
        continue;
      }
      if ( line.substr(0, 7) == "WARNING" )
      {
        result += color::rize(line, "White", "Yellow") + "\n";
        continue;
      }
      else
        if ( allErrors )
          result += line + "\n";
    }
    return result;
  }

  static std::atomic<Logger*> instance;
  static std::mutex loggerMutex;
};

std::atomic<Logger*> Logger::instance;
std::mutex Logger::loggerMutex;

#endif
