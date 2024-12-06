
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

#include "../lib/colorize/color.hpp"

#include "../processRules/checkRuleSyntax.cpp"


using namespace std;

//std::atomic<ConfigFileReader*> ConfigFileReader::instance;
//std::mutex ConfigFileReader::myMutex;

int main(int argc, char* argv[])
{

  chrono::system_clock::time_point begin;
  chrono::system_clock::time_point end;

  begin = std::chrono::high_resolution_clock::now();

  unordered_map<string, bool> testCases = { { "x264 x265 720p 1080p", true },
                                            { "movies tv std 720p", true },
                                            { "x264 x265  xyz 1080p", false },
                                            { "x264 x265 xyz 1080p ", false },
                                            { " x264 x265 xyz 1080p", false },
                                            { "tv 720p nw_us nw_gb hdtv h264 web webrip x264 currentseason documentary reality talk-show scripted language:(english) std", true }  
                                           };
  
  int errors = 0;

  for ( auto testCase : testCases )
  {
    cout << color::rize("TESTING : ", "Black", "Yellow") << "'"+testCase.first+"'  -> " << CheckRuleSyntax::checkSyntax(testCase.first) << endl;    
    if ( ( testCase.first == CheckRuleSyntax::checkSyntax(testCase.first) ) != testCase.second )
    {
      cout << color::rize("ERROR '"+testCase.first+"'", "White", "Red") << " Please investigate" << endl;
      errors++;
    }
    else
      cout << color::rize("SUCCESS " + testCase.first, "Black", "Green") << endl;
  }
      
  if (!errors) cout << color::rize("All tests run successfully", "White", "Green") << endl;

  end = std::chrono::high_resolution_clock::now();
  string text = "10x ConfigFileReader::parse()";
  cout << HelperFunctions::sprintTime(text, begin, end);

  return errors;
}



//TODO: testcases with pipes | and brackets () and some with unclosed brackets, TODO alert if brackets not closed

