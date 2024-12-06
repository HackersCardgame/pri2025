
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

#include "../processRelease/parseRelease.cpp"


//TODO: KOH.cfg Alita.bATTLE.ANGEL.2019.German.DL.1080p.UHD.Bluray.x264-miHD.

using namespace std;

vector<string> tests = {"The_Silver_Case_PS4-Playable", 
                        "it.2017.REPACK.BDRip.x264-GECKOS", 
                        "2036.Nexus.Dawn.2017.1080p.BluRay.x264-FLAME",
                        "Atomica.2017.1080p.BluRay.x264-ROVERS", 
                        "Chips.2017.1080p.BluRay.x264-DRONES", 
                        "Blade.Runner.2049.2017.1080p.BluRay.x264-SPARKS",
                        "2012.2017.1080p.BluRay.x264-SPARKS",
                        "Carson.Daly.2019.02.14.Jason.Blum.PROPER.WEB.x264-CookieMonster", 
                        "Conan.2019.02.14.Aubrey.Plaza.WEB.x264-TBS",
                        "88.Heroes.Update.v1.04.PS4-PRELUDE",
                        "Wulverblade.eShop.NSW-BigBlueBox",
                        "Xenoblade.Chronicles.2.NSW-BigBlueBox",
                        "Yesterday.Origins.EUR.NSW-BigBlueBox",
                        "After.The.End.The.Harvest.Update.v1.3.0-PLAZA",
                        "Age.of.Barbarian.Extended.Cut.The.Slaves.Fortress.Update.v1.8.4-PLAZA",
                        "Age.of.Empires.Definitive.Edition.Update.v1.3.5314-CODEX",
                        "Formula1.2018.Abu.Dhabi.Grand.Prix.Practice.One.720p.HDTV.x264-VERUM",
                        "Formula1.2018.Brazilian.Grand.Prix.Practice.Three.720p.HDTV.x264-VERUM",
                        "Formula1.2018.Mexican.Grand.Prix.1080p.HDTV.x264-VERUM",
                        "Welcome.To.Sweden.2014.S01E01.SWEDiSH.720p.HDTV.x264-xD2V",
                        "Wermland.Forever.S01E06.SWEDiSH.PDTV.x264-PRiNCE",
                        "Adventure.Time.Pirates.of.the.Enchiridion-PLAZA",
                        "Affliction-HI2U",
                        "Truefire.Frank.Vignolas.Jazzin.the.Blues.Vol.1.TUTORiAL-ADSR",
                        "Udemy.Learn.Ableton.Live.in.a.Day.Complete.Production.Course.TUTORiAL-ADSR",
                        "Academy.fm.Halloween.Project.Walkthrough.In.Ableton.Live.TUTORiAL-ADSR",
                        "UCL.2018.11.27.Group.H.Manchester.United.Vs.Young.Boys.720p.HDTV",
                        "V8.Supercars.2018.Coates.Hire.Newcastle.500.Day.3.Coverage.1080p",
                        "the.big.bang.theory.S09E09.720p.HDTV.x264-KYR",
                        "WWE.Smackdown.Live.2018.11.27.720p.HDTV.x264-KYR"};

  ParseRelease pr = ParseRelease();

static void subProc(string release)
{
//  string unified = RegexFunctions::unify(release);
  Release rls = Release(release);
  //cout << "going to parse release now" << endl;
  rls.log = true;
  rls.allErrors = true;
  pr.parse(rls, true);
  cout << rls.sprint() << endl;
}


int main(int argc, char* argv[])
{
  thread threads[1000];
  int i=0;

  //ConfigFileReader* cfr = ConfigFileReader::getInstance();
  //cfr->buildCache();

  if (argc > 1)
  {
    subProc(argv[1]);
  }
  else
  {

    for(auto release : tests)
    {
      threads[i] = thread(subProc, release);
      i++;
    }

    for ( int j = 0; j < i; j++ )
    {
      threads[j].join();
    }
  }

  cout << color::rize("TESTS PARRALEL, just for checking if the locks work and nothing crashes", "Yellow") << endl;
  cout << color::rize("the locks seem to work and we dont have any crash", "Green") << endl;
  cout << "(this is slower than one after another, because the release blocks each other)" << endl;
  cout << color::rize("even though it has only 3 seconds for 30 releases = 0.1s", "Green") << endl;
  cout << "-----------------------------------------------------------------------------" << endl;
  cout << "now we wait 10 seconds" << endl;
  this_thread::sleep_for(std::chrono::milliseconds(10000));
  for(auto release : tests)
  {
    subProc(release);
  }

}
