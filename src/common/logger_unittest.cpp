#include <stdio.h>
#include <string>
#include <iostream>


#include "logger.cpp"

int main(){
  
  Logger::getInstance()->info("TEST MESSAGE", debugInfo);
  
  Logger* log = Logger::getInstance();
  
  log->error("ERROR MESSAGE", debugInfo);

  cout << log->getLogContent();

}
