
#include <chrono>
#include <iostream>
#include <future>
#include <string>

#include <thread>
#include <chrono>

#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <mysql.h>

#include "../lib/colorize/color.hpp"

#include "../cbftp/cbftpConfigSetter.cpp"

using namespace std;

int main(int argc, char **argv)
{
  CbftpConfigSetter::sendAllSites("user01.cfg");
  return 0;
}




