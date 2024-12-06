
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

#include "regexParser.cpp"


using namespace std;


int main(int argc, char* argv[])
{

  chrono::system_clock::time_point begin;
  chrono::system_clock::time_point end;


  RegexParser* rp = RegexParser::getInstance();
  begin = std::chrono::high_resolution_clock::now();
  cout << "crf " << rp->parse("$TVEP", "matrix.S01E01.x264.1080p-ASDF", true) << endl;
  cout << "crf " << rp->parse("!$TVEP", "matrix.S01E01.x264.1080p-ASDF", true) << endl;

  end = std::chrono::high_resolution_clock::now();
  string text = "10x RegexParser::parse()";
  cout << HelperFunctions::sprintTime(text, begin, end);




  cout << "single functions" << endl;
  cout << rp->parsePipes("item1|(item2|item3)|item4|(asdf|asdf|asdf)", "matrix.1999.x264.1080p-ASDF") << endl;
  cout << rp->parsePipes("([._]|SUB|TRUE)FRENCH[._-]", "matrix.1999.x264.1080p-ASDF") << endl;
  

  return 0;
}



//TODO: testcases with pipes | and brackets () and some with unclosed brackets, TODO alert if brackets not closed



/*
# MAIN
# -----------------------------------------------------------------------
def main():

  releaseNames = [ [ "The.Handmaids.Tale.S02E12.1080p.HDTV.x264-MTB", "!$TVEP 1080",     "AND", False ],
                   [ "The.Handmaids.Tale.S02E12.1080p.HDTV.x264-MTB", "$TVEP 1080 X264", "AND", True  ],
                   [ "The.Handmaids.Tale.S02E12.1080p.HDTV.x264-MTB", "!$TVEP !1080",    "OR",  False ],
                   [ "The.Handmaids.Tale.S02E12.1080p.HDTV.x264-MTB", "!$TVEP 1080",     "OR",  True  ],
                   [ "Disgaea.5.Complete.Update.v2019020-CODEX", "$DOX-GROUPS $DOX !Multi\d",     "AND",  True  ],
                   [ "Disgaea.5.Complete.Update.v2019020-CODEX", "$GAME-GROUPS !$DOX",     "AND",  False  ]
                   
                 ]
  
  rp = ParseRegex.getInstance()

  if len(sys.argv)<2:

    print()
    print()
    cprint("U N I T  T E S T s  FOR RegexParser:", 'red', 'on_white') 
    print()

    for releaseName in releaseNames:
      print(releaseName[0])
      result = rp.parse(releaseName[1], releaseName[0], releaseName[2], False)
      if releaseName[3] == result:
        cprint(str(result) + ": " + releaseName[1] + " Logic: " + releaseName[2], 'white', 'on_green')
      else:
        cprint(result, 'white', 'on_red', end="")
        print(" should be: ", end="")
        cprint(releaseName[3], 'white', 'on_blue')

      print("--------------------------------------------------------------------------------")

if __name__ == "__main__": main()
*/
