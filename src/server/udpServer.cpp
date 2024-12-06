
// Server side implementation of UDP client-server model 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../lib/colorize/color.hpp"

#include "../processRelease/parseRelease.cpp"

#include "../server/tcpServer.cpp"
#include "../server/tcpServerListener.cpp"


#define MAXLINE 1024

using namespace std;

#ifndef UDP_SERVER
#define UDP_SERVER

/*******************************************************************************
* udp part of the server that listens on the bots from irc and then
* sends matching releases to the clients (phaseClient) and to its
* cbftp clients
*******************************************************************************/
class UdpServer
{

  private:
    inline static const bool debug = false;
    
    inline static string UDPHOST = "localhost";
    inline static int UDPPORT = 5555;
    inline static ParseRelease pr = ParseRelease();


    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();
    inline static thread refreshThread;

    static void refresh()
    {
      for (;;)
      {
        this_thread::sleep_for(std::chrono::milliseconds(60000));
        cout << "["+DateTimeFunctions::getTimeStamp() +"] " << color::rize("Updating cache from Database (because maybe another Node updated Database)", "Blue") << endl;
        cfr->buildCache();
      }
    }

  public:
    UdpServer()
    {
      pr = ParseRelease();
      thread tcpThread = thread(TcpServer::init);
      thread udpThread = thread(UdpServer::initSocket);
      thread refreshThread = thread(UdpServer::refresh);

//      UdpServer::initSocket();
//TODO: restart if one crashed
      tcpThread.join();
      udpThread.join();
      refreshThread.join();

    }
    
    ~UdpServer()
    {
    
    }
  
    static string resolveHostname(string host)
    {
      struct hostent *he;
      struct in_addr **addr_list;

      char* hostname = const_cast<char *>(host.c_str());
      
      if ( ( he = gethostbyname( hostname ) ) == NULL )
      {
        cout << "HOST NAME RESOLUTION ERROR: " << UDPHOST << endl;
        return "127.0.0.1";
      }
      
      addr_list = (struct in_addr **) he->h_addr_list;
      
      string ip;
      
      for ( int i = 0; addr_list[i] != NULL; i++ )
      {
//        strcpy(ip, inet_ntoa(*addr_list[i]) );
        ip = inet_ntoa(*addr_list[i]);
        if ( UdpServer::debug) cout << ip << endl;  //TODO: CHECK IF IPv6 works too
      }
      
      return ip;
    }
  
// Driver code
    static int initSocket()
    {
      cout << "["+DateTimeFunctions::getTimeStamp() +"] " << color::rize("STARTING UDP SERVER", "Yellow") << endl;
      int sockfd;
      char buffer[MAXLINE];
      struct sockaddr_in servaddr, cliaddr;
        
      // Creating socket file descriptor 
      if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
      {
          perror("socket creation failed");
          exit(EXIT_FAILURE);
      }
      else
        cout << "["+DateTimeFunctions::getTimeStamp() +"] " + "UDP Socket successfully created.." << endl;

      int opt = 1;

      setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
      setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

      memset(&servaddr, 0, sizeof(servaddr));
      memset(&cliaddr, 0, sizeof(cliaddr));
      

      string ip = resolveHostname(UDPHOST);

      // Filling server information
      servaddr.sin_family    = AF_INET; // IPv4
//      servaddr.sin_addr.s_addr = INADDR_ANY;
      servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
      servaddr.sin_port = htons(UDPPORT);
        
      // Bind the socket with the server address
      if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
      {
          perror("bind failed");
          cout << "["+DateTimeFunctions::getTimeStamp() +"] " << color::rize("UDP Socket successfully failed:", "White", "Red") << " " << color::rize(UDPHOST, "Blue") << ":" << color::rize(to_string(UDPPORT), "Green") << endl;

          exit(EXIT_FAILURE);
      }
      else
      {
        cout << "["+DateTimeFunctions::getTimeStamp() +"] " << color::rize("UDP Socket successfully bound:", "Green") << " " << color::rize(UDPHOST, "Blue") << ":" << color::rize(to_string(UDPPORT), "Green") << endl;
      }
      
      vector<string> history = {};
        
      unsigned int len, n;
      
      while (true)
      {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
        buffer[n] = '\0';
        cout << "["+DateTimeFunctions::getTimeStamp() +"] " << color::rize("UDP Client:", "White", "Megenta") << " " << color::rize(string(buffer), "Yellow") << endl;
        string releaseName = buffer;
        if (std::find(history.begin(), history.end(), releaseName) == history.end())
        {
          cout << "["+DateTimeFunctions::getTimeStamp() +"] " << color::rize("Acceppted:", "White", "Green") << " " << color::rize(releaseName, "Light Green", "Default", "Blink") << endl;
          history.push_back(releaseName);
          if ( history.size() > 100 )
            history.erase(history.begin());
          Release rls = Release(buffer);
          rls.log = true;
          rls.allErrors = false;
          pr.parse(rls, false);
          cout << rls.sprint("all") << endl;
          TcpServerListener::sendAll(rls.sprint("all"));
        }
        else
        {
          cout << "["+DateTimeFunctions::getTimeStamp() +"] " << color::rize("Duplicated:", "White", "Red") << " " << color::rize(releaseName, "Red") << " (skipping)" << endl;
        }
      }

      return 0;
    }
};
#endif
