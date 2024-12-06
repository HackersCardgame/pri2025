#include <cstdio>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <unistd.h>

#include "../lib/colorize/color.hpp"

#include "../processRelease/parseRelease.cpp"
#include "../results/release.cpp"

#include "../commandParser/commandParser.cpp"

#include "../common/regexFunctions.cpp"
#include "../common/configFileReader.cpp"

#include "../common/dateTimeFunctions.cpp"

#include "../server/hashPassword.cpp"



#define MAXTCP 65000
#define SA struct sockaddr

using namespace std;

#ifndef TCP_SERVER_LISTENER
#define TCP_SERVER_LISTENER

//Not sure what headers are needed or not
//This code (theoretically) writes "Hello World, 123" to a socket over a secure TLS connection
//compiled with g++ -Wall -o client.out client.cpp -L/usr/lib -lssl -lcrypto
//Based off of: https://www.cs.utah.edu/~swalton/listings/articles/ssl_client.c
//Some of the code was taken from this post: https://stackoverflow.com/questions/52727565/client-in-c-use-gethostbyname-or-getaddrinfo

//source: https://gist.github.com/vedantroy/d2b99d774484cf4ea5165b200888e414

/*******************************************************************************
* functions that handle things coming from the tcpClient (phaseClient)
*******************************************************************************/
class TcpServerListener
{

  struct userHook
  {
    SSL * ssl;
    int connFd;
    string userName;
    bool authenticated = false;
  };

  inline static vector<userHook> userHooks;

  private:
    inline static const bool debug = false;
    
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();
    inline static ParseRelease pr = ParseRelease();
    
    TcpServerListener()
    {
      pr = ParseRelease();
    }
    
    ~TcpServerListener()
    {
    
    }

  public:

    static string getMachineInfo()
    {
      char hostname[HOST_NAME_MAX];
      char username[LOGIN_NAME_MAX];
      int result;
      result = gethostname(hostname, HOST_NAME_MAX);
      if (result)
        {
          perror("gethostname");
          //exit(EXIT_FAILURE);
        }
      result = getlogin_r(username, LOGIN_NAME_MAX);
      if (result)
        {
          perror("getlogin_r");
          //exit(EXIT_FAILURE);
        }
      string machineInfo = /*string(username) +*/ "@" + string(hostname);
      return machineInfo;
    }



    static void printClientLog ( int socketFD , string message, string color1, string color2 )       
    {       
      struct sockaddr_in address = {0};       
      //socklen_t addressLength = sizeof(address);       
      string ip;       
      int port;       
      socklen_t addressLength = sizeof(address);
      getpeername ( socketFD , (struct sockaddr*) &address , &addressLength );       
      //cout << "Address length is " << addressLength << "     Return is " <<  result << endl;       

      ip = inet_ntoa ( address.sin_addr );       
      port = ntohs ( address.sin_port );       

      cout << "["+DateTimeFunctions::getTimeStamp() +"] ("  << socketFD << ") " << ip << ":" << port << " " <<  color::rize(message, color1, color2) << endl;       
    } 

    // Function that that listens on messages from TCP SERVER
    static void clientListener(SSL* ssl, int sslfd)
    {
      printClientLog(sslfd, "NEW CLIENT LISTENER (connected)", "Green", "Default");

      //cout << "["+DateTimeFunctions::getTimeStamp() +"] " + color::rize("NEW CLIENT LISTENER:", "Green") << " " << to_string(sslfd) << " opened" << endl;
      char buff[MAXTCP];
    
    
      if (SSL_accept(ssl) <= 0) 
      {
          cout << "["+DateTimeFunctions::getTimeStamp() +"] " + "ERROR accepting ssl connection" << endl;
          ERR_print_errors_fp(stderr);
      }
      else
      {
        addUser(ssl, sslfd);
        for (;;)  //maybe while?
        {
          bzero(buff, MAXTCP);
          if (! SSL_read(ssl, buff, sizeof(buff)) )
            break;
          cout << ">" << string(buff) << "<" << endl;
          if ( string(buff) == "" )
            break;
          else
          {
            for ( auto& it : TcpServerListener::userHooks )
            {
              if ( it.connFd == sslfd )
              {
                if ( it.authenticated )
                {
                  printClientLog(sslfd, "Message from "+ color::rize(it.userName, "Red")+": " + color::rize(string(buff), "Blue") , "Yellow", "Default");

//                  cout << color::rize("Message from client: " + string(buff), "Yellow") << endl;
                  string result = CommandParser::execute(buff, it.userName, false);
                  sendSingle(sslfd, result);
                  //sendAll(result);
                }
                else
                {
                  vector<string> data = RegexFunctions::split(' ', buff);
                  int i = 0;
                  for ( auto d : data )
                  {
                    if ( i == 0 )
                      it.userName = d;
                    if ( i == 1 )
                    {
                      vector<string> user = RegexFunctions::split('?', it.userName);
                      for ( auto uc : cfr->userCaches )
                      {
                        if ( uc.first == user.at(0)+".cfg" )
                        {
                          if ( d == uc.second->content["password"]["*"] )
                          {
                             if ( std::find(cfr->activeUsers.begin(), cfr->activeUsers.end(), it.userName) == cfr->activeUsers.end() )
                              cfr->activeUsers.push_back(it.userName+".cfg");

                            it.authenticated = true;
                            printClientLog(sslfd, "user autenticated: " + string(it.userName), "Green", "Default");
                          }
                        }
                      }
                    }
                    i++;
                  }
                  if ( it.authenticated )
                  {
                    string availableCommands = "";
                    for ( auto c : cfr->allCommands )
                      availableCommands += "," + c;
                    
                    sendSingle(sslfd, color::rize("successfully logged in on " + getMachineInfo() + " as " + it.userName + " (" + to_string(sslfd) + ")", "Green") + "\n"); 
                    //available commands:" + availableCommands.substr(1) + "\n" );
//                    cout << "["+DateTimeFunctions::getTimeStamp() +"] " + color::rize("successfully logged in as " + it.userName + " (" + to_string(sslfd) + ")", "Green") << endl;
                    printClientLog(sslfd, "successfully logged in as " + it.userName, "Green", "Default");

                  }
                  else
                  {
                    sendSingle(sslfd, color::rize("please authenticate with correct credentials on " + getMachineInfo() + ": <user> <password>", "Red"));
                    //cout << "["+DateTimeFunctions::getTimeStamp() +"] " + color::rize("user with wrong credentials tried to login: " + string(buff), "Red") << endl;
                    printClientLog(sslfd, "(authentication failure) authentication failed " + string(buff), "Red", "Default");  //TODO add fail2ban functionality

                  }
                }
              }
            }
            bzero(buff, MAXTCP);
          }
        }
      }
      removeUser(sslfd);

      SSL_free(ssl);
      close(sslfd);

      cout << "["+DateTimeFunctions::getTimeStamp() +"] " + color::rize("Connection " + to_string(sslfd) + " closed", "White", "Dark Gray") << endl;
    }
    
