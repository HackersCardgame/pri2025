//comple with g++ release.cpp -std=c++17
//TODO: raii c++ example

#include "release.cpp"
#include "site.cpp"
#include "user.cpp"

using namespace std;

int main()
{
  Release rls = Release("TEST-RLS");
  
  cout << rls.sprint() << endl;
  
  User user = User("user01");

  Site site = Site("BBR");
  

}
