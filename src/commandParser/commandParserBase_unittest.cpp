

#include "../commandParser/commandParserBase.cpp"



int main()
{
  cout << CommandParserBase::execute("debug dbstatus", "user", false) << endl;
  
  cout << CommandParserBase::execute("test the.matrix.1999.x264.1080p.web-ASDF", "user", false) << endl;
  
  cout << CommandParserBase::execute("test -d the.matrix.1999.x264.1080p.web-ASDF", "user", false) << endl;
  
  cout << CommandParserBase::execute("test the.matrix.1999.x264.1080p.web-ASDF -d", "user", false) << endl;
  
  cout << CommandParserBase::execute("site", "user", false) << endl;
  
  cout << CommandParserBase::execute("site ZH", "user", false) << endl;
  
  cout << CommandParserBase::execute("site ZH blacklist", "user", false) << endl;

  cout << CommandParserBase::execute("site ZH name", "user", false) << endl;
      
  cout << CommandParserBase::execute("site ZH rule", "user", false) << endl;

  cout << CommandParserBase::execute("site ZH rule TV.720p", "user", false) << endl;

  cout << CommandParserBase::execute("debug checkall", "user", false) << endl;

  cout << CommandParserBase::execute("debug general", "user", false) << endl;

  cout << CommandParserBase::execute("debug generalcache", "user", false) << endl;

  cout << CommandParserBase::execute("debug usercache", "user", false) << endl;

  cout << CommandParserBase::execute("debug sitecache", "user", false) << endl;

  cout << CommandParserBase::execute("help", "user", false) << endl;

  cout << CommandParserBase::execute("user", "user", false) << endl;

  cout << CommandParserBase::execute("site", "user", false) << endl;

  //TODO CAUSES CRASH
  //cout << CommandParserBase::execute("admin", "user", false) << endl;

  cout << CommandParserBase::execute("who", "user", false) << endl;

  cout << CommandParserBase::execute("whoami", "user", false) << endl;

  cout << CommandParserBase::execute("debug updateCache", "user", false) << endl;

  cout << CommandParserBase::execute("tvmaze the big bang theory", "user", false) << endl;

  cout << CommandParserBase::execute("tvmaze the big bang theory s10e10", "user", false) << endl;

  cout << CommandParserBase::execute("imdb black panther", "user", false) << endl;

  cout << CommandParserBase::execute("imdb black panther 2018", "user", false) << endl;

  cout << CommandParserBase::execute("imdb 2018 black panther", "user", false) << endl;

  cout << CommandParserBase::execute("list", "user", false) << endl;

  cout << CommandParserBase::execute("sync all", "user", false) << endl;

  cout << CommandParserBase::execute("sync bbr", "user", false) << endl;

  cout << CommandParserBase::execute("sync asdfasdfasdfasd", "user", false) << endl;



  /*
  
  cout << CommandParserBase::execute("site BBR control", "user", false) << endl;
  cout << CommandParserBase::execute("site BBR rule", "user", false) << endl;
  cout << CommandParserBase::execute("site BBR rule TV", "user", false) << endl;
  cout << CommandParserBase::execute("site", "user", false) << endl;
  cout << CommandParserBase::execute("site", "user", false) << endl;
  cout << CommandParserBase::execute("site", "user", false) << endl;
  */
}
