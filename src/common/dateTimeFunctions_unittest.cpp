#include <string>
#include <ctime>
#include <iostream>

#include "../common/dateTimeFunctions.cpp"

using namespace std;

int main()
{
  string dateString1 = "2017-05-22";
  string dateString2 = "2017-05-22";
  string dateString3 = "NONE";
  
  tm date1 = DateTimeFunctions::dateFromString(dateString1);
  tm date2 = DateTimeFunctions::dateFromString(dateString2);
  tm date3 = DateTimeFunctions::dateFromString(dateString3);
  
  if ( mktime(&date1) < mktime(&date2) )
    cout << "1 < 2" << endl;
  else
    cout << "1 !< 2" << endl;
      
  DateTimeFunctions::datePlusDays(&date1, -30);

  if ( mktime(&date1) < mktime(&date2) )
    cout << "1 < 2" << endl;
  else
    cout << "1 !< 2" << endl;

  cout << asctime( &date1 ) << asctime( &date1 ) << std::endl;
  
  cout << DateTimeFunctions::toDateString(date1) << " <=" << endl;
  
  cout << DateTimeFunctions::toDateString(date2) << " <-" << endl;  

  cout << DateTimeFunctions::toDateString(date3) << " <-" << endl;
  
  cout << "timestamp: " << DateTimeFunctions::getTimeStamp() << endl;
}


