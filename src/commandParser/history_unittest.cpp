
#include <time.h>   //for timestruct tm and mktime
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>  //for get_time()
#include <sstream>
#include <iostream>


#include "../server/history.cpp"

using namespace std;


int main()
{
  History::writeHistory("user01", "command", "tv", "tv");
  History::writeHistory("user01", "testaction", "testcommand", "testrelease");
  History::writeHistory("user01", "cbftp", "testcommand", "testrelease");
  History::writeHistory("user01", "cbftp", "testcommand", "testrelease32");
  History::writeHistory("user01", "testaction", "testcommand", "testrelease");
  History::writeHistory("user02", "testaction", "testcommand", "testrelease");
  History::writeHistory("user02", "testaction", "testcommand", "testrelease");
  History::writeHistory("user02", "testaction", "testcommand", "testrelease");
  History::writeHistory("user02", "cbftp", "testcommand", "testreleas2e");
  History::writeHistory("user02", "cbftp", "testcommand", "testrelease33");
  History::writeHistory("user02", "testaction", "testcommand", "testrelease");
  History::writeHistory("user02", "testaction", "testcommand", "testrelease");
  cout << "u1: " << History::lastRelease("user01") << endl;
  cout << "u2: " << History::lastRelease("user02") << endl;
  cout << "Hu1: " << History::history("user01", 2) << endl;
  cout << "Hu2: " << History::history("user02", 10) << endl;
}

