
#include <iostream>

#include "../common/regexFunctions.cpp"


using namespace std;


int main()
{
  cout << "STaRT" << endl;
  vector<string> years = RegexFunctions::findAll("\\d\\d\\d\\d", "1984.1990.x264.1080p-ASDF");
  for(auto year : years)
    cout << ":: " << year << endl;
  for (int i = 0; i < 1000; i++)
  {
    cout << RegexFunctions::match("[._-]x264[._-]", "matrix.1999.x264-ASDF");
    cout << RegexFunctions::match("[._-]x265[._-]", "matrix.1999.x264-ASDF");
    cout << RegexFunctions::occurances("[aij"+to_string(i%9)+"st]", "smmmimma4");
  }
  
  auto vec = RegexFunctions::split(' ', " TEST TEST ");
  for (auto i : vec)
    cout << "-" << i << "-" << endl;
  
  cout << "REPLACE" << RegexFunctions::replace("\\.", " ", "asdf.qwer.asdf.dafg.qadf.qwer") << endl;
  
  cout << "REPLACE" << RegexFunctions::replace("as", "XX", "asdf.qwer.asdf.dafg.qadf.qwer") << endl;

  cout << "." << RegexFunctions::trim("  test  ") << "." << endl;
  

  return 0;
}
