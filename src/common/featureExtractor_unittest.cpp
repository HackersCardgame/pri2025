
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


using namespace std;
;

int main(int argc, char* argv[])
{

  chrono::system_clock::time_point begin;
  chrono::system_clock::time_point end;

  begin = std::chrono::high_resolution_clock::now();

  Release rls("The.MaTriX.1999.x264.1080p-ASDF");
  
  cout << "getFeatures()" << endl;
  FeatureExtractor::getFeatures(rls);
  
  cout << "getSection()" << endl;
  FeatureExtractor::getSection(rls);

  
  for (auto feature : rls.features)
    cout << feature.first << ": " << feature.second << endl;
    
  cout << "SECTION: " << rls.section << endl;

  end = std::chrono::high_resolution_clock::now();
  string text = "10x ConfigFileReader::parse()";
  cout << HelperFunctions::sprintTime(text, begin, end);

  return 0;
}



//TODO: testcases with pipes | and brackets () and some with unclosed brackets, TODO alert if brackets not closed

