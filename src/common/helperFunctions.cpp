
#include <time.h>
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>

#include <mysql.h>

//#include "../lib/Jaro-Winkler/cpp/jwdistance.h"
#include "../lib/jarowinkler.c"
#include <math.h>

#include "../common/dbConnection.cpp"

using namespace std;

#ifndef HELPER_FUNCTIONS
#define HELPER_FUNCTIONS

//TODO: irgend nen off by one error bei den threads

/*******************************************************************************
* does what it says: class with helperfunctions like jaro winkler distnace 
* and paralell processing of sql results...
*******************************************************************************/
class HelperFunctions
{

  private:
    inline static int maxDeltaYear = 2;                                         //maximal difference between year specified in release and year in imdb
    inline static float minSimilarity = 0.85;                                   //minimal similarity to select a movie row TODO: Maybe put to 0.65, someone should check XXX

  public:
    inline static const bool debug = false;
    
    static char** findBest(char*** rows, int num_rows, int num_cols, string title, int titleColumnNo)
    {
      if (HelperFunctions::debug) cout << num_rows << endl;
      char ** bestRow = subProcess(rows, 0, num_rows, num_cols, 0, num_rows, &title[0], titleColumnNo);
      return bestRow;
    }
    
    //TODO: doing database stuff here suboptimal, maybe we should have a parameter that says what row to check for
    static char** subProcess(char*** rows, int processNo, int num_rows, int num_cols, int start, int end, char* cTitle, int titleColumnNo)
    {
      if (HelperFunctions::debug) cout << "Process started: " << processNo << endl;

      float bestSimilarity = 0;
      float similarity = 0;
      
      char** bestRow = new char*[num_cols];
      
      for (int i = 0; i < num_cols  ; i++)
      {
        bestRow[i] = (char*)"NONE";
      } 

      for (int i = start; i < end; i++)
      {
        similarity = HelperFunctions::similar(cTitle, rows[i][titleColumnNo]);

        if (similarity > minSimilarity)
          if (similarity > bestSimilarity)
          {
            if (HelperFunctions::debug) cout << "TAKEN: " << rows[i][0] << " " << rows[i][titleColumnNo] << endl;
            bestRow = rows[i];
            bestSimilarity = similarity;
          }
      }
      return bestRow;
    }
        
    static float similar(char* a, char* b)
    {
      int lena = strlen(a);
      int lenb = strlen(b);
      
      if (HelperFunctions::debug) cout << lena;
      if (HelperFunctions::debug) cout << lenb << endl;

      char c[lena+1];
      for (int i = 0; i < lena; ++i)
        c[i] = tolower(a[i]);
      c[lena] = '\0';

      char d[lenb+1];
      for (int i = 0; i < lenb; ++i)
        d[i] = tolower(b[i]);
      d[lenb] = '\0';

      double similarity = jaro_winkler_distance(c, d);
      
      double bonus = 0;
      
      if ( lena == lenb )
        bonus = 0.1;
      else
        bonus   = 0.1 * 1/(fabs(lena-lenb));
      
      if (HelperFunctions::debug) cout << endl;
      if (HelperFunctions::debug) cout << similarity << " " << bonus << ": '" << a << "' <-> '" << b << "'" << endl;
      if (HelperFunctions::debug) cout << similarity << " " << bonus << ": '" << string(c) << "' <-> '" << string(d) << "'" << endl;
      return similarity + bonus;
    }
    
    static void toLower(string& text)
    {
      std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c){ return std::tolower(c); });    
    }

    static string sprintTime(string text, chrono::time_point<chrono::system_clock> start_point, chrono::time_point<chrono::system_clock> end_point)
    {
      typedef chrono::duration<float> fsec;
      fsec fs = end_point - start_point;
      float time = fs.count()*1000;
      string ms = to_string(time);
      string result = "";
      if (time < 1)
        result += text + " TIME: " + color::rize(ms + "ms", "Green") + "\n";
      if (time > 1 && time < 100)
        result += text + " TIME: " + color::rize(ms + "ms", "Yellow") + "\n";
      if (time > 100)
        result += text + " TIME: " + color::rize(ms + "ms", "Light Red") + "\n";
      return result;
    }    

};

#endif
