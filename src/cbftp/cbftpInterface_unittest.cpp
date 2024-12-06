
#include <chrono>
#include <iostream>
#include <future>
#include <string>
#include <cstring>

#include <thread>
#include <chrono>

#include <cstdlib>
#include <cstdio>
#include <stdio.h>

#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 


#include "../cbftp/cbftpInterface.cpp"

using namespace std;

string getXY(string a, string b, string c, string d, string e, string f, vector<string> g)
{
  CbftpInterface::sendRelease("localhost","55477","123456","prepare","TV.1080p.EN.EXTERNAL.asdf,asdf,asdf,asdf,we.stdapps","test.S01E01.x264.hdtv-ASDF", g);
  return a;
}


int main()
{
  vector<string> sites = { "SITE1", "SITE2" };
  
  
  for ( int i = 0; i < 1000; i++ )
    auto fut8= std::async(std::launch::async, getXY, "localhost","55477","123456","prepare","TV.1080p.EN.EXTERNAL.asdf,asdf,asdf,asdf,we.stdapps","test.S01E01.x264.hdtv-ASDF", sites);

  CbftpInterface::sendRelease("localhost","55477","123456","prepare","TV.1080p.EN.EXTERNAL.asdf,asdf,asdf,asdf,we.stdapps","test.S01E01.x264.hdtv-ASDF", sites);

};
