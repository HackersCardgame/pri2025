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

#include "tcpClientCmd.cpp"



using namespace std;

int gSockfd;
SSL * gSsl;
bool gConnected = false;
bool answered = false;


int main(int argc, char** argv)
{
  TcpClientCmd tcc = TcpClientCmd();
  tcc.cmd();
}


