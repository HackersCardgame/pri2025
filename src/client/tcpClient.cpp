#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstdio>
#include <iostream>

#include <termios.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <thread>

#include <readline/readline.h>
#include <readline/history.h>

#include <signal.h>

#include "../lib/colorize/color.hpp"

#include "tcpClientNet.cpp"
#include "tcpClientCmd.cpp"


#ifndef TCP_CLIENT
#define TCP_CLIENT

using namespace std;

int gSockfd;
SSL * gSsl;
bool gConnected = false;
bool answered = false;

/*******************************************************************************
* tcpClient that connects to a phase server, uses SSL
* it is separated in 3 parts
* - tcpClient.cpp:    starts all the elements
* - tcpClientCmd.cpp: does all the input output and command history
* - tcpClientNet.cpp: does all the network related stuff (also SSL)
*******************************************************************************/
class TcpClient
{
  private:

  public:
    inline static const bool debug = false;


    TcpClient(string host, string user, string password)
    {
      TcpClientCmd tcc = TcpClientCmd("");
      thread t2 = thread(tcc.cmd);    

      TcpClientNet(host, user, password, 2592000);
    }

    TcpClient(string host, string user, string password, string script)
    {
      TcpClientCmd tcc = TcpClientCmd(script);
      thread t2 = thread(tcc.cmd);    

      TcpClientNet(host, user, password, 20);
    }
    
    ~TcpClient()
    {
    
    }
};



static void SetStdinEcho(bool enable = true)
{
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  if( !enable )
      tty.c_lflag &= ~ECHO;
  else
      tty.c_lflag |= ECHO;

  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

static void askPassword(string host, string user)
{
  if ( TcpClient::debug ) cout << "askPassword("+user+")" << endl;

  cout << "Password: ";
  std::string password;

  SetStdinEcho(false);
  cin >> password;
  SetStdinEcho(true);
  cout << endl;

  if ( TcpClient::debug ) cout << "TcpClient("+user+", "+password+")" << endl;

  TcpClient tc = TcpClient(host, user, password);
}

static void askUser(string host)
{
  if ( TcpClient::debug ) cout << "askUser()" << endl;

  cout << "User: ";
  string user;
  cin >> user;
  askPassword(host, user);
}


int main(int argc, char** argv)
{
//  std::cout << MESSAGE << std::endl;
  if ( TcpClient::debug ) cout << "main()" << endl;

  if ( argc < 2 )
    askUser("localhost");

  if ( argc == 2 )
    askPassword("localhost", argv[1]);

  if ( argc == 3 )
  {
    if ( string(argv[1]) == "--host" )
    {
      cout << "host specified: "  << color::rize(argv[2], "Blue") << endl;
      string host = string(argv[2]);
      askUser(host);
    }

    if ( string(argv[1]) == "--script" )
    {
      cout << "host specified: "  << color::rize(argv[2], "Blue") << endl;
      string host = string(argv[2]);
      TcpClient tc = TcpClient("localhost", "user01", "123456", "site;who;whoami;t black.panther.2018.x264.1080p.web-ASDF;group terminal;exit");
    }

    TcpClient tc = TcpClient("localhost", argv[1], argv[2]);
  }
}



#endif