    static void sendSingle(int sslfd, string message)
    {
      for ( auto it : TcpServerListener::userHooks )
        if ( sslfd == it.connFd )
        {
          int status = 0;
          try
          {
            status = SSL_write(it.ssl, message.c_str(), message.length() );
          }
          catch (...)
          {
            cout << "Error writing to SSL Socket" << endl;
          }
          cout << "["+DateTimeFunctions::getTimeStamp() +"] " + "Answering : " << to_string(status) << "Bytes to socket (" << it.connFd << ")" << endl;      //write will give -1 if closed
        }
    }

    static void sendAll(string message)  //TODO, implement use of this function
    {
      message += color::rize("\n<EOF> Press [ENTER]\n", "Yellow");
      for ( auto it : TcpServerListener::userHooks )
      {
        int status = 0;
        try
        {
          status = SSL_write(it.ssl, message.c_str(), message.length() );
        }
        catch (...)
        {
          cout << "Error writing to SSL Socket" << endl;
        }
        cout << "["+DateTimeFunctions::getTimeStamp() +"] " + "WRITE : " << to_string(status) << " to socket (" << it.connFd << ")" << endl;      //write will give -1 if closed
      }
    }


    static void sendSome(string message, Release rls) //TODO: still does all
    {
      message += color::rize("\n<EOF> Press [ENTER]\n", "Yellow");
      for ( auto it : TcpServerListener::userHooks )
      {
        if (true) //TODO: say wht users
        {
          int status = 0;
          try
          {
            status = SSL_write(it.ssl, message.c_str(), message.length() );
          }
          catch (...)
          {
            cout << "Error writing to SSL Socket" << endl;
          }

          cout << "["+DateTimeFunctions::getTimeStamp() +"] " + "WRITE : " << to_string(status) << " to socket (" << it.connFd << ")" << endl;      //write will give -1 if closed
        }
      }
    }
    
    static void setName(int connfd)
    {
      auto iter = std::find_if(TcpServerListener::userHooks.begin(), TcpServerListener::userHooks.end(),
                        [&](userHook const & uh) {return uh.connFd == connfd;});
      if ( iter != TcpServerListener::userHooks.end() )
      {
        cout << "["+DateTimeFunctions::getTimeStamp() +"] " + to_string(iter->connFd) << " " << iter->userName << " " << iter->authenticated << endl;
      }

    }

    static void addUser(SSL * ssl, int connfd)
    {
      cout << "["+DateTimeFunctions::getTimeStamp() +"] " + color::rize("user with no login added: " + to_string(connfd), "Yellow") << endl;
      userHook uh = { .ssl = ssl, .connFd = connfd, .userName = "NONE", .authenticated = false };
      TcpServerListener::userHooks.push_back(uh);
    }
    
    
    static void removeUser(int connfd) 
    {      
      TcpServerListener::userHooks.erase(
        std::remove_if(TcpServerListener::userHooks.begin(), TcpServerListener::userHooks.end(), [&](userHook const & uh)
        {
          return uh.connFd == connfd;
        } 
      ));

      for ( auto uh : userHooks )
        printClientLog ( (uh).connFd , "Still Active: " + uh.userName + " (" + to_string((uh).connFd) + ")", "Green", "Default" );
      
      for ( auto activeUser : cfr->activeUsers )
      {
        if ( TcpServerListener::debug) cout << "cfr: " << activeUser << endl;
        bool foundUser = false;
        for ( auto userHook : TcpServerListener::userHooks )
        {
          if ( TcpServerListener::debug) cout << "UserHook: " << userHook.userName << endl;
          if ( userHook.userName+".cfg" == activeUser )
            foundUser = true;
        }
        
        if ( !foundUser )
          cfr->activeUsers.erase(
            std::remove(cfr->activeUsers.begin(), cfr->activeUsers.end(), activeUser)
          );
        
  //TODO      cout << "["+DateTimeFunctions::getTimeStamp() +"] " + color::rize("user " + userHook.userName + " removed from activeUsers (" + to_string(connfd) + ")", "Yellow" ) << endl;
      }
    }
};

#endif
