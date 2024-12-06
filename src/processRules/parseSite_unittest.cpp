
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

#include "../common/featureExtractor.cpp"
#include "../results/release.cpp"
#include "../processRules/parseSite.cpp"
#include "../common/configFileReader.cpp"

using namespace std;



int main(int argc, char* argv[])
{

  ConfigFileReader* cfr = ConfigFileReader::getInstance();

  chrono::system_clock::time_point begin;
  chrono::system_clock::time_point end;

  begin = std::chrono::high_resolution_clock::now();


  cfr->buildCache();

  end = std::chrono::high_resolution_clock::now();
  string text = "init all";
  cout << HelperFunctions::sprintTime(text, begin, end);



  begin = std::chrono::high_resolution_clock::now();
  
  Release rls("The.MaTriX.1999.x264.web.1080p-ASDF");
  
  FeatureExtractor::getFeatures(rls);
  FeatureExtractor::getSection(rls);

  ParseSite::parseAllSites(rls, cfr->users);

  end = std::chrono::high_resolution_clock::now();
  text = "all parsSite() >> 202 << threads/siteConfigs";
  cout << HelperFunctions::sprintTime(text, begin, end);

  return 0;
}



//TODO: testcases with pipes | and brackets () and some with unclosed brackets, TODO alert if brackets not closed

