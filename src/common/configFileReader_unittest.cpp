
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

#include "../common/configFileReader.cpp"


using namespace std;

//std::atomic<ConfigFileReader*> ConfigFileReader::instance;
//std::mutex ConfigFileReader::myMutex;

int main(int argc, char* argv[])
{

  chrono::system_clock::time_point begin;
  chrono::system_clock::time_point end;

  ConfigFileReader* cfr = ConfigFileReader::getInstance();
  
  begin = std::chrono::high_resolution_clock::now();
  
  cfr->buildCache();
  //cout << cfr->getAllConfigFiles().array[0][0] << endl;

//  cout << cfr->userCaches["user01.cfg"]->directory["password"]["*"] << endl;
  cout << "general.cfg " << cfr->generalCache.content["order"]["ALL"] << endl;

  cout << "userxy.cfg " << cfr->userCaches["user01.cfg"]->content["password"]["*"] << endl;
  
  for ( auto item : cfr->userCaches["user01.cfg"]->content )
   cout << "ITEM: " << item.first << endl;

  cout << "sitexy.cfg " << cfr->siteCaches["BBR.cfg"]->content["blacklist"]["*"] << endl;

  end = std::chrono::high_resolution_clock::now();
  string text = "10x ConfigFileReader::parse()";
  cout << HelperFunctions::sprintTime(text, begin, end);

  return 0;
}



//TODO: testcases with pipes | and brackets () and some with unclosed brackets, TODO alert if brackets not closed

