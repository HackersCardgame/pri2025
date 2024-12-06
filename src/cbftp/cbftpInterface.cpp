
#include <chrono>
#include <iostream>
#include <future>
#include <string>
#include <cstring>
#include <vector>

#include <thread>
#include <chrono>

#include <cstdlib>
#include <cstdio>
#include <stdio.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>  //for gethostbyname


#include "../lib/colorize/color.hpp"


using namespace std;

#ifndef CBFTP_INTERFACE
#define CBFTP_INTERFACE


/*******************************************************************************
* interface over UDP to cbftp-client. works with all clients also non-modified
* but the cbftp-client with the pHase mod can automatically determine section
* from longer section strings. so you can send TV.x264.3D.WEB.DE
*                                              =================
*
* if a sites has the folliwing directories
* TV = /incoming/tv
* MOVIES = /incoming/movies
* -> it will go to TV

* if a sites has the folliwing directories
* x264 = /incoming/x264
* x264 = /incoming/x265
* -> it will go to x264

* if a sites has the folliwing directories
* TV = /incoming/tv
* TV.3D = /incoming/tv-3d
* MOVIES = /incoming/movies
* -> it will go to TV.3D

* if a sites has the folliwing directories
* TV = /incoming/tv
* TV.3D = /incoming/tv-3d
* MOVIES = /incoming/movies
* -> it will go to TV.3D


* if a sites has the folliwing directories
* TV
* TV.DE = /incoming/tv-de
* TV.DE = /incoming/tv-se
* MOVIES = /incoming/movies
* MOVIES.DE = /incoming/movies-de
* MOVIES.DE = /incoming/movies-se
* -> it will go to TV.DE

* with the non-modified cbftp client you could not do that or you would have
* to add hundreds of sections for each possible combination, with the
* phase modified cbftp you dont need to. it begins with the longest combination
* and takes the first (the more elements the earlier checked) where all
* items match
*******************************************************************************/
class CbftpInterface
{
  static mutex cbftpMutex;


  private:
    inline static const bool debug = false;

  public:
    
    static int sendRelease(string host, string port, string password, string messageType, string section, string release, vector<string> &siteArray)
    {      
      string sites = "";
      
      for ( auto site : siteArray )
        sites += "," + site;

      string message = password + " " + messageType + " " + "."+section+"." + " " + release + " " + sites.substr(1);

      if ( CbftpInterface::debug ) cout << message << endl;
    
      return send(host, port, password, messageType, message);
    }
    
          
    static int sendSetting(string host, string port, string password, string messageType, string site, string settings)
    {      
      string message = password + " " + messageType + " " + site + " " + settings;

      if ( CbftpInterface::debug ) cout << "MSG: " << message << endl;
    
      return send(host, port, password, messageType, message);
    }




    static int send(string host, string port, string password, string messageType, string message)
    {
    
      if ( CbftpInterface::debug ) cout << "send: " << message << endl;

      std::lock_guard<std::mutex> guard(cbftpMutex);
      int sockfd;
//      char buffer[1024];
      
      const char* cmessage = message.c_str();


      struct sockaddr_in serv_addr;
      
      //creating socket file descriptor
      if ( ( sockfd = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
      {
        cout << "Socket Creation Failed" << endl;
        exit(EXIT_FAILURE);
      }
      
      
      struct hostent *he;
      struct in_addr **addr_list;

      char* hostname = const_cast<char *>(host.c_str());
      
      if ( ( he = gethostbyname( hostname ) ) == NULL )
      {
        cout << "HOST NAME RESOLUTION E-R-R-O-R: " << host << endl;
        return -1;
      }

      if ( CbftpInterface::debug ) cout << "hostname found" << endl;

      
      addr_list = (struct in_addr **) he->h_addr_list;
      
      string ip;
      
      for ( int i = 0; addr_list[i] != NULL; i++ )
      {
//        strcpy(ip, inet_ntoa(*addr_list[i]) );
        ip = inet_ntoa(*addr_list[i]);
        if ( CbftpInterface::debug ) cout << ip << endl;  //TODO: CHECK IF IPv6 works too
      
      }

      if ( CbftpInterface::debug ) cout << "ip found" << endl;
      
      memset(&serv_addr, 0, sizeof(serv_addr));
      
      int portInt = 9999;
      
      serv_addr.sin_family = AF_INET;
//      serv_addr.sin_port = htons(55477);
      try { portInt = stoi(port); } catch (...) { cout << "prot error " << endl; }
      serv_addr.sin_port = htons(portInt);
//      serv_addr.sin_addr.s_addr = INADDR_ANY;
      serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());   //Destination Address
      
//      int n;
//      int len;

      
      sendto(sockfd, (const char *)cmessage, strlen(cmessage), MSG_CONFIRM, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));

      close(sockfd);
      
      return 0;
      
    }



    
};


mutex CbftpInterface::cbftpMutex;
#endif
