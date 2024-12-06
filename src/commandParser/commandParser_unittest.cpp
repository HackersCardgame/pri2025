

#include "../commandParser/commandParser.cpp"



int main()
{


  cout << CommandParser::execute("dbstatus", "user", false) << endl;
  
  cout << CommandParser::execute("test the.matrix.1999.x264.1080p.web-ASDF", "user", false) << endl;
  
  cout << CommandParser::execute("test -d the.matrix.1999.x264.1080p.web-ASDF", "user", false) << endl;
  
  cout << CommandParser::execute("site", "user", false) << endl;
  
  cout << CommandParser::execute("!zh", "user", false) << endl;
  
  cout << CommandParser::execute("site ZH", "user", false) << endl;
  
  cout << CommandParser::execute("site ZH blacklist", "user", false) << endl;

  cout << CommandParser::execute("site ZH name", "user", false) << endl;
      
  cout << CommandParser::execute("site ZH rule", "user", false) << endl;

  cout << CommandParser::execute("site ZH rule TV.720p", "user", false) << endl;

  cout << CommandParser::execute("debug dbstatus", "user", false) << endl;

  cout << CommandParser::execute("debug checkall", "user", false) << endl;

  cout << CommandParser::execute("debug general", "user", false) << endl;

  cout << CommandParser::execute("debug generalcache", "user", false) << endl;

  cout << CommandParser::execute("debug usercache", "user", false) << endl;

  cout << CommandParser::execute("debug sitealcache", "user", false) << endl;

  cout << CommandParser::execute("h import", "user", false) << endl;

  cout << CommandParser::execute("help import", "user", false) << endl;

  cout << CommandParser::execute("u", "user", false) << endl;

  cout << CommandParser::execute("user cbpassword", "user", false) << endl;

  cout << CommandParser::execute("s", "user", false) << endl;

  //TODO CAUSES CRASH
  //cout << CommandParser::execute("admin", "user", false) << endl;

  cout << CommandParser::execute("who", "user", false) << endl;

  cout << CommandParser::execute("whoami", "user", false) << endl;

  cout << CommandParser::execute("updateCache", "user", false) << endl;

  cout << CommandParser::execute("t big bang theory", "user", false) << endl;

  cout << CommandParser::execute("i black panther", "user", false) << endl;

  cout << CommandParser::execute("list", "user", false) << endl;

  cout << CommandParser::execute("us", "user", false) << endl;

  cout << CommandParser::execute("tv", "user", false) << endl;

  cout << CommandParser::execute("site BBR control", "user", false) << endl;

  cout << CommandParser::execute("site BBR rule", "user", false) << endl;

  cout << CommandParser::execute("site BBR rule TV", "user", false) << endl;

  cout << CommandParser::execute("site", "user", false) << endl;

}
