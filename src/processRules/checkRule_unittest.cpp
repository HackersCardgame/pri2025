
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
#include "../processRules/checkRule.cpp"
#include "../processRelease/parseRelease.cpp"


using namespace std;
;

int main(int argc, char* argv[])
{

  chrono::system_clock::time_point begin;
  chrono::system_clock::time_point end;

  begin = std::chrono::high_resolution_clock::now();

  //Release rls("The.MaTriX.1999.x264.web.1080p-ASDF");
  Release rls("hjp-und-ub-spinnen-flt");
  
  FeatureExtractor::getFeatures(rls);
  FeatureExtractor::getSection(rls);
  cout << rls.features["SECTION"] << endl;


  cout << "checkRule() template +" << CheckRule::checkRule("BBR.cfg", "nodupe", "noduple", rls) << endl;
  cout << "checkRule() template -" << CheckRule::checkRule("BBR.cfg", "releasedupe", "releasedupe", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "TV", "TV x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "GAMES", "GAMES x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "MOVIES", "MOVIES x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "TV", "TV x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "GAMES", "GAMES x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "MOVIES", "MOVIES x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "TV", "TV x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "GAMES", "GAMES x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "MOVIES", "MOVIES x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "TV", "TV x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "GAMES", "GAMES x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "MOVIES", "MOVIES x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "TV", "TV x264 TV", rls) << endl;
  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "GAMES", "GAMES x264 TV", rls) << endl;
  
  Release rls2("the.big.bang.theory.S12E24.x264.web.1080p-ASDF");
  
  //FeatureExtractor::getFeatures(rls);
  //FeatureExtractor::getSection(rls);
  //cout << rls.features["SECTION"] << endl;
  cout << "MARK1" << endl;
  ParseRelease pr = ParseRelease();
  pr.parse(rls2, true);
  cout << "MARK2" << endl;


  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "TV", "TV x264 TV currentseason", rls2) << endl;

  Release rls3("the.big.bang.theory.S11E24.x264.web.1080p-ASDF");
  
  //FeatureExtractor::getFeatures(rls);
  //FeatureExtractor::getSection(rls);
  //cout << rls.features["SECTION"] << endl;
  cout << "MARK1" << endl;
  pr.parse(rls3, true);
  cout << "MARK2" << endl;


  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "TV", "TV x264 TV currentseason", rls3) << endl;

  Release rls4("the.big.bang.theory.S13E24.x264.web.1080p-ASDF");
  
  //FeatureExtractor::getFeatures(rls);
  //FeatureExtractor::getSection(rls);
  //cout << rls.features["SECTION"] << endl;
  cout << "MARK1" << endl;
  pr.parse(rls4, true);
  cout << "MARK2" << endl;


  cout << "checkRule() " << CheckRule::checkRule("BBR.cfg", "TV", "TV x264 TV currentseason", rls4) << endl;

  end = std::chrono::high_resolution_clock::now();
  string text = "10x ConfigFileReader::parse()";
  cout << HelperFunctions::sprintTime(text, begin, end);

  return 0;
}



//TODO: testcases with pipes | and brackets () and some with unclosed brackets, TODO alert if brackets not closed

