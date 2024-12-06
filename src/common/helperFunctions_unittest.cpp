

#include "../common/helperFunctions.cpp"


int main()
{

  vector<vector<string>> tests1 = { { "a", "a" },
                                  { "aa", "aa" },
                                  { "aaa", "aaa" },
                                  { "aaaa", "aaaa" },
                                  { "aaaa", "aaab" },
                                  { "a", "b" },
                                  { "matrix", "Matrix" },
                                  { "matrix", "marix" },
                                  { "matrix", "atr" },
                                  { "black panter", "black panther" },
                                  { "bak panter", "lak panther" },
                                  { "it", "it" },
                                  { "it", "jit" },
                                  { "it", "git" },
                                  { "asdkfajhndsjvh", "askdgha" } };


  for ( auto test : tests1 )
  {
    cout << test.at(0) << " <--> " << test.at(1) << " forward: ";
    cout << to_string(HelperFunctions::similar((char*)test.at(0).c_str(), (char*)test.at(1).c_str())) << " reverse: ";
    cout << to_string(HelperFunctions::similar((char*)test.at(1).c_str(), (char*)test.at(0).c_str()));
    cout << endl;
  }

}


