
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

#include "extractName.cpp"


using namespace std;



int main(int argc, char* argv[])
{
  
  string releases[15][2] = { { "2036.Nexus.Dawn.2017.1080p.BluRay.x264.FLAME", "2036.Nexus.Dawn" },
                             { "Novitiate.2017.1080p.BluRay.X264-AMIABLE", "Novitiate" },
                             { "The.Handmaids.Tale.S02E12.1080p.HDTV.x264-MTB", "The.Handmaids.Tale" },
                             { "1922.2017.1080p.WEBRip.x264-iNTENSO", "1922" },
                             { "1922.test.2017.1080p.WEBRip.x264-iNTENSO", "1922.test" },
                             { "1914.The.War.Revolution.SWESUB.PDTV.XviD-APB", "1914.The.War.Revolution" },
                             { "1000.Mexicans.2016.1080p.BluRay.x264-PussyFoot", "1000.Mexicans" },
                             { "17.Again.1080p.BluRay.x264-REFiNED", "17.Again" },
                             { "The.Matrix.1999.COMpLETE.de.2D.BLUeRAY.x264.1080p-landev", "The.Matrix" },
                             { "1984.1984.COMpLETE.de.2D.BLUeRAY.x264.1080p-landev", "1984" },
                             { "1984.to.1999.1984.COMpLETE.de.2D.BLUeRAY.x264.1080p-landev", "1984.to.1999" },
                             { "show.EN.S01E01.x264.1080p-landev", "show.EN" },
                             { "Blade.Runner.2049.2017.1080p.BluRay.x264-SPARKS", "Blade.Runner.2049" },
                             { "A.Swedish.Love.Story.1970.1080p.BluRay.x264-RRH", "A.Swedish.Love.Story" },
                             { "Monkey.island.windows.german.ASDF", "Monkey.island.windows.german" }
                           };
  
  for(int i = 0; i < 14; i++)
  {
    string result = ExtractName::extractName(releases[i][0]);
    if(releases[i][1] == result)
    {
      cout << color::rize(result, "White", "Green")  << " --> ";
      cout << ExtractName::extractInfo(releases[i][0], result) << endl;
    }
    else
      cout << color::rize(">> " + result, "White", "Red")  << endl;
  }
}
