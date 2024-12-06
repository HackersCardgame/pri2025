#include<iostream>
#include "jwdistance.h"

using namespace std;

int main(){
  for(int i = 0; i <1; i++)
  {
    float weight = jw_distance("asterix", "obelix");
    cout << weight << endl; 
  }
  return 0;  
}
